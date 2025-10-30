#pragma once
#include <gtkmm.h>
#include "bluetooth.hpp"
#include <map>

class UI {
public:
    UI(const std::string& ui_path, const std::string& css_path);
    Gtk::Window* get_window();

private:
    Glib::RefPtr<Gtk::Builder> builder;
    Glib::RefPtr<Gtk::CssProvider> css_provider;
    Gtk::Window* window = nullptr;
    Gtk::ListBox* device_list = nullptr;
    Gtk::Button* refresh_button = nullptr;

    Gtk::PopoverMenu* context_menu = nullptr;

    Bluetooth bluetooth;
    DeviceInfo selected_device;
    std::map<Gtk::ListBoxRow*, DeviceInfo> row_device_map;

    void setupBuilder(const std::string& ui_path);
    void setupContextMenu();
    void load_css(const std::string& css_path);

    void on_refresh_clicked();
    void on_device_clicked(Gtk::ListBoxRow* row);

    void on_connect_selected();
    void on_disconnect_selected();
    void on_pair_selected();
    void on_remove_selected();
};

