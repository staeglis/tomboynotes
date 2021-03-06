#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QString>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QUrl>
#include <QDebug>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

#include "o2/o2replyserver.h"

O2ReplyServer::O2ReplyServer(QObject *parent): QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(onIncomingConnection()));
    replyContent_ = "<HTML></HTML>";
}

void O2ReplyServer::onIncomingConnection()
{
    QTcpSocket *socket = nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(onBytesReady()), Qt::UniqueConnection);
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void O2ReplyServer::onBytesReady()
{
    qDebug() << "O2ReplyServer::onBytesReady";
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }
    QByteArray reply;
    reply.append("HTTP/1.0 200 OK \r\n");
    reply.append("Content-Type: text/html; charset=\"utf-8\"\r\n");
    reply.append(QString("Content-Length: %1\r\n\r\n").arg(replyContent_.size()));
    reply.append(replyContent_);
    socket->write(reply);

    QByteArray data = socket->readAll();
    QMap<QString, QString> queryParams = parseQueryParams(&data);
    socket->disconnectFromHost();
    close();
    Q_EMIT verificationReceived(queryParams);
}

QMap<QString, QString> O2ReplyServer::parseQueryParams(QByteArray *data)
{
    qDebug() << "O2ReplyServer::parseQueryParams";

    QString splitGetLine = QString(*data).split("\r\n").first();
    splitGetLine.remove("GET ");
    splitGetLine.remove("HTTP/1.1");
    splitGetLine.remove("\r\n");
    splitGetLine.prepend("http://localhost");
    QUrl getTokenUrl(splitGetLine);

    QList< QPair<QString, QString> > tokens;
#if QT_VERSION < 0x050000
    tokens = getTokenUrl.queryItems();
#else
    QUrlQuery query(getTokenUrl);
    tokens = query.queryItems();
#endif
    QMultiMap<QString, QString> queryParams;
    QPair<QString, QString> tokenPair;
    foreach (tokenPair, tokens) {
        // FIXME: We are decoding key and value again. This helps with Google OAuth, but is it mandated by the standard?
        QString key = QUrl::fromPercentEncoding(QByteArray().append(tokenPair.first.trimmed()));
        QString value = QUrl::fromPercentEncoding(QByteArray().append(tokenPair.second.trimmed()));
        queryParams.insert(key, value);
    }
    return queryParams;
}

QByteArray O2ReplyServer::replyContent()
{
    return replyContent_;
}

void O2ReplyServer::setReplyContent(const QByteArray &value)
{
    replyContent_ = value;
}
