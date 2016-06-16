#include "tomboyjobbase.h"


TomboyJobBase::TomboyJobBase(QObject *parent)
{
    // Create the OAuth objects
    o1 = new O1Tomboy(this);
    manager = new KIO::AccessManager(this);
    requestor = new O1Requestor(manager, o1, this);
}

void TomboyJobBase::setServerURL(const QString &url, const QString &username)
{
    o1->setBaseURL(url);
    userURL = url + "/api/1.0/" + username;
}

void TomboyJobBase::setAuthentication(const QString &token, const QString &secret)
{
    o1->restoreAuthData(token, secret);
}
