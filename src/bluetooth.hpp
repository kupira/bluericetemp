#pragma once
#include <giomm.h>
#include <string>
#include <vector>

struct DeviceInfo {
    std::string name;
    std::string address;
    std::string path;
    bool connected = false;
};

class Bluetooth {
public:
    Bluetooth();

    Glib::RefPtr<Gio::DBus::Proxy> getDeviceProxy(const DeviceInfo& dev);
    const std::vector<DeviceInfo> get_devices() const;

    bool connect(const DeviceInfo& dev);
    bool disconnect(const DeviceInfo& dev);
    bool pair(const DeviceInfo& dev);
    bool startDiscovery();
    bool stopDiscovery();

private:
    Glib::RefPtr<Gio::DBus::Connection> connection_;
    Glib::RefPtr<Gio::DBus::Proxy> adapter_;
    Glib::RefPtr<Gio::DBus::Proxy> manager_;

    std::map<std::string, Glib::RefPtr<Gio::DBus::Proxy>> device_proxies_;
};

