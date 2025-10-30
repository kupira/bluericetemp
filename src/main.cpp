#include <gtkmm.h>
#include <iostream>
#include <thread>
#include "App.hpp"
#include "bluetooth.hpp"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.bluerice.app");

    App mainApp;  

    Bluetooth bt;
    bt.start_discovery();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    auto devices = bt.get_devices();
    bt.stop_discovery();

    for (auto &d : devices) {
        std::cout << d.name << " [" << d.address << "] "
                  << (d.connected ? "󰄬 Connected" : "") << std::endl;
    }

    app->signal_startup().connect([&]() {
        app->add_window(*mainApp.get_window());
    });

    return app->run(argc, argv);
}
