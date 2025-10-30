#include "bluetooth.hpp"
#include <iostream>

Bluetooth::Bluetooth() {
    try {
        connection_ = Gio::DBus::Connection::get_sync(Gio::DBus::BusType::SYSTEM);

        manager_ = Gio::DBus::Proxy::create_sync(
            connection_,
            "org.bluez",
            "/",
            "org.freedesktop.DBus.ObjectManager"
        );

        //TODO: add auto detection 
        adapter_ = Gio::DBus::Proxy::create_sync(
            connection_,
            "org.bluez",
            "/org/bluez/hci0",
            "org.bluez.Adapter1"
        );

    } catch (const Glib::Error& e) {
        std::cerr << "Bluetooth init error: " << e.what() << std::endl;
    }
}

Glib::RefPtr<Gio::DBus::Proxy> Bluetooth::getDeviceProxy(const DeviceInfo& dev) {
    auto it = device_proxies_.find(dev.path);
    if (it != device_proxies_.end())
        return it->second;

    try {
        auto proxy = Gio::DBus::Proxy::create_sync(
            connection_,
            "org.bluez",
            dev.path,
            "org.bluez.Device1"
        );
        device_proxies_[dev.path] = proxy;
        return proxy;
    } catch (const Glib::Error& e) {
        std::cerr << "Failed to create device proxy: " << e.what() << std::endl;
        return {};
    }
}

std::vector<DeviceInfo> Bluetooth::get_devices() const {
    std::vector<DeviceInfo> devices;
    try {
        auto result = manager_->call_sync("GetManagedObjects");
        Glib::VariantContainerBase dict;
        result.get_child(dict, 0);

        auto objects = Glib::VariantBase::cast_dynamic<
            Glib::Variant<std::map<Glib::DBusObjectPathString,
                std::map<Glib::ustring,
                    std::map<Glib::ustring, Glib::VariantBase>>>>>(dict);

        for (const auto& obj : objects.get()) {
            const auto& path = obj.first;
            const auto& interfaces = obj.second;

            auto dev_it = interfaces.find("org.bluez.Device1");
            if (dev_it != interfaces.end()) {
                DeviceInfo dev;
                dev.path = path;
                for (const auto& prop : dev_it->second) {
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

    } catch (const Glib::Error& e) {
        std::cerr << "DBus read error: " << e.what() << std::endl;
    }

    return devices;
}

bool Bluetooth::pair(const DeviceInfo& dev) {
    auto proxy = getDeviceProxy(dev);
    if (!proxy) return false;

    try {
        proxy->call_sync("Pair");
        std::cout << "Paired: " << dev.name << std::endl;
        return true;
    } catch (const Glib::Error& e) {
        std::cerr << "Pair failed for " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::connect(const DeviceInfo& dev) {
    auto proxy = getDeviceProxy(dev);
    if (!proxy) return false;

    try {
        proxy->call_sync("Connect");
        std::cout << "Connected: " << dev.name << std::endl;
        return true;
    } catch (const Glib::Error& e) {
        std::cerr << "Connect failed for " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::disconnect(const DeviceInfo& dev) {
    auto proxy = getDeviceProxy(dev);
    if (!proxy) return false;

    try {
        proxy->call_sync("Disconnect");
        std::cout << "Disconnected: " << dev.name << std::endl;
        return true;
    } catch (const Glib::Error& e) {
        std::cerr << "Disconnect failed for " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::remove(const DeviceInfo& dev) {
    if (!adapter_) return false;

    try {
        adapter_->call_sync("RemoveDevice", Glib::VariantContainerBase::create_tuple({
            Glib::Variant<Glib::DBusObjectPathString>::create(dev.path)
        }));
        std::cout << "Removed device: " << dev.name << std::endl;
        device_proxies_.erase(dev.path);
        return true;
    } catch (const Glib::Error& e) {
        std::cerr << "Remove failed for " << dev.name << ": " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::start_discovery() {
    if (!adapter_) return false;
    try {
        adapter_->call_sync("StartDiscovery");
        return true;
    } catch (const Glib::Error& e) {
        std::cerr << "Start discovery failed: " << e.what() << std::endl;
        return false;
    }
}

bool Bluetooth::stop_discovery() {
    if (!adapter_) return false;
    try {
        adapter_->call_sync("StopDiscovery");
        return true;
    } catch (const Glib::Error& e) {
        std::cerr << "Stop discovery failed: " << e.what() << std::endl;
        return false;
    }
}

