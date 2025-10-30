#include "App.hpp"

App::App() : ui("../ui/main_window.ui", "../ui/style.css") {}
Gtk::Window* App::get_window() { return ui.get_window(); }

