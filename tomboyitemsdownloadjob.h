#ifndef TOMBOYITEMSDOWNLOADJOB_H
#define TOMBOYITEMSDOWNLOADJOB_H

#include <AkonadiAgentBase/ResourceBase>
#include "tomboyjobbase.h"

class TomboyItemsDownloadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    // ctor
    TomboyItemsDownloadJob(const Akonadi::Collection::Id &id, QObject *parent = 0);
     // returns the parsed results wrapped in Akonadi::Item, see bellow
    Akonadi::Item::List items() const;
protected:
    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private:
    Akonadi::Collection::Id collectionId;
    Akonadi::Item::List pitems;
};

#endif // TOMBOYITEMSDOWNLOADJOB_H
