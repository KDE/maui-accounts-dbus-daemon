#ifndef DBUS_INTERFACES_DBUSINTERFACE_HPP
#define DBUS_INTERFACES_DBUSINTERFACE_HPP

#include <QObject>

class DBusInterface : public QObject {
  Q_OBJECT

 public:
  virtual QString name() = 0;
};

#endif
