#include "IManager.h"
#include "IManager_p.h"

#include <coreplugin/ICore.h>

#include "IImportWizard_p.h"

#include "Internal/Dialogs/ImportInitDialog.h"

namespace IEMgr {

    IManagerPrivate::IManagerPrivate() {
        running = false;
    }

    IManagerPrivate::~IManagerPrivate() {
    }

    void IManagerPrivate::init() {
    }

    static IManager *m_instance = nullptr;

    IManager *IManager::instance() {
        return m_instance;
    }

    bool IManager::addImportWizard(IImportWizard *factory) {
        Q_D(IManager);
        if (!factory) {
            qWarning() << "IEMgr::IManager::addImportWizard(): trying to add null factory";
            return false;
        }
        if (d->importWizards.contains(factory->id())) {
            qWarning() << "IEMgr::IManager::addImportWizard(): trying to add duplicated factory:" << factory->id();
            return false;
        }
        factory->setParent(this);
        d->importWizards.append(factory->id(), factory);
        return true;
    }

    bool IManager::removeImportWizard(IImportWizard *factory) {
        if (factory == nullptr) {
            qWarning() << "IEMgr::IManager::removeImportWizard(): trying to remove null factory";
            return false;
        }
        return removeImportWizard(factory->id());
    }

    bool IManager::removeImportWizard(const QString &id) {
        Q_D(IManager);
        auto it = d->importWizards.find(id);
        if (it == d->importWizards.end()) {
            qWarning() << "IEMgr::IManager::removeImportWizard(): factory does not exist:" << id;
            return false;
        }
        it.value()->setParent(nullptr);
        d->importWizards.erase(it);
        return true;
    }

    QList<IImportWizard *> IManager::importWizards() const {
        Q_D(const IManager);
        return d->importWizards.values();
    }

    void IManager::clearImportWizards() {
        Q_D(IManager);
        d->importWizards.clear();
    }

    void IManager::runImport(const QString &id, QWidget *parent) {
        Q_D(IManager);

        auto iWin = Core::ICore::instance()->windowSystem()->findWindow(parent->window());
        if (!iWin) {
            return;
        }

        if (d->running) {
            return;
        }

        d->running = true;

        WizardContextPrivate context_d{QDateTime::currentDateTime(), iWin};
        WizardContext context(&context_d);

        Internal::ImportInitDialog dlg(parent);
        if (!id.isEmpty()) {
            auto wizard = d->importWizards.value(id, nullptr);
            if (wizard)
                dlg.selectWizard(wizard);
        }

        int code;
        do {
            code = dlg.exec();
        } while (code == QDialog::Accepted && !dlg.currentWizard()->runWizard(&context));

        d->running = false;
    }

    void IManager::runExport(const QString &id, QWidget *parent) {
    }

    bool IManager::isRunning() const {
        Q_D(const IManager);
        return d->running;
    }

    IManager::IManager(QObject *parent) : IManager(*new IManagerPrivate(), parent) {
    }

    IManager::~IManager() {
        m_instance = nullptr;
    }

    IManager::IManager(IManagerPrivate &d, QObject *parent) : QObject(parent), d_ptr(&d) {
        m_instance = this;
        d.q_ptr = this;

        d.init();
    }

}
