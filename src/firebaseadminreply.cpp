#include "firebaseadminreply.h"

#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

FirebaseAdminReply::FirebaseAdminReply(QNetworkReply *reply)
    : QObject(reply), m_reply(reply) {}

void FirebaseAdminReply::setNetworkReply(QNetworkReply *reply) {
  m_reply = reply;

  QElapsedTimer t;
  t.start();
  connect(reply, &QNetworkReply::finished, this, [=, this] {
    const QByteArray data = reply->readAll();
    qDebug() << "FirebaseAdminReply finished" << reply->error() << data
             << "elapsed" << t.elapsed();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    const QJsonObject obj = doc.object();
    if (reply->error()) {
      const QJsonObject errorJ = obj[u"error"].toObject();
      m_error = true;
      m_errorCode = errorJ[u"code"].toInt();
      m_errorMessage = errorJ[u"message"].toString();
    } else {
      m_error = false;
      m_messageId = obj[u"name"].toString();
      qDebug() << "FirebaseAdminReply finished success" << m_messageId;
    }
    m_data = obj;
    Q_EMIT finished(this);
  });
}

bool FirebaseAdminReply::error() const { return m_error; }

QString FirebaseAdminReply::messageId() const { return m_messageId; }

int FirebaseAdminReply::errorCode() const { return m_errorCode; }

QString FirebaseAdminReply::errorMessage() const { return m_errorMessage; }

QJsonObject FirebaseAdminReply::data() const { return m_data; }
