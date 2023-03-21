//
// Created by Functioner on 2023/3/19.
//

#ifndef PROJECTWINDOWADDON_H
#define PROJECTWINDOWADDON_H


#include "Window/IWindowAddOn.h"

namespace Core {

    namespace Internal {

        class ProjectWindowAddOnFactory : public IWindowAddOnFactory {
        public:
            bool predicate(IWindow *iWindow) const override;
            IWindowAddOn *create(QObject *parent) override;
        };

        class ProjectWindowAddOn : public IWindowAddOn {
            Q_OBJECT
        public:
            explicit ProjectWindowAddOn(QObject *parent = nullptr);
            ~ProjectWindowAddOn();

            void initialize() override;
            void extensionsInitialized() override;
        };

    }

}

#endif // PROJECTWINDOWADDON_H