#ifndef TOMBOYJOBBASE_H
#define TOMBOYJOBBASE_H

#include <KIO/AccessManager>
#include <kcompositejob.h>
#include <QString>
#include "o1tomboy.h"
#include "o1requestor.h"

enum TomboyJobError
{
    NoError,
    TemporaryError,
    PermanentError
};

class TomboyJobBase : public KCompositeJob
{
     Q_OBJECT
public:
    explicit TomboyJobBase(QObject *parent = Q_NULLPTR);

    void setServerURL(const QString &apiurl, const QString &contenturl);
    void setAuthentication(const QString &token, const QString &secret);

protected:
    KIO::Integration::AccessManager *manager;
    O1Requestor *requestor;
    O1Tomboy *o1;
    QNetworkReply *mReply;

    QString mApiURL;
    QString mContentURL;

    TomboyJobError checkReplyError();
};

#endif // TOMBOYJOBBASE_H
