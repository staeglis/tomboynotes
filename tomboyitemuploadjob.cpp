#include "debug.h"
#include "tomboyitemuploadjob.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

TomboyItemUploadJob::TomboyItemUploadJob(const Akonadi::Item &item, JobType jobType, QObject *parent)
    : TomboyJobBase(parent)
{
    mSourceItem = Akonadi::Item(item);
    mNoteContent = item.payload<KMime::Message::Ptr>();
    mJobType = jobType;
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

    QJsonObject jo = document.object();
    QJsonArray notes = jo["notes"].toArray();

    // Check if server status is as expected
    bool found = false;
    foreach (auto note, notes) {
        if (note.toObject()["guid"].toString() == mSourceItem.remoteId()) {
            found = true;
            break;
        }
    }
    if (mJobType == JobType::deleteItem && found) {
        setError(0);
        setErrorText("Sync error. Server status not as expected!");
        return;
    }
    if (mJobType != JobType::deleteItem && !found) {
        setError(0);
        setErrorText("Sync error. Server status not as expected!");
        return;
    }
}
