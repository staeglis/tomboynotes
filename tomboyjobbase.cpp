#include "tomboyjobbase.h"


TomboyJobBase::TomboyJobBase(QObject *parent)
    : KCompositeJob(parent)
{
    // Create the OAuth objects
    mO1 = new O1Tomboy(this);
    mManager = new KIO::AccessManager(this);
    mRequestor = new O1Requestor(mManager, mO1, this);
}

void TomboyJobBase::setServerURL(const QString &apiurl, const QString &contenturl)
{
    mO1->setBaseURL(apiurl);
    mApiURL = apiurl + "/api/1.0";
    mContentURL = contenturl;
}

void TomboyJobBase::setAuthentication(const QString &token, const QString &secret)
{
    mO1->restoreAuthData(token, secret);
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
