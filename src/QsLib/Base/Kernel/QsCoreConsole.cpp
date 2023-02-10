#include "QsCoreConsole.h"
#include "private/QsCoreConsole_p.h"

#include "QsFileManager.h"
#include "QsPluginManager.h"

#include "QsSystem.h"

Q_SINGLETON_DECLARE(QsCoreConsole);

QsCoreConsole::QsCoreConsole(QObject *parent) : QsCoreConsole(*new QsCoreConsolePrivate(), parent) {
}

QsCoreConsole::~QsCoreConsole() {
}

void QsCoreConsole::load() {
    Q_D(QsCoreConsole);
    d->load_helper();
}

void QsCoreConsole::save() {
    Q_D(QsCoreConsole);
    d->save_helper();
}

void QsCoreConsole::MsgBox(QObject *parent, QsCoreConsole::MessageBoxFlag flag,
                           const QString &title, const QString &text) {
    Q_UNUSED(parent);
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MAC)
    Q_D(QsCoreConsole);
    d->osMessageBox_helper(nullptr, flag, title, text);
#else
    switch (flag) {
        case Critical:
        case Warning:
            fputs(qPrintable(text), stderr);
            break;
        case Question:
        case Information:
            fputs(qPrintable(text), stdout);
            break;
    };
#endif
}

void QsCoreConsole::SelectBox(QObject *parent, bool supportPreview, int max, const QString &title,
                              const QString &caption,
                              const QList<QsCoreConsole::SelectOption> &arguments) {
}
QsPluginManager *QsCoreConsole::createPluginManager(QObject *parent) {
    return new QsPluginManager(parent);
}


QsFileManager *QsCoreConsole::createFileManager(QObject *parent) {
    return new QsFileManager(parent);
}

QsCoreConsole::QsCoreConsole(QsCoreConsolePrivate &d, QObject *parent)
    : QObject(parent), d_ptr(&d) {
    construct();

    d.q_ptr = this;
    d.init();
}
