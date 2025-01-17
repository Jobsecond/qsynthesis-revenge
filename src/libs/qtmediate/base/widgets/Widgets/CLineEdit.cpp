#include "CLineEdit.h"

#include <QDebug>
#include <QEvent>
#include <QTimer>

CLineEdit::CLineEdit(QWidget *parent) : QLineEdit(parent) {
}

CLineEdit::CLineEdit(const QString &text, QWidget *parent) : QLineEdit(text, parent) {
}

CLineEdit::~CLineEdit() {
}

static bool isBlack(const QColor &color) {
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    return red == 0 && green == 0 && blue == 0;
}

bool CLineEdit::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::StyleChange: {
            auto palette = this->palette();
            if (isBlack(palette.brush(QPalette::Active, QPalette::PlaceholderText).color()) &&
                !isBlack(palette.brush(QPalette::Active, QPalette::Text).color())) {
                palette.setBrush(QPalette::Active, QPalette::PlaceholderText, {}); // Set colors twice
                setPalette(palette);
            }
            break;
        }
        default:
            break;
    }
    return QLineEdit::event(event);
}
