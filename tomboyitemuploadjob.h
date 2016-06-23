#ifndef TOMBOYITEMUPLOADJOB_H
#define TOMBOYITEMUPLOADJOB_H

#include <QJsonObject>
#include <AkonadiAgentBase/ResourceBase>
#include <KMime/Message>
#include "tomboyjobbase.h"


class TomboyItemUploadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    TomboyItemUploadJob(const Akonadi::Item &item, QObject *parent = 0);

    // Returns mSourceItem for post-processing purposes
    Akonadi::Item item() const;

    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // This will be called once the request is finished. You process the response
    // and emitResult(), to which your code in the resource will react.
    void onRequestFinished();

private:
    Akonadi::Item mSourceItem;
    KMime::Message::Ptr mNoteContent;
};

#endif // TOMBOYITEMUPLOADJOB_H
