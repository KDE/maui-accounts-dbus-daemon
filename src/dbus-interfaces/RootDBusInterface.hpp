#ifndef DBUS_INTERFACES_ROOTDBUSINTERFACE_HPP
#define DBUS_INTERFACES_ROOTDBUSINTERFACE_HPP

#include "DBusInterface.hpp"

#include <KWallet>
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
  KWallet::Wallet *wallet;
  QJsonObject accountsJsonObject;

  const QString WALLET_FOLDER_NAME = "org.mauikit.accounts";
  const QString WALLET_ENTRY_ACCOUNTS = "accounts";
  const QString JSON_FIELD_ACCOUNTS = "accounts";
  const QString JSON_ACCOUNT_ARRAY_FIELD_EXTRAS = "extras";
  const QString JSON_ACCOUNT_ARRAY_FIELD_USERNAME = "username";
  const QString JSON_ACCOUNT_ARRAY_FIELD_ACCOUNTNAME = "account_name";
  const QString JSON_ACCOUNT_ARRAY_FIELD_ID = "_id";

  QString accountsJsonFilePath;

  void writeAccountsJsonObjectToFile();

 public slots:
  Q_SCRIPTABLE QList<QVariant> getAccountNames();
  Q_SCRIPTABLE QMap<QString, QVariant> getAccount(QString id);
  Q_SCRIPTABLE QString createAccount(QString name, QString username,
                                     QString password, QString extras);
  Q_SCRIPTABLE bool removeAccount(QString id);
};

#endif
