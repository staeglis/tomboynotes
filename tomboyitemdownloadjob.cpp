#include "debug.h"
#include "tomboyitemdownloadjob.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <KMime/Message>

TomboyItemDownloadJob::TomboyItemDownloadJob(const Akonadi::Item &item, QObject *parent)
    : TomboyJobBase(parent)
{
    resultItem = Akonadi::Item(item);
}

Akonadi::Item TomboyItemDownloadJob::item() const
{
    return resultItem;
}

void TomboyItemDownloadJob::start()
{
    // Get the speicific note
    mContentURL.chop(1);
    QNetworkRequest request(mContentURL + "/" + resultItem.remoteId());
    mReply = requestor->get(request, QList<O0RequestParameter>());

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

    resultItem.setRemoteRevision(QString::number(jsonNote["last-sync-revision"].toInt()));
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Sync revision " << resultItem.remoteRevision();


    // Set timestamp
    QString timeStampJson = jsonNote["last-change-date"].toString();
    QDateTime modificationTime = QDateTime::fromString(timeStampJson, Qt::ISODate);
    resultItem.setModificationTime(modificationTime);

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
    resultItem.setPayload<KMime::Message::Ptr>(akonadiNote);

    emitResult();
}
