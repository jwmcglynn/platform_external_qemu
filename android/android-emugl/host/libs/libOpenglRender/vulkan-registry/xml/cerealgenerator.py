#!/usr/bin/python3 -i
#
# Copyright (c) 2013-2018 The Khronos Group Inc.
# Copyright (c) 2013-2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os, re, sys
from generator import *

# CerealGenerator - generates complete set of encoder, and decoder source
# while being agnostic to the stream implementation

# ---- methods overriding base class ----
# beginFile(genOpts)
# endFile()
# beginFeature(interface, emit)
# endFeature()
# genCmd(cmdinfo)
class CerealGenerator(OutputGenerator):
    """Generate serialization code"""
    def __init__(self, errFile = sys.stderr,
                       warnFile = sys.stderr,
                       diagFile = sys.stdout):
        OutputGenerator.__init__(self, errFile, warnFile, diagFile)

        self.moduleHeaderFileHandles = {}
        self.moduleImplFileHandles = {}

        self.moduleHeaderFilePreambles = {}
        self.moduleImplFilePreambles = {}

        self.indentLevel = 0
        self.code = ""

        self.cereal_Android_mk_header = """
LOCAL_PATH := $(call my-dir)

# For Vulkan libraries

cereal_C_INCLUDES := \\
    $(LOCAL_PATH) \\
    $(EMUGL_PATH)/host/include/vulkan \\

cereal_STATIC_LIBRARIES := \\
    android-emu \\
    android-emu-base \\
"""

        self.cereal_Android_mk_body = """
$(call emugl-begin-static-library,lib$(BUILD_TARGET_SUFFIX)OpenglRender_vulkan_cereal_guest)

LOCAL_C_INCLUDES += $(cereal_C_INCLUDES)

LOCAL_STATIC_LIBRARIES += $(cereal_STATIC_LIBRARIES)

LOCAL_SRC_FILES := \\
    guest/goldfish_vk_frontend.cpp \\

$(call emugl-end-module)
"""

        self.moduleHeaderFilePreambles[self.moduleKey("guest", "goldfish_vk_frontend")] = """
#include <vulkan.h>
"""

        self.moduleImplFilePreambles[self.moduleKey("guest", "goldfish_vk_frontend")] = """
#include "goldfish_vk_frontend.h"

#include "goldfish_vk_encoder.h"
"""

################################################################################
    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)

        write(self.cereal_Android_mk_header, file = self.outFile)

        write(self.cereal_Android_mk_body, file = self.outFile)

        self.beginModule("guest", "goldfish_vk_frontend")
        self.beginModule("guest", "goldfish_vk_encoder")

    def endFile(self):
        OutputGenerator.endFile(self)
    def beginFeature(self, interface, emit):
        # Start processing in superclass
        OutputGenerator.beginFeature(self, interface, emit)
        self.appendHeader("guest", "goldfish_vk_frontend", "#ifdef %s\n" % self.featureName)
        self.appendImpl("guest", "goldfish_vk_frontend", "#ifdef %s\n" % self.featureName)
        self.appendHeader("guest", "goldfish_vk_encoder", "#ifdef %s\n" % self.featureName)
        self.appendImpl("guest", "goldfish_vk_encoder", "#ifdef %s\n" % self.featureName)

    def endFeature(self):
        # Finish processing in superclass
        OutputGenerator.endFeature(self)
        self.appendHeader("guest", "goldfish_vk_frontend", "#endif\n")
        self.appendImpl("guest", "goldfish_vk_frontend", "#endif\n")
        self.appendHeader("guest", "goldfish_vk_encoder", "#endif\n")
        self.appendImpl("guest", "goldfish_vk_encoder", "#endif\n")

    def moduleKey(self, directory, basename):
        return os.path.join(directory, basename)

################################################################################
    def beginModule(self, directory, basename):
        # Create subdirectory, if needed
        absDir = os.path.join(self.genOpts.directory, directory)
        self.makeDir(absDir)

        filename = os.path.join(absDir, basename)

        fpHeader = open(filename + ".h", 'w', encoding='utf-8')
        write("// Module: %s (header) Autogenerated by CerealGenerator\n" % basename, file = fpHeader)

        fpImpl = open(filename + ".cpp", 'w', encoding='utf-8')
        write("// Module: %s (impl) Autogenerated by CerealGenerator\n" % basename, file = fpImpl)

        key = self.moduleKey(directory, basename)

        self.moduleHeaderFileHandles[key] = fpHeader
        self.moduleImplFileHandles[key] = fpImpl

        if key in self.moduleHeaderFilePreambles:
            write(self.moduleHeaderFilePreambles[key], file = fpHeader)

        if key in self.moduleImplFilePreambles:
            write(self.moduleImplFilePreambles[key], file = fpImpl)

    def endModule(self, directory, basename):
        self.moduleHeaderFileHandles[self.moduleKey(directory, basename)].close()
        self.moduleImplFileHandles[self.moduleKey(directory, basename)].close()

    def appendHeader(self, directory, basename, toAppend):
        write(toAppend, file = self.moduleHeaderFileHandles[self.moduleKey(directory, basename)])

    def appendImpl(self, directory, basename, toAppend):
        write(toAppend, file = self.moduleImplFileHandles[self.moduleKey(directory, basename)])

################################################################################

    def swapCode(self,):
        res = "%s" % self.code
        self.code = ""
        return res

    def indent(self,):
        return "".join("    " * self.indentLevel)

    def beginBlock(self,):
        self.code += self.indent() + "{\n"
        self.indentLevel += 1

    def endBlock(self,):
        self.indentLevel -= 1
        self.code += self.indent() + "}\n"

    def beginIf(self, cond):
        self.code += self.indent() + "if (" + cond + ")\n"
        self.beginBlock()

    def beginElse(self, cond):
        self.code += self.indent() + "else\n"
        self.beginBlock()

    def endElse(self):
        self.endBlock()

    def endIf(self):
        self.endBlock()

    def beginWhile(self, cond):
        self.code += self.indent() + "while (" + cond + ")\n"
        self.beginBlock()

    def endWhile(self):
        self.endBlock()

    def beginFor(self, initial, condition, increment):
        self.code += self.indent() + "for (" + "; ".join([initial, condition, increment]) + ")\n"
        self.beginBlock()

    def endFor(self, initial, condition, increment):
        self.endBlock()

    def stmt(self, code):
        self.code += self.indent() + code + ";\n"

    def line(self, code):
        self.code += self.indent() + code + "\n"

    def makeFuncProto(self, cmdInfoElem, name, callNamePrefix = "", extraArgs = None, retTypeOverride = None):
        params = cmdInfoElem.findall('param')

        if extraArgs:
            args = list(extraArgs)
        else:
            args = []

        for p in params:
            args.append(self.makeCParamDecl(p, 0).strip())

        callName = callNamePrefix + name

        if retTypeOverride:
            retType = retTypeOverride
        else:
            retType = cmdInfoElem.find('proto').find('type').text

        divider = "\n    "
        funcPrototype = retType + " " + callName + "(" + divider + ("," + divider).join(args) + ")\n"

        return funcPrototype

    def makeValidateProto(self, cmdInfoElem, name):
        return self.makeFuncProto(
            cmdInfoElem, name, "validate_",
            extraArgs = ["VkResult* validateResult"],
            retTypeOverride = "void")

    def makeEncodeProto(self, cmdInfoElem, name):
        return self.makeFuncProto(
            cmdInfoElem, name, "encode_",
            extraArgs = ["void* vkStream"])

    def makeValidateDef(self, cmdInfoElem, name):
        params = cmdInfoElem.findall('param')

        funcPrototype = \
            "static " + \
            self.makeValidateProto(cmdInfoElem, name)

        self.swapCode();

        self.beginBlock();

        self.endBlock();
        return funcPrototype + self.swapCode()

    def makeEncoderDef(self, cmdInfoElem, name):
        params = cmdInfoElem.findall('param')

        funcPrototype = \
            self.makeEncodeProto(cmdInfoElem, name)

        self.swapCode();

        self.beginBlock();

        retType = cmdInfoElem.find('proto').find('type').text

        if retType != "void":
            self.stmt("return (%s)0" % retType)

        self.endBlock();
        return funcPrototype + self.swapCode()

    def makeValidateCall(self, cmdInfoElem, name):
        params = cmdInfoElem.findall('param')

        args = ["&validateResult"]
        for p in params:
            ptype = p.find('type')
            pname = p.find('name')
            args.append(pname.text)

        callName = "validate_" + name

        return callName + "(" + ", ".join(args) + ")"

    def makeFrontendDef(self, cmdInfoElem, name):
        def makeEncoderCall(cmdInfoElem, name):
            params = cmdInfoElem.findall('param')

            args = ["vkStream"]
            for p in params:
                ptype = p.find('type')
                pname = p.find('name')
                args.append(pname.text)

            callName = "encode_" + name

            return callName + "(" + ", ".join(args) + ")"

        prototype = self.makeFuncProto(cmdInfoElem, name, "goldfish_frontend_") + "\n"

        self.swapCode()

        self.beginBlock()

        self.stmt("VkResult validateResult = VK_SUCCESS")

        retType = cmdInfoElem.find('proto').find('type').text

        self.stmt(self.makeValidateCall(cmdInfoElem, name))

        self.beginIf("validateResult != VK_SUCCESS")
        if retType == 'VkResult':
            self.stmt("return validateResult")
        elif retType != "void":
            self.stmt("return (%s)0" % retType)
        else:
            self.stmt("return")
        self.endIf()

        self.stmt("// VULKAN_STREAM_GET()")
        self.stmt("void* vkStream = 0")

        if retType != 'void':
            self.stmt(retType + " res = " + makeEncoderCall(cmdInfoElem, name))
            self.stmt("return res")
        else:
            self.stmt(makeEncoderCall(cmdInfoElem, name))

        self.endBlock()

        implBody = self.swapCode()
        res = prototype + implBody

        return res

    # Command generation
    def genCmd(self, cmdinfo, name, alias):
        OutputGenerator.genCmd(self, cmdinfo, name, alias)
        frontendDecl = self.makeFuncProto(cmdinfo.elem, name, "goldfish_frontend_") + ";\n"

        validateDef = self.makeValidateDef(cmdinfo.elem, name)
        frontendDef = self.makeFrontendDef(cmdinfo.elem, name)

        self.appendHeader("guest", "goldfish_vk_frontend", frontendDecl)
        self.appendImpl("guest", "goldfish_vk_frontend", validateDef)
        self.appendImpl("guest", "goldfish_vk_frontend", frontendDef)

        encoderDecl = self.makeEncodeProto(cmdinfo.elem, name) + ";\n"
        encoderDef = self.makeEncoderDef(cmdinfo.elem, name)

        self.appendHeader("guest", "goldfish_vk_encoder", encoderDecl)
        self.appendImpl("guest", "goldfish_vk_encoder", encoderDef)