#pragma once
#include "UI.hpp"

class App {
public:
    App();
    Gtk::Window* get_window();
private:
    UI ui;
};

