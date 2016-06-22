#include "debug.h"
#include "configdialog.h"
#include "tomboyserverauthenticatejob.h"
#include "tomboycollectionsdownloadjob.h"
#include "tomboyitemdownloadjob.h"
#include "tomboyitemsdownloadjob.h"
#include "tomboynotesresource.h"
#include "settings.h"
#include "settingsadaptor.h"

#include <QtDBus/QDBusConnection>
#include <kconfigdialog.h>
#include <kwindowsystem.h>

using namespace Akonadi;

TomboyNotesResource::TomboyNotesResource(const QString &id)
    : ResourceBase(id)
{
    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Settings"),
                                                 Settings::self(),
                                                 QDBusConnection::ExportAdaptors);

    // TODO: you can put any resource specific initialization code here.

    qCDebug(log_tomboynotesresource) << "Resource started";
}

TomboyNotesResource::~TomboyNotesResource()
{
}

void TomboyNotesResource::retrieveCollections()
{
    qCDebug(log_tomboynotesresource) << "Retriving collections started";

    auto job = new TomboyCollectionsDownloadJob(this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::username());
    // connect to its result() signal
    connect(job, &KJob::result, this, &TomboyNotesResource::onCollectionsRetrieved);
    job->start();
    qCDebug(log_tomboynotesresource) << "Retriving collections job started";
}


void TomboyNotesResource::retrieveItems(const Akonadi::Collection &collection)
{
    // create the job
    auto job = new TomboyItemsDownloadJob(collection.id(), this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::username());
    // connect to its result() signal
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemsRetrieved);
    job->start();
    qCDebug(log_tomboynotesresource) << "Retriving items job started";
}

bool TomboyNotesResource::retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    Q_UNUSED( parts );
    // TODO: this method is called when Akonadi wants more data for a given item.
    // You can only provide the parts that have been requested but you are allowed
    // to provide all in one go
    auto job = new TomboyItemDownloadJob(item, this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::username());
    // connect to its result() signal
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemRetrieved);
    job->start();
    qCDebug(log_tomboynotesresource) << "Retriving item data job started";

    return true;
}

void TomboyNotesResource::onAuthorizationFinished(KJob *kjob)
{
    // Saves the received client authentication data in the settings
    qCDebug(log_tomboynotesresource) << "Authorization job finished";
    auto job = qobject_cast<TomboyServerAuthenticateJob*>(kjob);
    if (job->errorString().isEmpty()) {
        Settings::setRequestToken(job->getRequestToken());
        Settings::setRequestTokenSecret(job->getRequestTokenSecret());
        Settings::self()->writeConfig();
        synchronizeCollectionTree();
    }
    else {
        cancelTask("Authorization has been failed!");
    }
}

void TomboyNotesResource::onCollectionsRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyCollectionsDownloadJob*>(kjob);
    collectionsRetrieved(job->collections());
}

void TomboyNotesResource::onItemRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyItemDownloadJob*>(kjob);
    itemRetrieved(job->item());
    qCDebug(log_tomboynotesresource) << "Retriving item data job with remoteId " << job->item().remoteId() << " finished";
}

void TomboyNotesResource::onItemsRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyItemsDownloadJob*>(kjob);
    itemsRetrieved(job->items());
    qCDebug(log_tomboynotesresource) << "Retriving items job finished";
}

void TomboyNotesResource::aboutToQuit()
{
    // TODO: any cleanup you need to do while there is still an active
    // event loop. The resource will terminate after this method returns
}

void TomboyNotesResource::configure(WId windowId)
{
    qCDebug(log_tomboynotesresource) << "Resource configuration started";

    ConfigDialog dialog(Settings::self());

    if (windowId) {
            KWindowSystem::setMainWindow(&dialog, windowId);
    }

    if (dialog.exec()) {
        dialog.saveSettings();
    }

    if (Settings::requestToken().isEmpty() || Settings::requestToken().isEmpty())
    {
        auto job = new TomboyServerAuthenticateJob(this);
        job->setServerURL(Settings::serverURL(), Settings::username());
        connect(job, &KJob::result, this, &TomboyNotesResource::onAuthorizationFinished);
        job->start();
        qCDebug(log_tomboynotesresource) << "Authorization job started";
    }
    else {
        synchronize();
    }
}

void TomboyNotesResource::itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    if (Settings::readOnly()) {
        cancelTask("Resource is read-only");
    }
}

void TomboyNotesResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    if (Settings::readOnly()) {
            cancelTask("Resource is read-only");
    }
}

void TomboyNotesResource::itemRemoved(const Akonadi::Item &item)
{
    if (Settings::readOnly()) {
            cancelTask("Resource is read-only");
    }
}

AKONADI_RESOURCE_MAIN(TomboyNotesResource)

#include "moc_tomboynotesresource.cpp"
