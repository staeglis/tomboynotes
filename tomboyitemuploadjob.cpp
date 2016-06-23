#include "debug.h"
#include "tomboyitemuploadjob.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <krandom.h>

TomboyItemUploadJob::TomboyItemUploadJob(const Akonadi::Item &item, JobType jobType, QObject *parent)
    : TomboyJobBase(parent)
{
    mSourceItem = Akonadi::Item(item);
    mNoteContent = item.payload<KMime::Message::Ptr>();
    mJobType = jobType;

    mRemoteRevision = item.parentCollection().remoteRevision().toInt();

    // Create random remote id if adding new item
    if (jobType == JobType::addItem) {
        mSourceItem.remoteId() = KRandom::randomString(37);
    }
}

Akonadi::Item TomboyItemUploadJob::item() const
{
    return mSourceItem;
}

void TomboyItemUploadJob::start()
{
    QJsonObject jsonNote;

    // Convert note to JSON
    jsonNote["guid"] = mSourceItem.remoteId();
    if (mJobType == JobType::deleteItem) {
        jsonNote["command"] = "delete";
    }
    else {
        jsonNote["title"] = mNoteContent->headerByType("subject")->asUnicodeString();
        jsonNote["note-content"] = mNoteContent->mainBodyPart()->decodedText(false, false);
        jsonNote["note-content-version"] = "1";
        jsonNote["last-change-date"] = mSourceItem.modificationTime().toString(Qt::ISODate);
    }

    // Create the full JSON object
    QJsonArray noteChanges;
    noteChanges.append(jsonNote);
    QJsonObject postJson;
    postJson["latest-sync-revision"] = mRemoteRevision;
    postJson["note-changes"] = noteChanges;
    QJsonDocument postData;
    postData.setObject(postJson);

    // Network request
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL + "/notes" );
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
