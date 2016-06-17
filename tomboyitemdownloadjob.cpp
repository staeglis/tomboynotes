#include "tomboyitemdownloadjob.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>>


TomboyItemDownloadJob::TomboyItemDownloadJob(const Akonadi::Item &item, QObject *parent)
    : TomboyJobBase(parent)
{
    resultItem.setMimeType("application/x-vnd.kde.notes");
    resultItem.setId(item.id());
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
    QJsonObject note = jo["note"].toArray().first().toObject();

    resultItem.setRemoteRevision(note["last-sync-revision"].toString());

    // Set timestamp
    QDateTime modifyTime = QDateTime::fromString(note["last-change-date"].toString().mid(0, 19), Qt::ISODate);
    modifyTime.setOffsetFromUtc(note["last-change-date"].toString().mid(27, 2).toInt() * 60 * 60);
    resultItem.setModificationTime(modifyTime);

    setError(0);
    emitResult();
}
