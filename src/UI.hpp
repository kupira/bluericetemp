#pragma once
#include <gtkmm.h>
#include "bluetooth.hpp"
#include <map>

class UI {
public:
    UI(const std::string& glade_path, const std::string& css_path);
    Gtk::Window* get_window();

private:
    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Window* window = nullptr;
    Gtk::ListBox* device_list = nullptr;
    Gtk::Button* refresh_button = nullptr;

    Gtk::Menu context_menu;
    Gtk::MenuItem item_connect{"Connect"};
    Gtk::MenuItem item_disconnect{"Disconnect"};
    Gtk::MenuItem item_pair{"Pair"};
    Gtk::MenuItem item_remove{"Remove"};

    std::map<Gtk::ListBoxRow*, DeviceInfo> row_device_map;
    DeviceInfo selected_device;

    Bluetooth bluetooth;

    void load_css(const std::string& css_path);
    void on_refresh_clicked();
    void on_device_clicked(Gtk::ListBoxRow* row);
    bool on_device_right_click(GdkEventButton* event);

    void on_connect_selected();
    void on_disconnect_selected();
    void on_pair_selected();
    void on_remove_selected();

    void setupBuilder(const std::string glade_path);
    void setupContextMenu();
};

