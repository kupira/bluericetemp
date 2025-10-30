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

        // Правий клік через GestureClick
        auto gesture = Gtk::GestureClick::create();
        gesture->set_button(GDK_BUTTON_SECONDARY);
        gesture->signal_pressed().connect([this](int, double x, double y) {
            if (auto row = device_list->get_row_at_y(y)) {
                selected_device = row_device_map[row];
                context_menu->popup();
            }
        });
        device_list->add_controller(gesture);
    }

    load_css(css_path);
    if (window)
        window->show();
}

Gtk::Window* UI::get_window() { return window; }

void UI::load_css(const std::string& css_path) {
    auto provider = Gtk::CssProvider::create();
    provider->load_from_path(css_path);
    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(),
        provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

void UI::on_refresh_clicked() {
    device_list->remove_all();
    row_device_map.clear();

    for (auto &device : bluetooth.get_devices()) {
        auto row = Gtk::make_managed<Gtk::ListBoxRow>();
        std::string label_text = (device.name.empty() ? device.address : device.name)
                               + (device.connected ? " " : "");
        auto label = Gtk::make_managed<Gtk::Label>(label_text);
        label->set_xalign(0);
        row->set_child(*label);

        device_list->append(*row);
        row_device_map[row] = device;
    }
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

void UI::setupBuilder(const std::string& glade_path) {
    builder = Gtk::Builder::create_from_file(glade_path);

    window = builder->get_widget<Gtk::Window>("main_window");
    device_list = builder->get_widget<Gtk::ListBox>("device_list");
    refresh_button = builder->get_widget<Gtk::Button>("refresh_button");
}

void UI::setupContextMenu() {
    context_menu = Gtk::make_managed<Gtk::PopoverMenu>();
    auto menu_model = Gio::Menu::create();
    menu_model->append("Connect", "app.connect");
    menu_model->append("Disconnect", "app.disconnect");
    menu_model->append("Pair", "app.pair");
    menu_model->append("Remove", "app.remove");

    context_menu->set_menu_model(menu_model);
    context_menu->set_parent(*window);  // замість add()
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
        // TODO: implement remove
        Glib::signal_idle().connect_once([this]() { on_refresh_clicked(); });
    }).detach();
}

