#include <dbus/dbus.h>
void panic(char* msg);
void panic_on_dbus_err(DBusError* err);