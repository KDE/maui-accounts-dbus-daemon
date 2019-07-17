#ifndef DBUS_INTERFACES_ROOTDBUSINTERFACE_HPP
#define DBUS_INTERFACES_ROOTDBUSINTERFACE_HPP

#include "DBusInterface.hpp"

#include <QJsonObject>
#include <QList>
#include <QString>
#include <QVariant>

class RootDBusInterface : public DBusInterface {
  Q_OBJECT

 public:
  QString name() override;

  RootDBusInterface();

 private:
  QString masterPassword = "";
  QJsonObject accountsJsonObject;

  const QString JSON_FIELD_ACCOUNTS = "accounts";
  const QString JSON_ACCOUNT_ARRAY_FIELD_ID = "_id";
  const QString JSON_ACCOUNT_ARRAY_FIELD_SECRET = "secret";
  const QString JSON_ACCOUNT_ARRAY_FIELD_APPID = "appId";
  const QString JSON_ACCOUNT_ARRAY_FIELD_TYPE = "type";
  const QString JSON_ACCOUNT_ARRAY_FIELD_USERNAME = "username";
  const QString JSON_ACCOUNT_ARRAY_FIELD_PASSWORD = "password";
  const QString JSON_ACCOUNT_ARRAY_FIELD_URL = "url";

  QString accountsJsonFilePath;

  void writeAccountsJsonObjectToFile();
  QString getManifestPath(QString appId);
  void readAccountsDB();

 public slots:
  Q_SCRIPTABLE bool isPasswordSet();
  Q_SCRIPTABLE void setPassword(QString password);
  Q_SCRIPTABLE QList<QVariant> getAccountIds();
  Q_SCRIPTABLE QList<QVariant> getAccountIdsByType(QString type);
  Q_SCRIPTABLE QMap<QString, QVariant> getAccount(QString id);
  Q_SCRIPTABLE QString getAccountPassword(QString secret);
  Q_SCRIPTABLE QString createWebDAVAccount(QString appId, QString username,
                                           QString password, QString url);
  Q_SCRIPTABLE QString createCardDAVAccount(QString appId, QString username,
                                            QString password, QString url);
  Q_SCRIPTABLE bool removeAccount(QString id);
};

#endif
