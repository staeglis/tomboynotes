#ifndef TOMBOYCOLLECTIONSDOWNLOADJOB_H
#define TOMBOYCOLLECTIONSDOWNLOADJOB_H

#include <AkonadiAgentBase/ResourceBase>
#include "tomboyjobbase.h"

class TomboyCollectionsDownloadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    // ctor
    TomboyCollectionsDownloadJob(QObject *parent = 0);
     // returns the parsed results wrapped in Akonadi::Item, see bellow
    Akonadi::Collection::List collections();

    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // This will be called once the request is finished. You process the response
    // and emitResult(), to which your code in the resource will react.
    void onRequestFinished();

private:
    Akonadi::Collection::List resultCollections;
    //Akonadi::Collection &parentCollection;
};

#endif // TOMBOYCOLLECTIONSDOWNLOADJOB_H
