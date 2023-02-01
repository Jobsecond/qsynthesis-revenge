#include "QsCoreStartInfo.h"
#include "private/QsCoreStartInfo_p.h"

#include <QCoreApplication>
#include <QMetaType>

#include "QsCoreDecorator.h"
#include "QsCoreConfig.h"

Q_SINGLETON_DECLARE(QsCoreStartInfo)

QsCoreStartInfo::QsCoreStartInfo(QObject *parent) : QsCoreStartInfo(*new QsCoreStartInfoPrivate(), parent) {
}

QsCoreStartInfo::~QsCoreStartInfo() {
}

void QsCoreStartInfo::parse() {
    Q_D(QsCoreStartInfo);
    d->parse_helper();
}

QString QsCoreStartInfo::mainTitle() const {
    return qApp->applicationName();
}

QString QsCoreStartInfo::windowTitle() const {
    return mainTitle() + QString(" %1").arg(qApp->applicationVersion());
}

QString QsCoreStartInfo::errorTitle() const {
    return QCoreApplication::tr("Error");
}

QsCoreDecorator *QsCoreStartInfo::createDecorator(QObject *parent) {
    return new QsCoreDecorator(parent);
}

QsCoreConfig *QsCoreStartInfo::creatDistConfig() {
    return new QsCoreConfig();
}

QsCoreStartInfo::QsCoreStartInfo(QsCoreStartInfoPrivate &d, QObject *parent)
    : QObject(parent), d_ptr(&d) {
    construct();

    d.q_ptr = this;
    d.init();

    allowRootUser = false;
    allowSecondary = false;

    parser.addHelpOption();
    parser.addVersionOption();
}

void QsCoreStartInfo::newStartedInstance() {
}

void QsCoreStartInfo::receiveMessage(quint32 instanceId, const QByteArray &message) {
}

void QsCoreStartInfo::_q_instanceStarted() {
    newStartedInstance();
}

void QsCoreStartInfo::_q_messageReceived(quint32 instanceId, QByteArray message) {
    receiveMessage(instanceId, message);
}