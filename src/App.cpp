#include "App.hpp"

App::App() : ui("../ui/main.glade", "../ui/style.css") {}
Gtk::Window* App::get_window() { return ui.get_window(); }

