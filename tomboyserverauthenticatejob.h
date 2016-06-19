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

    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onLinkingFailed();
    void onLinkingSucceeded();
    void onLinkedChanged();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();

};

#endif // TOMBOYSERVERAUTHENTICATEJOB_H
