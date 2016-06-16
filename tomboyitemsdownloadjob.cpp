#include "tomboyitemsdownloadjob.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>>


TomboyItemsDownloadJob::TomboyItemsDownloadJob(const Akonadi::Collection::Id &id, QObject *parent)
    : TomboyJobBase(parent)
{
    collectionId = id;
}

Akonadi::Item::List TomboyItemsDownloadJob::items() const
{
    return pitems;
}

void TomboyItemsDownloadJob::start()
{
    // Get all notes
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL + "/notes");
    QNetworkReply *reply = requestor->get(request, requestParams);

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    QJsonArray notes = jo["notes"].toArray();

    foreach (auto note, notes) {
        Akonadi::Item item( QLatin1String( "application/x-vnd.kde.notes" ) );
        item.setRemoteId(note.toObject()["guid"].toString());
        pitems << item;
    }

    setError(0);
    emitResult();
}
