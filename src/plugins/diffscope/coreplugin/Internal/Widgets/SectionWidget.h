#ifndef CHORUSKIT_SECTIONWIDGET_H
#define CHORUSKIT_SECTIONWIDGET_H

#include <QFrame>

#include <QPixelSize.h>

namespace Core::Internal {

    class SectionWidget : public QFrame {
        Q_OBJECT
        Q_PROPERTY(QPixelSize preferredHeight READ preferredHeight WRITE setPreferredHeight)
    public:
        explicit SectionWidget(QWidget *parent = nullptr);
        ~SectionWidget();

        QPixelSize preferredHeight() const;
        void setPreferredHeight(const QPixelSize &height);

        QSize sizeHint() const override;

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        int m_preferredHeight;
    };

}


#endif // CHORUSKIT_SECTIONWIDGET_H
