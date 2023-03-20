#ifndef IWINDOW_H
#define IWINDOW_H

#include <QWidget>

#include "Action/ActionItem.h"
#include "Global/CoreGlobal.h"

class QMenuBar;
class QStatusBar;

namespace Core {

    namespace Internal {
        class CorePlugin;
    }

    class IWindow;
    class IWindowFactoryPrivate;

    class CORE_EXPORT IWindowFactory {
    public:
        enum AvailableCreator {
            Create,
            CreateWithId,
        };

        explicit IWindowFactory(const QString &id, AvailableCreator creator);
        virtual ~IWindowFactory();

        QString id() const;
        AvailableCreator creator() const;

        virtual IWindow *create(QObject *parent);
        virtual IWindow *create(const QString &id, QObject *parent);

    private:
        QScopedPointer<IWindowFactoryPrivate> d_ptr;
    };

    class IWindowPrivate;

    class CORE_EXPORT IWindow : public QObject {
        Q_OBJECT
    public:
        QString id() const;
        QWidget *window() const;

        virtual QMenuBar *menuBar() const;
        virtual void setMenuBar(QMenuBar *menuBar);

        virtual QWidget *centralWidget() const;
        virtual void setCentralWidget(QWidget *widget);

        virtual QStatusBar *statusBar() const;
        virtual void setStatusBar(QStatusBar *statusBar);

        void addWidget(const QString &id, QWidget *w);
        void removeWidget(const QString &id);
        QWidget *widget(const QString &id) const;
        QList<QWidget *> widgets() const;

        void addActionItem(const QString &id, ActionItem *item);
        void removeActionItem(const QString &id);
        ActionItem *actionItem(const QString &id) const;
        QList<ActionItem *> actionItems() const;

        void reloadActions();

    signals:
        void aboutToClose();
        void closed();

    protected:
        explicit IWindow(const QString &id, QObject *parent = nullptr);
        ~IWindow();

        virtual QWidget *createWindow(QWidget *parent) const = 0;
        virtual void setupWindow();

    protected:
        IWindow(IWindowPrivate &d, const QString &id, QObject *parent = nullptr);
        QScopedPointer<IWindowPrivate> d_ptr;

        Q_DECLARE_PRIVATE(IWindow)

        friend class ICore;
        friend class ICorePrivate;
        friend class Internal::CorePlugin;
        friend class WindowSystem;
        friend class WindowSystemPrivate;
        friend class IWindowFactory;
    };

}

#endif // IWINDOW_H
