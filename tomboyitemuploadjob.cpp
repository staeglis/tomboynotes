#include "debug.h"
#include "tomboyitemuploadjob.h"

#include <QJsonDocument>

TomboyItemUploadJob::TomboyItemUploadJob(const Akonadi::Item &item, QObject *parent)
    : TomboyJobBase(parent)
{
    mSourceItem = Akonadi::Item(item);
    mNoteContent = item.payload<KMime::Message::Ptr>();
}

Akonadi::Item TomboyItemUploadJob::item() const
{
    return mSourceItem;
}

void TomboyItemUploadJob::start()
{
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL + "/notes" );

    QJsonDocument postData;
    mReply = requestor->put(request, requestParams, postData.toJson());
    connect(mReply, &QNetworkReply::finished, this, &TomboyItemUploadJob::onRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyItemUploadJob: Start network request";
}

void TomboyItemUploadJob::onRequestFinished()
{
    if (mReply->error() != QNetworkReply::NoError)
    {
        setErrorText(mReply->errorString());
        setError(mReply->error());
        return;
    }
    qCDebug(log_tomboynotesresource) << "TomboyItemUploadJob: Network request finished. No error occured";

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);
}
