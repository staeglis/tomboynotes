#include <QDesktopServices>
#include "tomboyserverauthenticatejob.h"

void TomboyServerAuthenticateJob::start()
{
    // Create the OAuth objects
    o1 = new O1Tomboy(this);
    manager = new KIO::AccessManager(this);
    requestor = new O1Requestor(manager, o1, this);

    // Connect the o2 authenfication signals
    connect(o1, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(o1, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(o1, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(o1, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(o1, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));

    o1->link();
}

void TomboyServerAuthenticateJob::onLinkingFailed()
{
    setError(1);
    setErrorText("Authorization has been failed");
    emitResult();
}

void TomboyServerAuthenticateJob::onLinkingSucceeded()
{
    setError(0);
    emitResult();
}

void TomboyServerAuthenticateJob::onLinkedChanged()
{

}

void TomboyServerAuthenticateJob::onOpenBrowser(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void TomboyServerAuthenticateJob::onCloseBrowser()
{

}
