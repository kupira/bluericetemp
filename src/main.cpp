#include <iostream>
#include <gtkmm.h>
#include "App.hpp"
#include "bluetooth.hpp"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.bluerice.app");
    App mainApp;

    Bluetooth bt;

    for (auto &d : bt.get_devices()) {
        std::cout << d.name << " [" << d.address << "] "
                  << (d.connected ? "✓ Connected" : "✗ Disconnected") << std::endl;
    }

    return app->run(*mainApp.get_window());
}

