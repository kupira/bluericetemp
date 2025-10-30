#include "UI.hpp"
#include <thread>
#include <glibmm/main.h>

UI::UI(const std::string& glade_path, const std::string& css_path) {
    setupBuilder(glade_path);
    setupContextMenu();

    if (refresh_button)
        refresh_button->signal_clicked().connect(sigc::mem_fun(*this, &UI::on_refresh_clicked));

    if (device_list) {
        device_list->signal_row_activated().connect(sigc::mem_fun(*this, &UI::on_device_clicked));
        device_list->signal_button_press_event().connect(
            sigc::mem_fun(*this, &UI::on_device_right_click), false);
    }

    load_css(css_path);
    if (window)
        window->show_all();
}

Gtk::Window* UI::get_window() { return window; }

void UI::load_css(const std::string& css_path) {
    auto provider = Gtk::CssProvider::create();
    provider->load_from_path(css_path);
    Gtk::StyleContext::add_provider_for_screen(
        Gdk::Screen::get_default(),
        provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void UI::on_refresh_clicked() {
    for (auto child : device_list->get_children())
        device_list->remove(*child);
    row_device_map.clear();

    for (auto &device : bluetooth.get_devices()) {
        auto row = Gtk::make_managed<Gtk::ListBoxRow>();
        std::string label_text = (device.name != "" ? device.name : device.address)
                               + (device.connected ? " " : "");
        auto label = Gtk::make_managed<Gtk::Label>(label_text);

        label->set_xalign(0);
        label->get_style_context()->add_class("device-label");
                                                             
        row->add(*label);
        device_list->add(*row);
        row_device_map[row] = device;
    }
    device_list->show_all();
}

void UI::on_device_clicked(Gtk::ListBoxRow* row) {
    auto it = row_device_map.find(row);
    if (it == row_device_map.end()) return;
    DeviceInfo dev = it->second;

    std::thread([this, dev]() mutable {
        if (dev.connected)
            bluetooth.disconnect(dev);
        else
            bluetooth.connect(dev);
        Glib::signal_idle().connect_once([this]() { on_refresh_clicked(); });
    }).detach();
}

bool UI::on_device_right_click(GdkEventButton* event) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        if (auto row = device_list->get_row_at_y(event->y)) {
            int index = row->get_index();
            auto devices = bluetooth.get_devices();
            if (index >= 0 && index < (int)devices.size())
                selected_device = devices[index];
            context_menu.popup_at_pointer((GdkEvent*)event);
            return true;
        }
    }
    return false;
}

void UI::on_connect_selected() {
    std::thread([this]() {
        bluetooth.connect(selected_device);
        Glib::signal_idle().connect_once([this]() { on_refresh_clicked(); });
    }).detach();
}

void UI::on_disconnect_selected() {
    std::thread([this]() {
        bluetooth.disconnect(selected_device);
        Glib::signal_idle().connect_once([this]() { on_refresh_clicked(); });
    }).detach();
}

void UI::on_pair_selected() {
    std::thread([this]() {
        bluetooth.pair(selected_device);
        Glib::signal_idle().connect_once([this]() { on_refresh_clicked(); });
    }).detach();
}

void UI::on_remove_selected() {
    std::thread([this]() {
        //bluetooth.remove(selected_device);
        Glib::signal_idle().connect_once([this]() { on_refresh_clicked(); });
    }).detach();
}

void UI::setupContextMenu() {
    context_menu.append(item_connect);
    context_menu.append(item_disconnect);
    context_menu.append(item_pair);
    context_menu.append(item_remove);
    context_menu.show_all();

    context_menu.get_style_context()->add_class("context_menu");
    item_connect.get_style_context()->add_class("context_menu_item");
    item_disconnect.get_style_context()->add_class("context_menu_item");
    item_pair.get_style_context()->add_class("context_menu_item");
    item_remove.get_style_context()->add_class("context_menu_item_danger");

    item_connect.signal_activate().connect(sigc::mem_fun(*this, &UI::on_connect_selected));
    item_disconnect.signal_activate().connect(sigc::mem_fun(*this, &UI::on_disconnect_selected));
    item_pair.signal_activate().connect(sigc::mem_fun(*this, &UI::on_pair_selected));
    item_remove.signal_activate().connect(sigc::mem_fun(*this, &UI::on_remove_selected));
}

void UI::setupBuilder(const std::string glade_path) {
    builder = Gtk::Builder::create_from_file(glade_path);
    builder->get_widget("main_window", window);
    builder->get_widget("device_list", device_list);
    builder->get_widget("refresh_button", refresh_button);
}

