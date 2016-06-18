#ifndef TOMBOYNOTESRESOURCE_H
#define TOMBOYNOTESRESOURCE_H

#include <AkonadiAgentBase/ResourceBase>
#include <QtNetwork>
#include "o1tomboy.h"
#include "o1requestor.h"

class TomboyNotesResource : public Akonadi::ResourceBase,
                           public Akonadi::AgentBase::Observer
{
    Q_OBJECT

public:
    TomboyNotesResource(const QString &id);
    ~TomboyNotesResource();

public Q_SLOTS:
    void configure(WId windowId) Q_DECL_OVERRIDE;

protected Q_SLOTS:
    void retrieveCollections() Q_DECL_OVERRIDE;
    void retrieveItems(const Akonadi::Collection &col) Q_DECL_OVERRIDE;
    bool retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts) Q_DECL_OVERRIDE;
    void onAuthorizationFinished(KJob *kjob);
    void onItemRetrieved(KJob *kjob);
    void onItemsRetrieved(KJob *kjob);

protected:
    void aboutToQuit() Q_DECL_OVERRIDE;

    void itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection) Q_DECL_OVERRIDE;
    void itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts) Q_DECL_OVERRIDE;
    void itemRemoved(const Akonadi::Item &item) Q_DECL_OVERRIDE;

private:
    QNetworkAccessManager *manager;
    O1Requestor *requestor;
    O1Tomboy *o1;
};

#endif
