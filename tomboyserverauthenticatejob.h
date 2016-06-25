#ifndef TOMBOYSERVERAUTHENTICATEJOB_H
#define TOMBOYSERVERAUTHENTICATEJOB_H

#include <QString>
#include "tomboyjobbase.h"


class TomboyServerAuthenticateJob : public TomboyJobBase
{
    Q_OBJECT
public:
    TomboyServerAuthenticateJob(QObject *parent = Q_NULLPTR);

    QString getRequestToken();
    QString getRequestTokenSecret();
    QString getContentUrl();
    QString getUserURL();

    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onLinkingFailed();
    void onLinkingSucceeded();
    void onLinkedChanged();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();

    void onApiRequestFinished();
    void onUserRequestFinished();

private:
    QString mUserURL;

};

#endif // TOMBOYSERVERAUTHENTICATEJOB_H
