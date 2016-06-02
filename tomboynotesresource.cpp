#include "tomboynotesresource.h"

#include "settings.h"
#include "settingsadaptor.h"
#include "debug.h"

#include <QDesktopServices>
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
    o1 = new O1(this);
    manager = new QNetworkAccessManager(this);
    requestor = new O1Requestor(manager, o1, this);

    connect(o1, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(o1, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(o1, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(o1, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(o1, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));

    qCDebug(log_tomboynotesresource) << "Resource started";
}

TomboyNotesResource::~TomboyNotesResource()
{
}

void TomboyNotesResource::retrieveCollections()
{
    // TODO: this method is called when Akonadi wants to have all the
    // collections your resource provides.
    // Be sure to set the remote ID and the content MIME types
}

void TomboyNotesResource::retrieveItems(const Akonadi::Collection &collection)
{
    // TODO: this method is called when Akonadi wants to know about all the
    // items in the given collection. You can but don't have to provide all the
    // data for each item, remote ID and MIME type are enough at this stage.
    // Depending on how your resource accesses the data, there are several
    // different ways to tell Akonadi when you are done.
}

bool TomboyNotesResource::retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    // TODO: this method is called when Akonadi wants more data for a given item.
    // You can only provide the parts that have been requested but you are allowed
    // to provide all in one go

    return true;
}

void TomboyNotesResource::onLinkedChanged()
{

}

void TomboyNotesResource::onLinkingFailed()
{

}

void TomboyNotesResource::onLinkingSucceeded()
{

}

void TomboyNotesResource::onOpenBrowser(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void TomboyNotesResource::onCloseBrowser()
{

}

void TomboyNotesResource::aboutToQuit()
{
    // TODO: any cleanup you need to do while there is still an active
    // event loop. The resource will terminate after this method returns
}

void TomboyNotesResource::configure(WId windowId)
{
    // TODO: this method is usually called when a new resource is being
    // added to the Akonadi setup. You can do any kind of user interaction here,
    // e.g. showing dialogs.
    // The given window ID is usually useful to get the correct
    // "on top of parent" behavior if the running window manager applies any kind
    // of focus stealing prevention technique
    //
    // If the configuration dialog has been accepted by the user by clicking Ok,
    // the signal configurationDialogAccepted() has to be emitted, otherwise, if
    // the user canceled the dialog, configurationDialogRejected() has to be emitted.
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
