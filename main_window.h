// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
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
    void visibility_changed();

private:
    void update_labels();
    void update_checkboxes();
    void update_reset_to_default_action();

private:
    Ui::main_window *ui;
    auto_visibility_controller* avc = nullptr;
};
