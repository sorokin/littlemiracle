#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class main_window; }
QT_END_NAMESPACE

struct auto_visibility_controller;

class main_window : public QMainWindow
{
    Q_OBJECT
    
public:
    main_window(QWidget *parent = nullptr);
    ~main_window();

public slots:
    void numerator_changed();
    void denominator_changed();
    void smoothness_changed();
    void visibility_changed();
    void checkbox_changed();
    void color_changed();
    void antialiasing_changed();

private:
    Ui::main_window *ui;
    auto_visibility_controller* avc = nullptr;
};

#endif // MAIN_WINDOW_H
