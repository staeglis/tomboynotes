#include "tomboyitemdownloadjob.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>>
#include <KMime/Message>


TomboyItemDownloadJob::TomboyItemDownloadJob(const Akonadi::Item &item, QObject *parent)
    : TomboyJobBase(parent)
{
    resultItem.setMimeType("application/x-vnd.kde.note");
    //resultItem.setId(item.id());
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
    QNetworkReply *reply = requestor->get(request, requestParams);

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    QJsonObject jsonNote = jo["note"].toArray().first().toObject();

    resultItem.setRemoteRevision(jsonNote["last-sync-revision"].toString());

    // Set timestamp
    QDateTime modifyTime = QDateTime::fromString(jsonNote["last-change-date"].toString().mid(0, 19), Qt::ISODate);
    modifyTime.setOffsetFromUtc(jsonNote["last-change-date"].toString().mid(27, 2).toInt() * 60 * 60);
    resultItem.setModificationTime(modifyTime);

    KMime::Message::Ptr akonadiNote = KMime::Message::Ptr(new KMime::Message);
    akonadiNote->subject(true)->fromUnicodeString( jsonNote["title"].toString(), "utf-8" );

    akonadiNote->contentType()->setMimeType("text/plain");
    akonadiNote->contentType()->setCharset("utf-8");
    akonadiNote->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
    akonadiNote->mainBodyPart()->fromUnicodeString(jsonNote["note-content"].toString().toUtf8());

    akonadiNote->assemble();
    resultItem.setPayload<KMime::Message::Ptr>(akonadiNote);

    setError(0);
    emitResult();
}
