#include "dbus-interfaces/DBusInterface.hpp"
#include "dbus-interfaces/RootDBusInterface.hpp"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>

#define SERVICE_NAME "org.mauikit.accounts"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr,
            "Cannot connect to the D-Bus session bus.\n"
            "To start it, run:\n"
            "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
    fprintf(stderr, "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    exit(1);
  }

  RootDBusInterface *interface = new RootDBusInterface();
  QDBusConnection::sessionBus().registerObject(
      "/", interface->name(), interface, QDBusConnection::ExportAllSlots);

  return app.exec();
}
