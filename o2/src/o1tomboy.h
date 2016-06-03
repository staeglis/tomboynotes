#ifndef O1TOMBOY_H
#define O1TOMBOY_H

#include "o1.h"

class O1Tomboy : public O1 {
    Q_OBJECT
public:
    explicit O1Tomboy(QObject *parent = 0) : O1(parent) {

    }

    void setBaseURL(const QString &value) {
        setRequestTokenUrl(QUrl(value + "/oauth/oauth_request.php"));
        setAuthorizeUrl(QUrl(value + "/oauth/oauth_authorize.php"));
        setAccessTokenUrl(QUrl(value + "/oauth/oauth_access.php"));
        setClientId("anyone");
        setClientSecret("anynone");
    }

};

#endif // O1TOMBOY_H
