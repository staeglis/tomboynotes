#include "tomboyitemsdownloadjob.h"


TomboyItemsDownloadJob::TomboyItemsDownloadJob(const Akonadi::Collection::Id &id, QObject *parent)
    : TomboyJobBase(parent)
{
    collectionId = id;
}

Akonadi::Item::List TomboyItemsDownloadJob::items() const
{
    return pitems;
}

void TomboyItemsDownloadJob::start()
{

}

void TomboyItemsDownloadJob::onResult(KJob *kjob)
{

}
