#include "debug.h"
#include "configdialog.h"
#include "tomboyserverauthenticatejob.h"
#include "tomboycollectionsdownloadjob.h"
#include "tomboyitemdownloadjob.h"
#include "tomboyitemsdownloadjob.h"
#include "tomboyitemuploadjob.h"
#include "tomboynotesresource.h"
#include "settings.h"
#include "settingsadaptor.h"

#include <QtDBus/QDBusConnection>
#include <kwindowsystem.h>

using namespace Akonadi;

TomboyNotesResource::TomboyNotesResource(const QString &id)
    : ResourceBase(id)
{
    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Settings"),
                                                 Settings::self(),
                                                 QDBusConnection::ExportAdaptors);

    // Status message stuff
    mStatusMessageTimer = new QTimer(this);
    mStatusMessageTimer->setSingleShot(true);
    connect(mStatusMessageTimer, &QTimer::timeout, this, &TomboyNotesResource::clearStatusMessage);
    connect(this, &AgentBase::error, this, &TomboyNotesResource::showError);

    qCDebug(log_tomboynotesresource) << "Resource started";
}

TomboyNotesResource::~TomboyNotesResource()
{
    delete mStatusMessageTimer;
}

void TomboyNotesResource::retrieveCollections()
{
    qCDebug(log_tomboynotesresource) << "Retriving collections started";

    auto job = new TomboyCollectionsDownloadJob(this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::contentURL());
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
    job->setServerURL(Settings::serverURL(), Settings::contentURL());
    // connect to its result() signal
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemsRetrieved);
    job->start();
    qCDebug(log_tomboynotesresource) << "Retriving items job started";
}

bool TomboyNotesResource::retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    Q_UNUSED( parts );

    // this method is called when Akonadi wants more data for a given item.
    auto job = new TomboyItemDownloadJob(item, this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::contentURL());
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
    if (job->error() == TomboyJobError::NoError) {
        Settings::setRequestToken(job->getRequestToken());
        Settings::setRequestTokenSecret(job->getRequestTokenSecret());
        Settings::setContentURL(job->getContentUrl());
        Settings::self()->save();
        synchronizeCollectionTree();
    }
    else {
        showError(job->errorText());
    }
}

void TomboyNotesResource::onCollectionsRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyCollectionsDownloadJob*>(kjob);
    if (job->error() != TomboyJobError::NoError) {
        cancelTask();
        showError(job->errorText());
        return;
    }

    collectionsRetrieved(job->collections());
}

void TomboyNotesResource::onItemChangeCommitted(KJob *kjob)
{
    auto job = qobject_cast<TomboyItemUploadJob*>(kjob);
    switch (job->error()) {
    case TomboyJobError::PermanentError:
        cancelTask();
        showError(job->errorText());
        return;
    case TomboyJobError::TemporaryError:
        retryAfterFailure(job->errorString());
        return;
    case TomboyJobError::NoError:
        changeCommitted(job->item());
        synchronize();
        return;
    }
}

void TomboyNotesResource::onItemRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyItemDownloadJob*>(kjob);

    if (job->error() != TomboyJobError::NoError) {
        cancelTask();
        showError(job->errorText());
        return;
    }

    itemRetrieved(job->item());
    qCDebug(log_tomboynotesresource) << "Retriving item data job with remoteId " << job->item().remoteId() << " finished";
}

void TomboyNotesResource::onItemsRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyItemsDownloadJob*>(kjob);
    if (job->error() != TomboyJobError::NoError) {
        cancelTask();
        showError(job->errorText());
        return;
    }

    itemsRetrieved(job->items());
    qCDebug(log_tomboynotesresource) << "Retriving items job finished";
}

void TomboyNotesResource::clearStatusMessage()
{
    Q_EMIT status(Akonadi::AgentBase::Idle, QString());
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

    if (configurationNotValid())
    {
        auto job = new TomboyServerAuthenticateJob(this);
        job->setServerURL(Settings::serverURL(), Settings::contentURL());
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
    if (Settings::readOnly() || configurationNotValid()) {
        cancelTask("Resource is read-only");
        return;
    }

    auto job = new TomboyItemUploadJob(item, JobType::addItem, this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::contentURL());
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemChangeCommitted);
    job->start();
}

void TomboyNotesResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    Q_UNUSED( parts );
    if (Settings::readOnly() || configurationNotValid() || true) {
            cancelTask("Resource is read-only");
            return;
    }

    auto job = new TomboyItemUploadJob(item, JobType::modifyItem, this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::contentURL());
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemChangeCommitted);
    job->start();
}

void TomboyNotesResource::itemRemoved(const Akonadi::Item &item)
{
    if (Settings::readOnly() || configurationNotValid()) {
            cancelTask("Resource is read-only");
            return;
    }

    auto job = new TomboyItemUploadJob(item, JobType::deleteItem, this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::contentURL());
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemChangeCommitted);
    job->start();
}

bool TomboyNotesResource::configurationNotValid()
{
    return Settings::requestToken().isEmpty() || Settings::requestToken().isEmpty() || Settings::contentURL().isEmpty();
}

void TomboyNotesResource::retryAfterFailure(const QString &errorMessage)
{
    Q_EMIT status(Broken, errorMessage);
    deferTask();
    setTemporaryOffline(Settings::self()->refreshInterval() <= 0 ? 300 : Settings::self()->refreshInterval() * 60);
}

void TomboyNotesResource::showError(const QString errorText)
{
    Q_EMIT status(Akonadi::AgentBase::Idle, errorText);
    mStatusMessageTimer->start(1000 * 10);
}

AKONADI_RESOURCE_MAIN(TomboyNotesResource)

#include "moc_tomboynotesresource.cpp"
