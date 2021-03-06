// Copyright 2019 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "ui_perfstats-page.h"

#include <memory>

class PerfStatsPage : public QFrame {
    Q_OBJECT

public:
    explicit PerfStatsPage(QWidget* parent = 0);
    ~PerfStatsPage();

    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

signals:

    void windowVisible();
    void windowHidden();

public slots:

    void enableCollection();
    void disableCollection();

private:
    std::unique_ptr<Ui::PerfStatsPage> mUi;
};