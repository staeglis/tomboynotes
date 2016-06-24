#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "debug.h"
#include "tomboyitemsdownloadjob.h"


TomboyItemsDownloadJob::TomboyItemsDownloadJob(const Akonadi::Collection::Id &id, QObject *parent)
    : TomboyJobBase(parent)
{
    mCollectionId = id;
}

Akonadi::Item::List TomboyItemsDownloadJob::items() const
{
    return mResultItems;
}

void TomboyItemsDownloadJob::start()
{
    // Get all notes
    QNetworkRequest request(mContentURL);
    mReply = mRequestor->get(request, QList<O0RequestParameter>());

    connect(mReply, &QNetworkReply::finished, this, &TomboyItemsDownloadJob::onRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyItemsDownloadJob: Start network request";
}

void TomboyItemsDownloadJob::onRequestFinished()
{
    qCDebug(log_tomboynotesresource) << "TomboyItemsDownloadJob: Network request finished";
    checkReplyError();
    if (error() != TomboyJobError::NoError)
    {
        setErrorText(mReply->errorString());
        emitResult();
        return;
    }

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    QJsonArray notes = jo["notes"].toArray();

    foreach (auto note, notes) {
        Akonadi::Item item( "text/x-vnd.akonadi.note" );
        item.setRemoteId(note.toObject()["guid"].toString());
        mResultItems << item;
        qCDebug(log_tomboynotesresource) << "TomboyItemsDownloadJob: Retriving note with id" << item.remoteId();
    }

    setError(TomboyJobError::NoError);
    emitResult();
}
