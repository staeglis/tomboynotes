#include "tomboyserverauthenticatejob.h"
#include "tomboynotesresource.h"

#include "tomboyitemdownloadjob.h"
#include "tomboyitemsdownloadjob.h"
#include "settings.h"
#include "settingsadaptor.h"
#include "debug.h"

#include <QtDBus/QDBusConnection>

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
    Collection c;
    c.setParentCollection( Collection::root() );
    c.setRemoteId( Settings::serverURL() );
    c.setName( name() );

    QStringList mimeTypes;
    mimeTypes << QLatin1String("application/x-vnd.kde.note");
    c.setContentMimeTypes( mimeTypes );

    Collection::List list;
    list << c;
    collectionsRetrieved( list );
}

void TomboyNotesResource::retrieveItems(const Akonadi::Collection &collection)
{
    // create the job
    auto job = new TomboyItemsDownloadJob(collection.id(), this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::username());
    // connect to its result() signal
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemsRetrieved);
    qCDebug(log_tomboynotesresource) << "Retriving items job started";
}

bool TomboyNotesResource::retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    // TODO: this method is called when Akonadi wants more data for a given item.
    // You can only provide the parts that have been requested but you are allowed
    // to provide all in one go
    auto job = new TomboyItemDownloadJob(item, this);
    job->setAuthentication(Settings::requestToken(), Settings::requestTokenSecret());
    job->setServerURL(Settings::serverURL(), Settings::username());
    // connect to its result() signal
    connect(job, &KJob::result, this, &TomboyNotesResource::onItemRetrieved);
    qCDebug(log_tomboynotesresource) << "Retriving item data job started";

    return true;
}

void TomboyNotesResource::onAuthorizationFinished(KJob *kjob)
{
    // Saves the received client authentication data in the settings
    qCDebug(log_tomboynotesresource) << "Authorization job finished";
    auto job = qobject_cast<TomboyServerAuthenticateJob*>(kjob);
    Settings::setRequestToken(job->getRequestToken());
    Settings::setRequestTokenSecret(job->getRequestTokenSecret());
}

void TomboyNotesResource::onItemRetrieved(KJob *kjob)
{
    auto job = qobject_cast<TomboyItemDownloadJob*>(kjob);
    itemRetrieved(job->item());
    qCDebug(log_tomboynotesresource) << "Retriving item data job finished";
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
    if (Settings::requestToken().isEmpty() || Settings::requestToken().isEmpty())
    {
        auto job = new TomboyServerAuthenticateJob(this);
        job->setServerURL(Settings::serverURL(), Settings::username());
        connect(job, &KJob::result, this, &TomboyNotesResource::onAuthorizationFinished);
        job->start();
        qCDebug(log_tomboynotesresource) << "Authorization job started";
    }
}

void TomboyNotesResource::itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    // TODO: this method is called when somebody else, e.g. a client application,
    // has created an item in a collection managed by your resource.

    // NOTE: There is an equivalent method for collections, but it isn't part
    // of this template code to keep it simple
}

void TomboyNotesResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    // TODO: this method is called when somebody else, e.g. a client application,
    // has changed an item managed by your resource.

    // NOTE: There is an equivalent method for collections, but it isn't part
    // of this template code to keep it simple
}

void TomboyNotesResource::itemRemoved(const Akonadi::Item &item)
{
    // TODO: this method is called when somebody else, e.g. a client application,
    // has deleted an item managed by your resource.

    // NOTE: There is an equivalent method for collections, but it isn't part
    // of this template code to keep it simple
}

AKONADI_RESOURCE_MAIN(TomboyNotesResource)

#include "moc_tomboynotesresource.cpp"
