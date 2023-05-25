#include "QMMath.h"

#include <QDebug>
#include <QSet>
#include <QtMath>

#include <cstdio>

QString QMMath::removeSideQuote(const QString &token) {
    auto str = token;
    if (str.front() == '\"') {
        str.remove(0, 1);
    }
    if (str.back() == '\"') {
        str.remove(str.size() - 1, 1);
    }
    return str;
}

QList<int> QMMath::toIntList(const QStringList &list) {
    QList<int> res;
    for (const auto &item : list) {
        bool isNum;
        int num = item.toInt(&isNum);
        if (!isNum) {
            return {};
        }
        res.append(num);
    }
    return res;
}

QList<double> QMMath::toDoubleList(const QStringList &list) {
    QList<double> res;
    for (auto it = list.begin(); it != list.end(); ++it) {
        bool isNum;
        int num = it->toDouble(&isNum);
        if (!isNum) {
            return {};
        }
        res.append(num);
    }
    return res;
}

QStringList QMMath::arrayToStringList(const QJsonArray &arr, bool considerNumber) {
    QStringList res;
    for (const auto &item : arr)
        if (item.isString())
            res.append(item.toString());
        else if (item.isDouble() && considerNumber)
            res.append(QString::number(item.toDouble()));
    return res;
}

bool QMMath::isNumber(const QString &s, bool considerDot, bool considerNeg) {
    bool flag = true;

    for (int i = 0; i < s.size(); ++i) {
        QChar ch = s.at(i);
        if ((ch >= '0' && ch <= '9') || (considerDot && ch == '.') || (considerNeg && ch == '-')) {
            // is Number
        } else {
            flag = false;
            break;
        }
    }

    return flag;
}

double QMMath::euclideanDistance(const QPoint &p1, const QPoint &p2) {
    return qSqrt(qPow(p1.x() - p2.x(), 2) + qPow(p1.y() - p2.y(), 2));
}

QStringList QMMath::splitAll(const QString &str, const QChar &delim) {
    QStringList res;
    QString buf;
    for (int i = 0; i < str.size(); ++i) {
        auto cur = str.at(i);
        if (cur == delim) {
            if (!buf.isEmpty()) {
                res.append(buf);
                buf.clear();
            }
            continue;
        }
        buf += cur;
    }
    if (!buf.isEmpty()) {
        res.append(buf);
    }
    return res;
}

QString QMMath::adjustRepeatedName(const QSet<QString> &set, const QString &name) {
    if (!set.contains(name)) {
        return name;
    }

    QString body;
    int num;

    bool failed = false;

    // Check if there's a formatted suffix
    int index = name.lastIndexOf(" (");
    if (index >= 0) {
        QString suffix = name.mid(index);

        // Get suffix index
        const char fmt[] = " (%d)%c";
        char ch;
        int n = ::sscanf(suffix.toUtf8().data(), fmt, &num, &ch);
        if (n != 1) {
            failed = true;
        } else {
            body = name.left(index);
        }
    } else {
        failed = true;
    }

    if (failed) {
        body = name;
        num = 0;
    }

    QString newName;
    while (true) {
        num++;
        newName = body + QString(" (%1)").arg(QString::number(num));
        if (!set.contains(newName)) {
            break;
        }
    };

    return newName;
}
