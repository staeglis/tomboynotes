#ifndef TOMBOYITEMUPLOADJOB_H
#define TOMBOYITEMUPLOADJOB_H

#include <AkonadiAgentBase/ResourceBase>
#include <KMime/Message>
#include "tomboyjobbase.h"

enum class JobType
{
    addItem,
    modifyItem,
    deleteItem
};

class TomboyItemUploadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    TomboyItemUploadJob(const Akonadi::Item &item, JobType jobType, QObject *parent = 0);

    // Returns mSourceItem for post-processing purposes
    Akonadi::Item item() const;

    JobType jobType();

    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // This will be called once the request is finished.
    void onRequestFinished();

private:
    // Workaround for https://bugreports.qt-project.org/browse/QTBUG-26161 Qt bug
    QString getCurrentISOTime();

    Akonadi::Item mSourceItem;
    KMime::Message::Ptr mNoteContent;

    JobType mJobType;

    int mRemoteRevision;
};

#endif // TOMBOYITEMUPLOADJOB_H
