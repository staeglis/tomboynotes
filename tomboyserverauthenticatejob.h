#ifndef TOMBOYSERVERAUTHENTICATEJOB_H
#define TOMBOYSERVERAUTHENTICATEJOB_H

#include <QString>
#include "tomboyjobbase.h"


class TomboyServerAuthenticateJob : public TomboyJobBase
{
    Q_OBJECT
public:
    TomboyServerAuthenticateJob(QObject *parent = Q_NULLPTR);

    void start() Q_DECL_OVERRIDE;
    QString getRequestToken();
    QString getRequestTokenSecret();

private Q_SLOTS:
    void onLinkingFailed();
    void onLinkingSucceeded();
    void onLinkedChanged();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();

};

#endif // TOMBOYSERVERAUTHENTICATEJOB_H
