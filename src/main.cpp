#include "dbus-interfaces/DBusInterface.hpp"
#include "dbus-interfaces/RootDBusInterface.hpp"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>

#define SERVICE_NAME "org.mauikit.accounts"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::systemBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus system bus.\n");
    return 1;
  }

  if (!QDBusConnection::systemBus().registerService(SERVICE_NAME)) {
    fprintf(stderr, "%s\n",
            qPrintable(QDBusConnection::systemBus().lastError().message()));
    exit(1);
  }

  RootDBusInterface *interface = new RootDBusInterface();
  QDBusConnection::systemBus().registerObject("/", interface->name(), interface,
                                              QDBusConnection::ExportAllSlots);

  return app.exec();
}
