#include "debug.h"
#include "tomboycollectionsdownloadjob.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>>
#include <QJsonValue>

TomboyCollectionsDownloadJob::TomboyCollectionsDownloadJob(QObject *parent)
    : TomboyJobBase(parent)
{
}

Akonadi::Collection::List TomboyCollectionsDownloadJob::collections()
{
    return resultCollections;
}

void TomboyCollectionsDownloadJob::start()
{
    // Get user informations
    QList<O0RequestParameter> requestParams = QList<O0RequestParameter>();
    QNetworkRequest request(userURL);
    mReply = requestor->get(request, requestParams);

    connect(mReply, &QNetworkReply::finished, this, &TomboyCollectionsDownloadJob::onRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: Start network request";
}

void TomboyCollectionsDownloadJob::onRequestFinished()
{
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: Network request finished";
    if (mReply->error() != QNetworkReply::NoError)
    {
        // TODO
    }

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: " << jo;
    QJsonValue collectionRevision = jo["latest-sync-revision"];
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: " << collectionRevision;

    Akonadi::Collection c;
    c.setParentCollection( Akonadi::Collection::root());
    c.setRemoteId( userURL );
    c.setName( "Tomboy Notes");
    c.setRemoteRevision(QString::number(collectionRevision.toInt()));
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: Sync revision " << collectionRevision.toString();

    QStringList mimeTypes;
    mimeTypes << QLatin1String("application/x-vnd.kde.note");
    c.setContentMimeTypes( mimeTypes );

    resultCollections << c;

    emitResult();
}
