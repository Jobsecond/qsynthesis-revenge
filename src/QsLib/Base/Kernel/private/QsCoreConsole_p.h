#ifndef QSCORECONSOLEPRIVATE_H
#define QSCORECONSOLEPRIVATE_H

#include "../QsCoreConsole.h"

class QSBASE_API QsCoreConsolePrivate {
    Q_DECLARE_PUBLIC(QsCoreConsole)
public:
    QsCoreConsolePrivate();
    virtual ~QsCoreConsolePrivate();

    void init();

#if defined(Q_OS_WINDOWS) || defined(Q_OS_MAC)
    void osMessageBox_helper(void *winHandle, QsCoreConsole::MessageBoxFlag flag,
                             const QString &title, const QString &text);
#endif

    QsCoreConsole *q_ptr;
};

#endif // QSCORECONSOLEPRIVATE_H