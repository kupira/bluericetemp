#include "UI.hpp"
#include <iostream>

UI::UI(const std::string& glade_path, const std::string& css_path) {
    builder = Gtk::Builder::create_from_file(glade_path);
    builder->get_widget("main_window", window);
    builder->get_widget("device_list", device_list);
    builder->get_widget("refresh_button", refresh_button);

    if (refresh_button)
        refresh_button->signal_clicked().connect(sigc::mem_fun(*this, &UI::on_refresh_clicked));

    load_css(css_path);

    if(window)
        window->show_all();
}

Gtk::Window* UI::get_window() { return window; }

void UI::load_css(const std::string& css_path) {
    auto provider = Gtk::CssProvider::create();
    provider->load_from_path(css_path);
    Gtk::StyleContext::add_provider_for_screen(
        Gdk::Screen::get_default(),
        provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

void UI::on_refresh_clicked() {
    std::cout << "Refresh clicked. Updating devices..." << std::endl;

    // Clean before refresh
    for (auto child : device_list->get_children())
        device_list->remove(*child);

    for (auto &device : bluetooth.get_devices()) {
        auto row = Gtk::make_managed<Gtk::ListBoxRow>();

        std::string preLabel = "";
        preLabel += device.name;
        preLabel += device.connected ? " [connected]" : "";
        auto label = Gtk::make_managed<Gtk::Label>(preLabel);
        row->add(*label);

        device_list->add(*row);
    }
    device_list->show_all();
}
