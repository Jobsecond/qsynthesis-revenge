#include "DsStartInfo.h"
#include "private/DsStartInfo_p.h"

#include "DsConsole.h"
#include "DsDistConfig.h"

#include <QApplication>

DsStartInfo::DsStartInfo(QObject *parent) : DsStartInfo(*new DsStartInfoPrivate(), parent) {
    appDescription = "DiffSinger editor maintained by OpenVPI";
}

DsStartInfo::~DsStartInfo() {
}

void DsStartInfo::load() {
    Q_D(DsStartInfo);
    QsStartInfo::load();
    d->load_helper();
}

QsCoreConsole *DsStartInfo::createConsole(QObject *parent) {
    return new DsConsole(parent);
}

QsCoreConfig *DsStartInfo::creatDistConfig() {
    return new DsDistConfig();
}

DsStartInfo::DsStartInfo(DsStartInfoPrivate &d, QObject *parent) : QsStartInfo(d, parent) {
    d.init();
}
