#ifndef TOMBOYJOBBASE_H
#define TOMBOYJOBBASE_H

#include <KIO/AccessManager>
#include <kjob.h>
#include "o1tomboy.h"
#include "o1requestor.h"

class TomboyJobBase : public KJob
{
     Q_OBJECT
public:
    explicit TomboyJobBase(QObject *parent = Q_NULLPTR);

protected:
    KIO::AccessManager *manager;
    O1Requestor *requestor;
    O1Tomboy *o1;
};

#endif // TOMBOYJOBBASE_H
