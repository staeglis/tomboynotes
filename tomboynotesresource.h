#ifndef TOMBOYNOTESRESOURCE_H
#define TOMBOYNOTESRESOURCE_H


#include <QtNetwork>
#include <AkonadiAgentBase/ResourceBase>
#include <KIO/AccessManager>
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
    // Standard akonadi slots
    void retrieveCollections() Q_DECL_OVERRIDE;
    void retrieveItems(const Akonadi::Collection &col) Q_DECL_OVERRIDE;
    bool retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts) Q_DECL_OVERRIDE;

    // Slots for Job result handling
    void onAuthorizationFinished(KJob *kjob);
    void onCollectionsRetrieved(KJob *kjob);
    void onItemChangeCommitted(KJob *kjob);
    void onItemRetrieved(KJob *kjob);
    void onItemsRetrieved(KJob *kjob);

private Q_SLOTS:
    // Status handling
    void clearStatusMessage();
    void onSslError(QNetworkReply *reply, const QList<QSslError> &errors);

protected:
    void aboutToQuit() Q_DECL_OVERRIDE;

    // Standard akonadi
    void itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection) Q_DECL_OVERRIDE;
    void itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts) Q_DECL_OVERRIDE;
    void itemRemoved(const Akonadi::Item &item) Q_DECL_OVERRIDE;

private:
    bool configurationNotValid();

    void retryAfterFailure(const QString &errorMessage);
    // Status handling
    void showError(const QString errorText);
    QTimer *mStatusMessageTimer;

    // Only one UploadJob should run per time
    bool mUploadJobProcessRunning;

    KIO::AccessManager *mManager;
};

#endif
