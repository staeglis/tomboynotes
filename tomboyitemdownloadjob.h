#ifndef TOMBOYITEMDOWNLOADJOB_H
#define TOMBOYITEMDOWNLOADJOB_H

#include <AkonadiAgentBase/ResourceBase>
#include "tomboyjobbase.h"

class TomboyItemDownloadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    TomboyItemDownloadJob(const Akonadi::Item &item, QObject *parent = 0);
    // returns the parsed results wrapped in Akonadi::Item, see bellow
    Akonadi::Item item() const;

    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // This will be called once the request is finished.
    void onRequestFinished();

private:
    Akonadi::Item mResultItem;
};

#endif // TOMBOYITEMDOWNLOADJOB_H
