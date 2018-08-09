// Copyright 2018 The Android Open Source Project
//
// This software is licensed under the terms of the GNU General Public
// License version 2, as published by the Free Software Foundation, and
// may be copied, distributed, and modified under those terms.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <errno.h>
#include <cstdio>

#include "android/base/files/FileShareOpen.h"
#include "android/base/files/FileShareOpenImpl.h"
#include "android/base/StringFormat.h"

void android::base::createFileForShare(const char* filename) {
    void* handle = internal::openFileForShare(filename);
    if (handle) {
        internal::closeFileForShare(handle);
    }
}

#ifdef _WIN32

#include "android/base/system/Win32UnicodeString.h"

#include <share.h>
#include <windows.h>

FILE* android::base::fsopen(const char* filename,
                            const char* mode,
                            android::base::FileShare fileshare) {
    int shflag = _SH_DENYWR;
    switch (fileshare) {
        case FileShare::Read:
            // Others cannot write
            shflag = _SH_DENYWR;
            break;
        case FileShare::Write:
            // Others cannot read nor write
            shflag = _SH_DENYRW;
            break;
    }
    const Win32UnicodeString filenameW(filename);
    const Win32UnicodeString modeW(mode);
    FILE* file = _wfsopen(filenameW.c_str(), modeW.c_str(), shflag);
    if (!file) {
        fprintf(stderr, "%s open failed errno %d\n", filename, errno);
    }
    return file;
}

bool android::base::updateFileShare(FILE* file, FileShare fileshare) {
    // TODO: have windows support
    // BUG: 112265408
    fprintf(stderr, "Error: updateFileShare not supported on windows\n");
    return false;
}

void* android::base::internal::openFileForShare(const char* filename) {
    const Win32UnicodeString filenameW(filename);
    void* hndl = CreateFileW(filenameW.c_str(), 0,
            FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hndl != INVALID_HANDLE_VALUE) {
        return hndl;
    } else {
        return nullptr;
    }
}

void android::base::internal::closeFileForShare(void* fileHandle) {
    CloseHandle(fileHandle);
}

#else
#include <sys/file.h>
#include <unistd.h>

static int getFlockOperation(android::base::FileShare fileshare) {
    switch (fileshare) {
        case android::base::FileShare::Read:
            return LOCK_SH;
        case android::base::FileShare::Write:
            return LOCK_EX;
        default:
            return LOCK_SH;
    }
}

FILE* android::base::fsopen(const char* filename,
                            const char* mode,
                            android::base::FileShare fileshare) {
    std::string tmp;
    // "e" is a glibc extension from glibc 2.7, it open the file with
    // O_CLOEXEC flag. It doesn't work for macOS/Windows actually but it
    // also doesn't hurt.
    if (!strchr(mode, 'e')) {
        tmp = StringFormat("%se", mode);
        mode = tmp.c_str();
    }
    FILE* file = fopen(filename, mode);
    if (!file) {
        return nullptr;
    }
    int operation = getFlockOperation(fileshare);
    int fd = fileno(file);
    if (flock(fd, operation | LOCK_NB) == -1) {
        fclose(file);
        fprintf(stderr, "%s lock failed errno %d\n", filename, errno);
        return nullptr;
    }
    return file;
}

bool android::base::updateFileShare(FILE* file, FileShare fileshare) {
    int operation = getFlockOperation(fileshare);
    int fd = fileno(file);
    return -1 != flock(fd, operation | LOCK_NB);
}

void* android::base::internal::openFileForShare(const char* filename) {
    return fopen(filename, "a");
}

void android::base::internal::closeFileForShare(void* fileHandle) {
    fclose((FILE*)fileHandle);
}

#endif