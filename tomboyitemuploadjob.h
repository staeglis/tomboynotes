#ifndef TOMBOYITEMUPLOADJOB_H
#define TOMBOYITEMUPLOADJOB_H

#include <AkonadiAgentBase/ResourceBase>
#include "tomboyjobbase.h"


class TomboyItemUploadJob : public TomboyJobBase
{
    Q_OBJECT
public:
    TomboyItemUploadJob(QObject *parent = 0);

    // automatically called by KJob
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    // This will be called once the request is finished. You process the response
    // and emitResult(), to which your code in the resource will react.
    void onRequestFinished();
};

#endif // TOMBOYITEMUPLOADJOB_H
