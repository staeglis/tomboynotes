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

#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <klocalizedstring.h>
#include "debug.h"
#include "tomboyserverauthenticatejob.h"

TomboyServerAuthenticateJob::TomboyServerAuthenticateJob(KIO::AccessManager *manager, QObject *parent)
    : TomboyJobBase(manager, parent)
{
    // Connect the o2 authenfication signals
    connect(mO1, &O1::linkingFailed, this, &TomboyServerAuthenticateJob::onLinkingFailed);
    connect(mO1, &O1::linkingSucceeded, this, &TomboyServerAuthenticateJob::onLinkingSucceeded);
    connect(mO1, &O1::openBrowser, this, &TomboyServerAuthenticateJob::onOpenBrowser);
}

void TomboyServerAuthenticateJob::start()
{
    mO1->link();
}

QString TomboyServerAuthenticateJob::getRequestToken() const
{
    return mO1->getRequestToken();
}

QString TomboyServerAuthenticateJob::getRequestTokenSecret() const
{
    return mO1->getRequestTokenSecret();
}

QString TomboyServerAuthenticateJob::getContentUrl() const
{
    return mContentURL;
}

QString TomboyServerAuthenticateJob::getUserURL() const
{
    return mUserURL;
}

void TomboyServerAuthenticateJob::onLinkingFailed()
{
    setError(TomboyJobError::PermanentError);
    setErrorText(i18n("Authorization has been failed!"));
    emitResult();
}

void TomboyServerAuthenticateJob::onLinkingSucceeded()
{
    QNetworkRequest request(mApiURL);
    mReply = mRequestor->get(request, QList<O0RequestParameter>());

    connect(mReply, &QNetworkReply::finished, this, &TomboyServerAuthenticateJob::onApiRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyServerAuthenticateJob: Start network request";
}

void TomboyServerAuthenticateJob::onOpenBrowser(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void TomboyServerAuthenticateJob::onApiRequestFinished()
{
    checkReplyError();
    if (error() != TomboyJobError::NoError) {
        setErrorText(mReply->errorString());
        emitResult();
        return;
    }

    // Parse received data as JSON and get user-href
    const QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);
    const QJsonObject jo = document.object();
    mUserURL = jo[QLatin1String("user-ref")].toObject()[QLatin1String("api-ref")].toString();

    QNetworkRequest request(mUserURL);
    mReply = mRequestor->get(request, QList<O0RequestParameter>());

    connect(mReply, &QNetworkReply::finished, this, &TomboyServerAuthenticateJob::onUserRequestFinished);
    qCDebug(log_tomboynotesresource) << "TomboyServerAuthenticateJob: Start network request";
}

void TomboyServerAuthenticateJob::onUserRequestFinished()
{
    checkReplyError();
    if (error() != TomboyJobError::NoError)
    {
        setErrorText(mReply->errorString());
        emitResult();
        return;
    }

    // Parse received data as JSON and get contentURL
    QJsonDocument document = QJsonDocument::fromJson(mReply->readAll(), Q_NULLPTR);
    QJsonObject jo = document.object();
    mContentURL = jo[QLatin1String("notes-ref")].toObject()[QLatin1String("api-ref")].toString();

    setError(TomboyJobError::NoError);
    emitResult();
}
