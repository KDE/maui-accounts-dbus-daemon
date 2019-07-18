#include "RootDBusInterface.hpp"
#include "../EncryptionHelper.hpp"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QTimer>
#include <QUuid>
#include <libdavclient/CardDAV.hpp>
#include <libdavclient/WebDAV.hpp>
#include <libdavclient/utils/CardDAVReply.hpp>
#include <libdavclient/utils/WebDAVReply.hpp>

QString RootDBusInterface::name() { return "org.mauikit.accounts"; }

RootDBusInterface::RootDBusInterface() {
  QDir appDataFolder(QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::AppDataLocation));
  if (!appDataFolder.exists()) {
    appDataFolder.mkpath(".");
  }

  accountsJsonFilePath =
      QStandardPaths::writableLocation(
          QStandardPaths::StandardLocation::AppDataLocation) +
      "/accounts.db.crypt";
}

void RootDBusInterface::writeAccountsJsonObjectToFile() {
  QFile accountJsonFile(accountsJsonFilePath);

  if (!accountJsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    qWarning("Couldn't open config file.");
  }

  QByteArray accountsData = QByteArray::fromStdString(
      EncryptionHelper::encrypt(masterPassword,
                                QJsonDocument(accountsJsonObject)
                                    .toJson(QJsonDocument::JsonFormat::Compact))
          .toStdString());

  accountJsonFile.write(accountsData);
  accountJsonFile.close();
}

QString RootDBusInterface::getManifestPath(QString appId) {
  return "/usr/share/maui-accounts/manifests/" + appId + ".json";
}

void RootDBusInterface::readAccountsDB() {
  QFile accountJsonFile(accountsJsonFilePath);

  if (!accountJsonFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open config file.");
    accountsJsonObject = QJsonObject();

    writeAccountsJsonObjectToFile();
  } else {
    QString accountsData =
        EncryptionHelper::decrypt(masterPassword, accountsJsonFilePath);
    QByteArray accountsjsonData =
        QByteArray::fromStdString(accountsData.toStdString());

    accountsJsonObject = QJsonDocument::fromJson(accountsjsonData).object();

    accountJsonFile.close();

    qDebug() << accountsData;
  }
}

QString RootDBusInterface::insertAccountToDb(QString appId, QString type,
                                             QString username, QString password,
                                             QString url) {
  if (!accountsJsonObject.contains(JSON_FIELD_ACCOUNTS)) {
    accountsJsonObject[JSON_FIELD_ACCOUNTS] = QJsonArray();
  }

  bool accountFound = false;
  QString secret = "";
  QJsonArray accountsArray = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();

  for (int i = 0; i < accountsArray.size(); i++) {
    QJsonObject obj = accountsArray.at(i).toObject();

    if (obj[JSON_ACCOUNT_ARRAY_FIELD_APPID].toString() == appId &&
        obj[JSON_ACCOUNT_ARRAY_FIELD_TYPE].toString() == type &&
        obj[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString() == username &&
        obj[JSON_ACCOUNT_ARRAY_FIELD_PASSWORD].toString() == password &&
        obj[JSON_ACCOUNT_ARRAY_FIELD_URL].toString() == url) {
      qDebug().noquote() << "    Account already added";

      secret = obj[JSON_ACCOUNT_ARRAY_FIELD_SECRET].toString();
      accountFound = true;

      break;
    }
  }

  if (!accountFound) {
    secret = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);

    QJsonObject accountObject;
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_ID] =
        QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_SECRET] = secret;
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_APPID] = appId;
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_TYPE] = type;
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_USERNAME] = username;
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_PASSWORD] = password;
    accountObject[JSON_ACCOUNT_ARRAY_FIELD_URL] = url;

    accountsArray.append(accountObject);
    accountsJsonObject[JSON_FIELD_ACCOUNTS] = accountsArray;

    writeAccountsJsonObjectToFile();

    qDebug().noquote() << "    Account Created";
  }

  return secret;
}

bool RootDBusInterface::isPasswordSet() { return masterPassword != ""; }

void RootDBusInterface::setPassword(QString password) {
  masterPassword = password;
  readAccountsDB();
}

QList<QVariant> RootDBusInterface::getAccountIds() {
  // TODO : Return a List of Maps containing the account data except passwords
  return getAccountIdsByType("");
}

QList<QVariant> RootDBusInterface::getAccountIdsByType(QString type) {
  qDebug().noquote() << "* List of account names requested of type `" + type +
                            "`";
  QList<QVariant> list;
  QJsonArray accounts = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();
  QList<QString> accountsStringArray;

  for (int i = 0; i < accounts.size(); i++) {
    QJsonObject account = accounts[i].toObject();

    if (type == "" ||
        type == account[JSON_ACCOUNT_ARRAY_FIELD_TYPE].toString()) {
      list.append(account[JSON_ACCOUNT_ARRAY_FIELD_ID].toString());
    }
  }

  return list;
}

QMap<QString, QVariant> RootDBusInterface::getAccount(QString id) {
  // TODO : Create a ENUM for the return value map KEY

  qDebug().noquote() << "* Account data requested";

  QMap<QString, QVariant> returnVal;
  QJsonArray accounts = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();
  QList<QString> accountsStringArray;

  for (int i = 0; i < accounts.size(); i++) {
    QJsonObject account = accounts[i].toObject();

    if (account[JSON_ACCOUNT_ARRAY_FIELD_ID].toString() == id) {
      qDebug().noquote()
          << "    Found user `" +
                 account[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString() + "`";

      returnVal.insert("id", account[JSON_ACCOUNT_ARRAY_FIELD_ID].toString());
      returnVal.insert("appId",
                       account[JSON_ACCOUNT_ARRAY_FIELD_APPID].toString());
      returnVal.insert("type",
                       account[JSON_ACCOUNT_ARRAY_FIELD_TYPE].toString());
      returnVal.insert("username",
                       account[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString());
      returnVal.insert("url", account[JSON_ACCOUNT_ARRAY_FIELD_URL].toString());
    }
  }

  if (returnVal.size() <= 0) {
    qDebug().noquote() << "    [ERROR] Account not found";
  }

  return returnVal;
}

QString RootDBusInterface::getAccountPassword(QString secret) {
  qDebug().noquote() << "* Account password requested";

  QJsonArray accounts = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();

  for (int i = 0; i < accounts.size(); i++) {
    QJsonObject account = accounts[i].toObject();

    if (account[JSON_ACCOUNT_ARRAY_FIELD_SECRET].toString() == secret) {
      qDebug().noquote()
          << "    Found user `" +
                 account[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString() + "`";

      return account[JSON_ACCOUNT_ARRAY_FIELD_PASSWORD].toString();
    }
  }

  qDebug().noquote() << "    [ERROR] Account not found";
  return "";
}

QString RootDBusInterface::createWebDAVAccount(QString appId, QString username,
                                               QString password, QString url) {
  qDebug().noquote() << "* Create Request for WebDAV Account `" + username +
                            "` Received";

  QString secret = "";
  QEventLoop *qloop = new QEventLoop();

  if (QFile(getManifestPath(appId)).exists()) {
    WebDAV *m_WebDAV = new WebDAV(url, username, password);
    WebDAVReply *reply = m_WebDAV->testConnection();

    this->connect(reply, &WebDAVReply::testConnectionResponse,
                  [=, &secret](bool isSuccess) {
                    if (isSuccess) {
                      qDebug().noquote() << "    Success connecting to Server";

                      secret = insertAccountToDb(appId, "WEBDAV", username,
                                                 password, url);

                      QTimer::singleShot(0, qloop, &QEventLoop::quit);
                    } else {
                      qDebug().noquote()
                          << "    [ERROR] Could not connect to server";
                    }
                  });
    this->connect(reply, &WebDAVReply::error,
                  [=](QNetworkReply::NetworkError err) {
                    qDebug().noquote()
                        << "    [ERROR] Could not connect to server" << err;
                    qDebug().noquote() << "    [ERROR] Account not created";

                    QTimer::singleShot(0, qloop, &QEventLoop::quit);
                  });

    qloop->exec();
  } else {
    qDebug().noquote() << "    [ERROR] Invalid `appId`";
  }

  return secret;
}

QString RootDBusInterface::createCardDAVAccount(QString appId, QString username,
                                                QString password, QString url) {
  qDebug().noquote() << "* Create Request for CardDAV Account `" + username +
                            "` Received";

  QString secret = "";
  QEventLoop *qloop = new QEventLoop();

  if (QFile(getManifestPath(appId)).exists()) {
    CardDAV *m_CardDAV = new CardDAV(url, username, password);
    CardDAVReply *reply = m_CardDAV->testConnection();

    this->connect(reply, &CardDAVReply::testConnectionResponse,
                  [=, &secret](bool isSuccess) {
                    if (isSuccess) {
                      qDebug().noquote() << "    Success connecting to Server";

                      secret = insertAccountToDb(appId, "CARDDAV", username,
                                                 password, url);

                      QTimer::singleShot(0, qloop, &QEventLoop::quit);
                    }
                  });
    this->connect(reply, &CardDAVReply::error,
                  [=](QNetworkReply::NetworkError err) {
                    qDebug().noquote()
                        << "    [ERROR] Could not connect to server" << err;
                    qDebug().noquote() << "    [ERROR] Account not created";

                    QTimer::singleShot(0, qloop, &QEventLoop::quit);
                  });

    qloop->exec();
  } else {
    qDebug().noquote() << "    [ERROR] Invalid `appId`";
  }

  return secret;
}

bool RootDBusInterface::removeAccount(QString id) {
  qDebug().noquote() << "* Remove Account Requested";

  bool accountDeleted = false;
  QJsonArray accountsArray = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();

  for (int i = 0; i < accountsArray.size(); i++) {
    QJsonObject accountObject = accountsArray[i].toObject();

    if (accountObject[JSON_ACCOUNT_ARRAY_FIELD_ID].toString() == id) {
      qDebug().noquote()
          << "    Account Removed with username `" +
                 accountObject[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString() +
                 "`";

      accountsArray.removeAt(i);
      accountsJsonObject[JSON_FIELD_ACCOUNTS] = accountsArray;

      accountDeleted = true;
      break;
    }
  }

  writeAccountsJsonObjectToFile();

  if (!accountDeleted) {
    qDebug().noquote() << "    [ERROR] Account not found";
  }

  return accountDeleted;
}
