#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include "Windows/PlainWindow.h"

class ProjectWindowPrivate;

class ProjectWindow : public PlainWindow {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProjectWindow)
public:
    ProjectWindow(QWidget *parent = nullptr);
    ~ProjectWindow();

protected:
    ProjectWindow(ProjectWindowPrivate &d, QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;
};

#endif // PROJECTWINDOW_H