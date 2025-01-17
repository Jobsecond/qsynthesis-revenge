#include "SectionWidget.h"

namespace Core::Internal {

    SectionWidget::SectionWidget(QWidget *parent) : QFrame(parent) {
        m_preferredHeight = 30;
    }

    SectionWidget::~SectionWidget() {
    }

    QPixelSize SectionWidget::preferredHeight() const {
        return m_preferredHeight;
    }

    void SectionWidget::setPreferredHeight(const QPixelSize &height) {
        m_preferredHeight = height;
        updateGeometry();
    }
    QSize SectionWidget::sizeHint() const {
        return {QFrame::sizeHint().width(), m_preferredHeight};
    }

    void SectionWidget::paintEvent(QPaintEvent *event) {
        QFrame::paintEvent(event);
    }

}