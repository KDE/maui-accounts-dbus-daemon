#ifndef ENCRYPTIONHELPER_HPP
#define ENCRYPTIONHELPER_HPP

#include <QString>

class EncryptionHelper {
 public:
  static QString encrypt(QString password, QString text);
  static QString decrypt(QString password, QString filePath);
};

#endif
