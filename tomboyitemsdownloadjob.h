#ifndef TOMBOYITEMSDOWNLOADJOB_H
#define TOMBOYITEMSDOWNLOADJOB_H

#include <AkonadiAgentBase/ResourceBase>
#include "tomboyjobbase.h"

class TomboyItemsDownloadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    // ctor
    TomboyItemsDownloadJob(const Akonadi::Collection::Id &id, KIO::AccessManager *manager, QObject *parent = 0);
    // returns the parsed results wrapped in Akonadi::Item::List, see bellow
    Akonadi::Item::List items() const;

    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // This will be called once the request is finished.
    void onRequestFinished();

private:
    Akonadi::Collection::Id mCollectionId;
    Akonadi::Item::List mResultItems;
};

#endif // TOMBOYITEMSDOWNLOADJOB_H
