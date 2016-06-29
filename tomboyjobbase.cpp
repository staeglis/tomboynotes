/*
    Copyright (c) 2016 Stefan Stäglich

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "tomboyjobbase.h"

TomboyJobBase::TomboyJobBase(KIO::AccessManager *manager, QObject *parent)
    : KCompositeJob(parent)
{
    // Create the OAuth objects
    mO1 = new O1Tomboy(this);
    mManager = manager;
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
    case QNetworkReply::RemoteHostClosedError:
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::TimeoutError:
        return TomboyJobError::TemporaryError;
    default:
        return TomboyJobError::PermanentError;
    }
}
