#include "bluetooth.hpp"
#include <iostream>

Bluetooth::Bluetooth() {
    try {
        connection_ = Gio::DBus::Connection::get_sync(Gio::DBus::BUS_TYPE_SYSTEM);
    } catch (const Glib::Error &e) {
        std::cerr << "DBus connection error: " << e.what() << std::endl;
    }
//TODO: remove hardcoded values in adapter and manager    
    try {
        adapter_ = Gio::DBus::Proxy::create_sync(
            connection_,
            "org.bluez",
            "/org/bluez/hci0",
            "org.bluez.Adapter1"
        );
    } catch (const Glib::Error &e) {
        std::cerr << "Adapter error: " << e.what() << std::endl;
    }

    try {
        manager_ = Gio::DBus::Proxy::create_sync(
            connection_,
            "org.bluez",
            "/",
            "org.freedesktop.DBus.ObjectManager"
        );
    } catch (const Glib::Error &e) {
        std::cerr << "Manager error: " << e.what() << std::endl;
    }
}

Glib::RefPtr<Gio::DBus::Proxy> Bluetooth::getDeviceProxy(const DeviceInfo& dev) {
    // If already exists
    auto it = device_proxies_.find(dev.path);
    if (it != device_proxies_.end())
        return it->second;

    auto proxy = Gio::DBus::Proxy::create_sync(
        connection_,
        "org.bluez",
        dev.path,
        "org.bluez.Device1"
    );

    device_proxies_[dev.path] = proxy;
    return proxy;
}

const std::vector<DeviceInfo> Bluetooth::get_devices() const{
    std::vector<DeviceInfo> devices;
    try {
        auto result = manager_->call_sync("GetManagedObjects");
        Glib::VariantContainerBase dict;
        result.get_child(dict, 0);

        auto objects = Glib::VariantBase::cast_dynamic<Glib::Variant<
            std::map<Glib::DBusObjectPathString,
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

bool Bluetooth::connect(const DeviceInfo& dev) {
    try {
        auto device_proxy = getDeviceProxy(dev);

        device_proxy->call_sync("Connect");
        this->pair(dev); //WARN: DEBUG ONLY
                          
        std::cout << "Connected to: " << dev.name << std::endl;
        return true;
    } catch (const Glib::Error &e) {
        std::cerr << "Connect failed for " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::disconnect(const DeviceInfo& dev) {
    try {
        auto device_proxy = getDeviceProxy(dev);
        device_proxy->call_sync("Disconnect");
        std::cout << "Disconnected from: " << dev.name << std::endl;
        return true;
    } catch (const Glib::Error &e) {
        std::cerr << "Disconnect failed for " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::pair(const DeviceInfo& dev) {
    try {
        auto device_proxy = getDeviceProxy(dev);

        try {
            device_proxy->call_sync("Pair");
            std::cout << "Paired device: " << dev.name << std::endl;
        } catch (const Glib::Error &e) {
            std::cerr << "Pairing skipped or failed for " << dev.name
                << e.what() << std::endl;
        }

        return true;
    } catch (const Glib::Error &e) {
        std::cerr << "Failed to pair device " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::startDiscovery() {
    try {
        adapter_->call_sync("StartDiscovery");
        return true;
    } catch (const Glib::Error &e) {
        std::cerr << "Failed to start discovery: " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::stopDiscovery() {
    try {
        adapter_->call_sync("StopDiscovery");
        return true;
    } catch (const Glib::Error &e) {
        std::cerr << "Failed to stop discovery: " << e.what() << std::endl;
        return false;
    }
}
