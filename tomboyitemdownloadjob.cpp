#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <KMime/Message>
#include "debug.h"
#include "tomboyitemdownloadjob.h"

TomboyItemDownloadJob::TomboyItemDownloadJob(const Akonadi::Item &item, QObject *parent)
    : TomboyJobBase(parent)
{
    mResultItem = Akonadi::Item(item);
}

Akonadi::Item TomboyItemDownloadJob::item() const
{
    return mResultItem;
}

void TomboyItemDownloadJob::start()
{
    // Get the speicific note
    mContentURL.chop(1);
    QNetworkRequest request(mContentURL + "/" + mResultItem.remoteId());
    mReply = mRequestor->get(request, QList<O0RequestParameter>());

    connect(mReply, &QNetworkReply::finished, this, &TomboyItemDownloadJob::onRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Start network request";
}

void TomboyItemDownloadJob::onRequestFinished()
{
    checkReplyError();
    if (error() != TomboyJobError::NoError)
    {
        setErrorText(mReply->errorString());
        emitResult();
        return;
    }
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Network request finished. No error occured";

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jsonNote = document.object();

    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: JSON note: " << jsonNote;

    mResultItem.setRemoteRevision(QString::number(jsonNote["last-sync-revision"].toInt()));
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Sync revision " << mResultItem.remoteRevision();


    // Set timestamp
    QString timeStampJson = jsonNote["last-change-date"].toString();
    QDateTime modificationTime = QDateTime::fromString(timeStampJson, "yyyy-MM-ddThh-mm-ss.0000zzz-HH:00");
    mResultItem.setModificationTime(modificationTime);

    // Set note title
    KMime::Message::Ptr akonadiNote = KMime::Message::Ptr(new KMime::Message);
    akonadiNote->subject(true)->fromUnicodeString( jsonNote["title"].toString().toUtf8(), "utf-8" );

    // Set note content
    akonadiNote->contentType()->setMimeType("text/html");
    akonadiNote->contentType()->setCharset("utf-8");
    akonadiNote->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
    akonadiNote->mainBodyPart()->fromUnicodeString(jsonNote["note-content"].toString().toUtf8());

    // Add title and content to Akonadi::Item
    akonadiNote->assemble();
    mResultItem.setPayload<KMime::Message::Ptr>(akonadiNote);

    emitResult();
}
