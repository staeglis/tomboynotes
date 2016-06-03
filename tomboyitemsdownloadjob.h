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
    // automatically called by KJob, here you start the HTTP request using
    // KIO::get (or KIO::stored_get) (see [0]) and connect onResult() (see
    // bellow) to its result() signal
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // called when the KJob your created above via KIO::get finishes.
    // Read the data from the job, parse them, store the results and
    // call emitResult(), which will....well, emit result() signal
    void onResult(KJob *kjob);
};

#endif // TOMBOYITEMSDOWNLOADJOB_H
