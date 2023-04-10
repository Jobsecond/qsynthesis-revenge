#include "IEMgrAddOn.h"

#include "CoreApi/IWindow.h"
#include "IManager.h"

#include <QMDecorator.h>

namespace IEMgr::Internal {

    using namespace Core;

    bool IEMgrAddOnFactory::predicate(Core::IWindow *handle) const {
        return handle->id() == "home" || handle->id() == "project";
    }

    Core::IWindowAddOn *IEMgrAddOnFactory::create(QObject *parent) {
        return new IEMgrAddOn(parent);
    }

    IEMgrAddOn::IEMgrAddOn(QObject *parent) : Core::IWindowAddOn(parent) {
    }

    IEMgrAddOn::~IEMgrAddOn() {
    }

    void IEMgrAddOn::initialize() {
        auto iWin = windowHandle();

        initActions();

        qIDec->installLocale(this, {{}}, _LOC(IEMgrAddOn, this));
    }

    void IEMgrAddOn::extensionsInitialized() {
    }

    void IEMgrAddOn::reloadStrings() {
        importGroupItem->setText(tr("Import Actions"));
        importItem->setText(tr("Import"));

        exportGroupItem->setText(tr("Export Actions"));
        exportItem->setText(tr("Export"));
    }

    void IEMgrAddOn::initActions() {
        auto iWin = windowHandle();

        importGroupItem = new ActionItem("iemgr.ImportGroup", new QActionGroup(this));
        importItem = new ActionItem("iemgr.Import", new QAction());

        exportGroupItem = new ActionItem("iemgr.ExportGroup", new QActionGroup(this));
        exportItem = new ActionItem("iemgr.Export", new QAction());

        connect(importItem->action(), &QAction::triggered, this, [this]() {
            IManager::instance()->runImport({}, windowHandle()->window()); //
        });

        connect(exportItem->action(), &QAction::triggered, this, [this]() {
            IManager::instance()->runExport({}, windowHandle()->window()); //
        });

        iWin->addActionItems({
            importGroupItem,
            importItem,
            exportGroupItem,
            exportItem,
        });
    }
}