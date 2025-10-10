#pragma once
#include <giomm.h>
#include <vector>
#include <string>

struct DeviceInfo {
    std::string path;
    std::string name;
    std::string address;
    bool connected;
};

class Bluetooth {
public:
    Bluetooth();

    std::vector<DeviceInfo> get_devices();

    void start_scan(); 
    void stop_scan();

private:
    Glib::RefPtr<Gio::DBus::Connection> connection;
    Glib::VariantContainerBase call(const std::string& bus,
                                    const std::string& path,
                                    const std::string& iface,
                                    const std::string& method);
};
