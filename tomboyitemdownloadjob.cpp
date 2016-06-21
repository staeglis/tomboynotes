#include "debug.h"
#include "tomboyitemdownloadjob.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>>
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
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL + "/note" + resultItem.remoteId());
    mReply = requestor->get(request, requestParams);

    connect(mReply, &QNetworkReply::finished, this, &TomboyItemDownloadJob::onRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Start network request";
}

void TomboyItemDownloadJob::onRequestFinished()
{
    if (mReply->error() != QNetworkReply::NoError)
    {
        setErrorText(mReply->errorString());
    }
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Network request finished. No error occured";

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jsonNote = document.object();

    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: JSON note: " << jsonNote;

    resultItem.setRemoteRevision(QString::number(jsonNote["last-sync-revision"].toInt()));
    qCDebug(log_tomboynotesresource) << "TomboyItemDownloadJob: Sync revision " << QString::number(jsonNote["last-sync-revision"].toInt());


    // Set timestamp
    QString timeStampJson = jsonNote["last-change-date"].toString();
    QDateTime modificationTime = QDateTime::fromString(timeStampJson.mid(0, 19), Qt::ISODate);
    modificationTime.setOffsetFromUtc(timeStampJson.mid(27, 2).toInt() * 60 * 60);
    resultItem.setModificationTime(modificationTime);

    // Set note title
    KMime::Message::Ptr akonadiNote = KMime::Message::Ptr(new KMime::Message);
    akonadiNote->subject(true)->fromUnicodeString( jsonNote["title"].toString(), "utf-8" );

    // Set note content
    akonadiNote->contentType()->setMimeType("text/plain");
    akonadiNote->contentType()->setCharset("utf-8");
    akonadiNote->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
    akonadiNote->mainBodyPart()->fromUnicodeString(jsonNote["note-content"].toString().toUtf8());

    // Add title and content to Akonadi::Item
    akonadiNote->assemble();
    resultItem.setPayload<KMime::Message::Ptr>(akonadiNote);

    emitResult();
}
