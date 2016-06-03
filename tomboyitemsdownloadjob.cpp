#include "tomboyitemsdownloadjob.h"


TomboyItemsDownloadJob::TomboyItemsDownloadJob(const Akonadi::Collection::Id &id, QObject *parent)
    : TomboyJobBase(parent)
{
    collectionId = id;
}

void TomboyItemsDownloadJob::start()
{

}

void TomboyItemsDownloadJob::onResult(KJob *kjob)
{

}
