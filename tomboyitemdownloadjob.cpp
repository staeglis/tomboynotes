#include "tomboyitemdownloadjob.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>>
#include <KMime/Message>


TomboyItemDownloadJob::TomboyItemDownloadJob(const Akonadi::Item &item, QObject *parent)
    : TomboyJobBase(parent)
{
    resultItem.setMimeType("application/x-vnd.kde.note");
    resultItem.setRemoteId(item.remoteId());
    resultItem.setParentCollection(item.parentCollection());
}

Akonadi::Item TomboyItemDownloadJob::item() const
{
    return resultItem;
}

void TomboyItemDownloadJob::start()
{
    // Get the speicific note
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL + "/note/" + resultItem.remoteId());
    mReply = requestor->get(request, requestParams);

    connect(mReply, &QNetworkReply::finished, this, &TomboyItemDownloadJob::onRequestFinished);
}

void TomboyItemDownloadJob::onRequestFinished()
{
    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    QJsonObject jsonNote = jo["note"].toArray().first().toObject();

    resultItem.setRemoteRevision(jsonNote["last-sync-revision"].toString());

    // Set timestamp
    QDateTime modificationTime = QDateTime::fromString(jsonNote["last-change-date"].toString().mid(0, 19), Qt::ISODate);
    modificationTime.setOffsetFromUtc(jsonNote["last-change-date"].toString().mid(27, 2).toInt() * 60 * 60);
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

    setError(0);
    emitResult();
}
