//
// Created by Functioner on 2023/3/16.
//

#ifndef IWINDOWADDON_H
#define IWINDOWADDON_H

#include <QObject>

#include "Global/CoreGlobal.h"

namespace Core {

    class IWindow;
    class IWindowAddOn;
    class IWindowAddOnPrivate;

    class CORE_EXPORT IWindowAddOnFactory {
    public:
        virtual bool predicate(IWindow *iWindow) const;
        virtual IWindowAddOn *create(QObject *parent) = 0;
    };

    class CORE_EXPORT IWindowAddOn : public QObject {
        Q_OBJECT
    public:
        explicit IWindowAddOn(QObject *parent = nullptr);
        ~IWindowAddOn();

        IWindow *iWin() const;

    public:
        virtual bool initialize() = 0;
        virtual void extensionsInitialized() = 0;
        virtual bool delayedInitialize() = 0;

    protected:
        IWindowAddOn(IWindowAddOnPrivate &d, QObject *parent = nullptr);

        QScopedPointer<IWindowAddOnPrivate> d_ptr;
    };

}



#endif // IWINDOWADDON_H