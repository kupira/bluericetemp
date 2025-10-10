#include "bluetooth.hpp"
#include <iostream>

Bluetooth::Bluetooth() {
    try {
        connection = Gio::DBus::Connection::get_sync(Gio::DBus::BUS_TYPE_SYSTEM);
    } catch (const Glib::Error &e) {
        std::cerr << "DBus connection error: " << e.what() << std::endl;
    }
}


void start_scan() {
    //WARN: remove hardcoded adapter
    call("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", "StartDiscovery");
    std::cout << "Scan started.";

}
void stop_scan() {
    call("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", "StopDiscovery");
    std::cout << "Scan stopped.";
}

std::vector<DeviceInfo> Bluetooth::get_devices() {
    std::vector<DeviceInfo> devices;
    try {
        auto proxy = Gio::DBus::Proxy::create_sync(
            connection,
            "org.bluez",          // Bus name
            "/",                  // Root path
            "org.freedesktop.DBus.ObjectManager"
        );

        auto result = proxy->call_sync("GetManagedObjects");
        Glib::VariantContainerBase dict;
        result.get_child(dict, 0);

        auto objects = Glib::VariantBase::cast_dynamic<Glib::Variant<std::map<Glib::DBusObjectPathString,
                           std::map<Glib::ustring,
                           std::map<Glib::ustring, Glib::VariantBase>>>>>(dict);

        for (const auto &obj : objects.get()) {
            const auto &path = obj.first;
            const auto &interfaces = obj.second;

            auto dev_it = interfaces.find("org.bluez.Device1");
            if (dev_it != interfaces.end()) {
                DeviceInfo dev;
                dev.path = path;
                for (const auto &prop : dev_it->second) {
                    if (prop.first == "Name")
                        dev.name = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring>>(prop.second).get();
                    else if (prop.first == "Address")
                        dev.address = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring>>(prop.second).get();
                    else if (prop.first == "Connected")
                        dev.connected = Glib::VariantBase::cast_dynamic<Glib::Variant<bool>>(prop.second).get();
                }
                devices.push_back(dev);
            }
        }

    } catch (const Glib::Error &e) {
        std::cerr << "DBus read error: " << e.what() << std::endl;
    }

    return devices;
}

