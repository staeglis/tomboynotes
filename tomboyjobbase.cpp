#include "tomboyjobbase.h"


TomboyJobBase::TomboyJobBase(QObject *parent)
{
    // Create the OAuth objects
    o1 = new O1Tomboy(this);
    manager = new KIO::AccessManager(this);
    requestor = new O1Requestor(manager, o1, this);
}

void TomboyJobBase::setServerURL(const QString &apiurl, const QString &contenturl)
{
    o1->setBaseURL(apiurl);
    mApiURL = apiurl + "/api/1.0";
    mContentURL = contenturl;
}

void TomboyJobBase::setAuthentication(const QString &token, const QString &secret)
{
    o1->restoreAuthData(token, secret);
}

TomboyJobError TomboyJobBase::checkReplyError()
{
    switch (mReply->error()) {
    case QNetworkReply::NoError :
        return TomboyJobError::NoError;
        break;
    case QNetworkReply::RemoteHostClosedError:
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::TimeoutError:
        return TomboyJobError::TemporaryError;
        break;
    default:
        return TomboyJobError::PermanentError;
    }
}
