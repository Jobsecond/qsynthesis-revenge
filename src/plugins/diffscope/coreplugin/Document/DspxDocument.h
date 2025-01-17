#ifndef DSPXDOCUMENT_H
#define DSPXDOCUMENT_H

#include <ItemModel/AceTreeTransaction.h>

#include <CoreApi/IDocument.h>

#include "coreplugin/Document/Entities/DspxRootEntity.h"

namespace Core {

    class DspxDocumentPrivate;

    class CORE_EXPORT DspxDocument : public IDocument {
        Q_OBJECT
        Q_DECLARE_PRIVATE(DspxDocument)
    public:
        explicit DspxDocument(QObject *parent = nullptr);
        ~DspxDocument();

        AceTreeTransaction *TX() const;
        DspxContentEntity *project() const;

    public:
        // Initializers, must call one of them before accessing the instance
        bool open(const QString &fileName) override;
        bool openRawData(const QString &suggestFileName, const QByteArray &data);
        void makeNew();
        bool recover(const QString &fileName);

        // Save functions
        bool save(const QString &fileName) override;
        bool saveRawData(QByteArray *data) const;

        // VST related
        bool isVSTMode() const;
        void setVSTMode(bool on);

        bool isOpened() const;

        ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const override;
        bool reload(ReloadFlag flag, ChangeType type) override;

        QString defaultPath() const override;
        QString suggestedFileName() const override;
        bool isModified() const override;

        void close() override;

    signals:
        void opened();

    protected:
        DspxDocument(DspxDocumentPrivate &d, QObject *parent = nullptr);
    };

}

#endif // DSPXDOCUMENT_H
