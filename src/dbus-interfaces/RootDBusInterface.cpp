#include "RootDBusInterface.hpp"

#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QUuid>

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
      "/accounts.json";
  wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), 0,
                                       KWallet::Wallet::OpenType::Synchronous);

  if (!wallet->hasFolder(WALLET_FOLDER_NAME)) {
    wallet->createFolder(WALLET_FOLDER_NAME);
  }
  wallet->setFolder(WALLET_FOLDER_NAME);

  QFile accountJsonFile(accountsJsonFilePath);

  if (!accountJsonFile.open(QIODevice::ReadWrite)) {
    qWarning("Couldn't open config file.");
  }

  accountsJsonObject =
      QJsonDocument::fromJson(accountJsonFile.readAll()).object();

  accountJsonFile.close();
}

void RootDBusInterface::writeAccountsJsonObjectToFile() {
  QFile accountJsonFile(accountsJsonFilePath);

  if (!accountJsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    qWarning("Couldn't open config file.");
  }

  accountJsonFile.write(QJsonDocument(accountsJsonObject).toJson());
  accountJsonFile.close();
}

QList<QVariant> RootDBusInterface::getAccountNames() {
  qDebug() << "getAccountNames :";

  QList<QVariant> list;
  QJsonArray accounts = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();
  QList<QString> accountsStringArray;

  for (int i = 0; i < accounts.size(); i++) {
    QJsonObject account = accounts[i].toObject();
    list.append(account[JSON_ACCOUNT_ARRAY_FIELD_ACCOUNTNAME].toString());
  }

  return list;
}

QMap<QString, QVariant> RootDBusInterface::getAccount(QString id) {
  qDebug() << "getAccount :" << id;

  QMap<QString, QVariant> returnVal;
  QJsonArray accounts = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();
  QList<QString> accountsStringArray;

  for (int i = 0; i < accounts.size(); i++) {
    QJsonObject account = accounts[i].toObject();

    if (account[JSON_ACCOUNT_ARRAY_FIELD_ID] == id) {
      QByteArray password;
      wallet->readEntry(account[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString(),
                        password);

      returnVal.insert(
          "name", account[JSON_ACCOUNT_ARRAY_FIELD_ACCOUNTNAME].toString());
      returnVal.insert("username",
                       account[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString());
      returnVal.insert("password",
                       QString::fromStdString(password.toStdString()));
      returnVal.insert("extras",
                       account[JSON_ACCOUNT_ARRAY_FIELD_EXTRAS].toString());
    }
  }

  return returnVal;
}

QString RootDBusInterface::createAccount(QString name, QString username,
                                         QString password, QString extras) {
  qDebug() << "createAccount :" << name << username << password << extras;

  if (!accountsJsonObject.contains(JSON_FIELD_ACCOUNTS)) {
    accountsJsonObject[JSON_FIELD_ACCOUNTS] = QJsonArray();
  }

  QString id = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
  QJsonArray accountsArray = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();

  QJsonObject accountObject;
  accountObject[JSON_ACCOUNT_ARRAY_FIELD_EXTRAS] = extras;
  accountObject[JSON_ACCOUNT_ARRAY_FIELD_USERNAME] = username;
  accountObject[JSON_ACCOUNT_ARRAY_FIELD_ACCOUNTNAME] = name;
  accountObject[JSON_ACCOUNT_ARRAY_FIELD_ID] = id;

  wallet->writeEntry(username,
                     QByteArray::fromStdString(password.toStdString()));

  accountsArray.append(accountObject);
  accountsJsonObject[JSON_FIELD_ACCOUNTS] = accountsArray;

  writeAccountsJsonObjectToFile();

  return id;
}

bool RootDBusInterface::removeAccount(QString id) {
  qDebug() << "removeAccount :" << id;

  bool accountDeleted = false;
  QJsonArray accountsArray = accountsJsonObject[JSON_FIELD_ACCOUNTS].toArray();

  for (int i = 0; i < accountsArray.size(); i++) {
    QJsonObject accountObject = accountsArray[i].toObject();

    if (accountObject[JSON_ACCOUNT_ARRAY_FIELD_ID].toString() == id) {
      qDebug() << "Removing account" << id;

      accountsArray.removeAt(i);
      accountsJsonObject[JSON_FIELD_ACCOUNTS] = accountsArray;
      wallet->removeEntry(
          accountObject[JSON_ACCOUNT_ARRAY_FIELD_USERNAME].toString());

      accountDeleted = true;
      break;
    }
  }

  writeAccountsJsonObjectToFile();

  return accountDeleted;
}
