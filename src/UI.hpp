#pragma once
#include "bluetooth.hpp"
#include <gtkmm.h>

class UI {
public:
    UI(const std::string& glade_path, const std::string& css_path);
    Gtk::Window* get_window();

private:
    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Window* window = nullptr;

    Gtk::ListBox* device_list = nullptr;
    Gtk::Button* refresh_button = nullptr;

    Bluetooth bluetooth;

    void load_css(const std::string& css_path);
    void on_refresh_clicked();
};

