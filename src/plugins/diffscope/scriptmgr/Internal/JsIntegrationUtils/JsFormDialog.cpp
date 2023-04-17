//
// Created by Crs_1 on 2023/4/17.
//

#include "JsFormDialog.h"
#include "CTabButton.h"
#include "QMEqualBoxLayout.h"
#include <QPushButton>

namespace ScriptMgr::Internal {

    JsFormDialog::JsFormDialog(QWidget *parent) : ConfigurableDialog(parent) {
        setApplyButtonVisible(false);
    }

    JsFormDialog::~JsFormDialog() noexcept {
    }

}