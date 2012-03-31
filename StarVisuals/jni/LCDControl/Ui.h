
#ifndef __UI_H
#define __UI_H

#include "ui_lcdcontrol_main_window.h"
#include "ui_lcdcontrol_crystalfontz_configure.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow window_;

    public:
    MainWindow();
    ~MainWindow();
};

class CFConfigWindow : public QMainWindow {
    Q_OBJECT

    Ui::CrystalfontzConfigure window_;

    public:
    CFConfigWindow();
    ~CFConfigWindow();
};
#endif
