#ifndef TOMBOYJOBBASE_H
#define TOMBOYJOBBASE_H

#include <KIO/AccessManager>
#include <kcompositejob.h>
#include <QString>
#include "o1tomboy.h"
#include "o1requestor.h"

class TomboyJobBase : public KCompositeJob
{
     Q_OBJECT
public:
    explicit TomboyJobBase(QObject *parent = Q_NULLPTR);

    void setServerURL(const QString &url, const QString &username);
    void setAuthentication(const QString &token, const QString &secret);

protected:
    KIO::Integration::AccessManager *manager;
    O1Requestor *requestor;
    O1Tomboy *o1;

    QString userURL;
};

#endif // TOMBOYJOBBASE_H