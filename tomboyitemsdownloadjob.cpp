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
    return resultItems;
}

void TomboyItemsDownloadJob::start()
{
    // Get all notes
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL + "/notes");
    mReply = requestor->get(request, requestParams);

    connect(mReply, &QNetworkReply::finished, this, &TomboyItemsDownloadJob::onRequestFinished);
}

void TomboyItemsDownloadJob::onRequestFinished()
{
    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    QJsonArray notes = jo["notes"].toArray();

    foreach (auto note, notes) {
        Akonadi::Item item( QLatin1String( "application/x-vnd.kde.note" ) );
        item.setRemoteId(note.toObject()["guid"].toString());
        resultItems << item;
    }

    emitResult();
}
