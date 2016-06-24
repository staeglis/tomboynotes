#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "debug.h"
#include "tomboycollectionsdownloadjob.h"

TomboyCollectionsDownloadJob::TomboyCollectionsDownloadJob(const QString &collectionName, QObject *parent)
    : TomboyJobBase(parent)
{
    mCollectionName = collectionName;
}

Akonadi::Collection::List TomboyCollectionsDownloadJob::collections()
{
    return mResultCollections;
}

void TomboyCollectionsDownloadJob::start()
{
    // Get user informations
    QNetworkRequest request(mContentURL);
    mReply = mRequestor->get(request, QList<O0RequestParameter>());

    connect(mReply, &QNetworkReply::finished, this, &TomboyCollectionsDownloadJob::onRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: Start network request";
}

void TomboyCollectionsDownloadJob::onRequestFinished()
{
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: Network request finished";
    checkReplyError();
    if (error() != TomboyJobError::NoError)
    {
        setErrorText(mReply->errorString());
        emitResult();
        return;
    }

    // Parse received data as JSON
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);

    QJsonObject jo = document.object();
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: " << jo;
    QJsonValue collectionRevision = jo["latest-sync-revision"];
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: " << collectionRevision;

    Akonadi::Collection c;
    c.setParentCollection( Akonadi::Collection::root());
    c.setRemoteId( mContentURL );
    c.setName( mCollectionName );
    c.setRemoteRevision(QString::number(collectionRevision.toInt()));
    qCDebug(log_tomboynotesresource) << "TomboyCollectionsDownloadJob: Sync revision " << collectionRevision.toString();

    QStringList mimeTypes;
    mimeTypes << QLatin1String("text/x-vnd.akonadi.note");
    c.setContentMimeTypes( mimeTypes );

    mResultCollections << c;

    emitResult();
}
