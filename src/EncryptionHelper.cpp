#include "EncryptionHelper.hpp"

#include <QDebug>
#include <QProcess>

QString EncryptionHelper::encrypt(QString password, QString text) {
  QProcess process;
  process.start("/bin/sh", QStringList()
                               << "-c"
                               << "echo '" + text +
                                      "' | gpg --batch --armor --passphrase '" +
                                      password + "' -c -o -");
  process.waitForFinished(-1);

  QString stdout = process.readAllStandardOutput();
  QString stderr = process.readAllStandardError();

  if (stderr != "") {
    qDebug() << "ENCRYPT ERROR :" << stderr;
  }

  return stdout;
}

QString EncryptionHelper::decrypt(QString password, QString filepath) {
  QProcess process;
  process.start("/bin/sh", QStringList() << "-c"
                                         << "gpg --batch --passphrase '" +
                                                password + "' -d " + filepath);
  process.waitForFinished(-1);

  QString stdout = process.readAllStandardOutput();
  QString stderr = process.readAllStandardError();

  if (stderr != "") {
    qDebug() << "DECRYPT ERROR :" << stderr;
  }

  return stdout;
}
