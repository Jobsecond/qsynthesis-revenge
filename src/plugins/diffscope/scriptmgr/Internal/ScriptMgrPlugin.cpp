#include "ScriptMgrPlugin.h"

#include <QApplication>
#include <QJSEngine>
#include <QThread>

#include "CoreApi/ILoader.h"

#include <QMDecoratorV2.h>
#include <QMSystem.h>

#include <coreplugin/ICore.h>
#include <extensionsystem/pluginspec.h>

#include "AddOn/ScriptMgrAddOn.h"
#include "ScriptLoader.h"
#include "ScriptSettingsPage.h"

namespace ScriptMgr {

    using namespace Core;

    namespace Internal {

        ScriptMgrPlugin::ScriptMgrPlugin() {
        }

        ScriptMgrPlugin::~ScriptMgrPlugin() {
        }

        static ScriptLoader *scriptLoader = nullptr;

        bool ScriptMgrPlugin::initialize(const QStringList &arguments, QString *errorMessage) {
            // Add resources
            qIDec->addTranslationPath(pluginSpec()->location() + "/translations");

            auto splash = qobject_cast<QSplashScreen *>(ILoader::instance()->getFirstObject("choruskit_init_splash"));
            if (splash) {
                splash->showMessage(tr("Initializing script manager..."));
            }

            auto actionMgr = ICore::instance()->actionSystem();
            if (actionMgr->loadContexts(":/scriptmgr_actions.xml").isEmpty()) {
                *errorMessage = tr("Failed to load action configuration!");
                return false;
            }

            auto sc = ICore::instance()->settingCatalog();
            sc->addPage(new ScriptSettingsPage);

            // Add basic windows and add-ons
            new ScriptLoader(this);
            ScriptLoader::instance()->reloadEngine();

            auto winMgr = ICore::instance()->windowSystem();
            winMgr->addAddOn(new ScriptMgrAddOnFactory());

            return true;
        }

        void ScriptMgrPlugin::extensionsInitialized() {
        }

        bool ScriptMgrPlugin::delayedInitialize() {
            return false;
        }

    }

}