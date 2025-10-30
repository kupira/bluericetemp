#pragma once
#include <giomm.h>
#include <glibmm.h>
#include <string>
#include <vector>
#include <map>

struct DeviceInfo {
    std::string name;
    std::string address;
    Glib::DBusObjectPathString path;
    bool connected = false;
};

class Bluetooth {
public:
    Bluetooth();

    Glib::RefPtr<Gio::DBus::Proxy> getDeviceProxy(const DeviceInfo& dev);

    std::vector<DeviceInfo> get_devices() const;

    // Device operations
    bool pair(const DeviceInfo& dev);
    bool connect(const DeviceInfo& dev);
    bool disconnect(const DeviceInfo& dev);
    bool remove(const DeviceInfo& dev);

    bool start_discovery();
    bool stop_discovery();

private:
    Glib::RefPtr<Gio::DBus::Connection> connection_;
    Glib::RefPtr<Gio::DBus::Proxy> adapter_;
    Glib::RefPtr<Gio::DBus::Proxy> manager_;

    std::map<Glib::DBusObjectPathString, Glib::RefPtr<Gio::DBus::Proxy>> device_proxies_;

    Glib::RefPtr<Gio::DBus::Proxy> get_adapter_proxy();
};

