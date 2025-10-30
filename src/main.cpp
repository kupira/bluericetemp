#include <chrono>
#include <iostream>
#include <gtkmm.h>
#include <thread>
#include "App.hpp"
#include "bluetooth.hpp"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.bluerice.app");
    App mainApp;

    Bluetooth bt;

    bt.startDiscovery();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    auto devices = bt.get_devices();

    bt.stopDiscovery();

    for (auto &d : devices) {
        std::cout << d.name << " [" << d.address << "] "
                  << (d.connected ? "󰄬 Connected" : "") << std::endl;
    }

    return app->run(*mainApp.get_window());
}

