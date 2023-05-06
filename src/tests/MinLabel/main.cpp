#include "gui/MainWindow.h"

#include "QMSystem.h"

#include <QApplication>
#include <QMessageBox>

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    if (QMOs::isUserRoot() && !a.arguments().contains("--allow-root")) {
        QString title = qAppName();
        QString msg = QString("You're trying to start %1 as the %2, which may cause "
                              "security problem and isn't recommended.")
                          .arg(qAppName(), "Administrator");
#ifdef Q_OS_WINDOWS
        ::MessageBoxW(0, msg.toStdWString().data(), title.toStdWString().data(),
                      MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_ICONWARNING);
#elif Q_OS_LINUX
        fputs(qPrintable(msg), stdout);
#else
        QMessageBox::warning(nullptr, title, msg, QApplication::tr("Confirm"));
#endif
        return 0;
    }

    QFont f("Microsoft YaHei");
    f.setPointSize(9);
    a.setFont(f);

    // Set library loading info
    qApp->addLibraryPath(qApp->applicationDirPath() + "/../lib/QsLib/plugins/");

    MainWindow w;
    w.show();

    return a.exec();
}
