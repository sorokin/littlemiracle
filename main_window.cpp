// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "main_window.h"
#include "auto_visibility_controller.h"
#include "./ui_main_window.h"
#include <cassert>

namespace
{
    QString polygon_name(size_t n)
    {
        switch (n)
        {
        case 0:
            assert(false);
            return "<error>";
        case 1:
            return "Dots";
        case 2:
            return "Lines";
        case 3:
            return "Triangles";
        case 4:
            return "Squares";
        case 5:
            return "Pentagons";
        case 6:
            return "Hexagons";
        default:
            return QString("%1-gons").arg(n);
        }
    }
}

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::main_window)
{
    ui->setupUi(this);

    ui->triangles_color_button->setColor(QColor::fromRgbF(0.25, 0.6, 0.225));
    ui->squares_color_button->setColor(QColor::fromRgbF(0.9, 0.6, 0.1));
    ui->circle_color_button->setColor(QColor::fromRgbF(0.9, 0.3, 0.45));
    ui->star_color_button->setColor(QColor::fromRgbF(64. / 255., 163. / 255., 199. / 255.));
    ui->dots_color_button->setColor(QColor::fromRgbF(71. / 255., 64. / 255., 199. / 255.));
    connect(ui->numerator_spinbox, &QSpinBox::textChanged, this, &main_window::numerator_changed);
    connect(ui->denominator_spinbox, &QSpinBox::textChanged, this, &main_window::denominator_changed);
    connect(ui->sharpness_edit, &sharpness_line_edit::value_changed, this, [this] (double value)
    {
        ui->animation->set_sharpness(value);
        if (avc)
            avc->goto_star();
        update_reset_to_default_action();
    });
    connect(ui->visibility_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(visibility_changed()));
    connect(ui->star_checkbox, &QCheckBox::toggled, this, [this] (bool checked)
    {
        ui->animation->set_visibility(chart_element::star, checked);
    });
    connect(ui->triangles_checkbox, &QCheckBox::toggled, this, [this] (bool checked)
    {
        ui->animation->set_visibility(chart_element::triangles, checked);
    });
    connect(ui->squares_checkbox, &QCheckBox::toggled, this, [this] (bool checked)
    {
        ui->animation->set_visibility(chart_element::squares, checked);
    });
    connect(ui->circle_checkbox, &QCheckBox::toggled, this, [this] (bool checked)
    {
        ui->animation->set_visibility(chart_element::circle, checked);
    });
    connect(ui->dots_checkbox, &QCheckBox::toggled, this, [this] (bool checked)
    {
        ui->animation->set_visibility(chart_element::dots, checked);
    });

    connect(ui->star_color_button, &KColorButton::changed, this, [this] (QColor color)
    {
        ui->animation->set_color(chart_element::star, color);
    });
    connect(ui->triangles_color_button, &KColorButton::changed, this, [this] (QColor color)
    {
        ui->animation->set_color(chart_element::triangles, color);
    });
    connect(ui->squares_color_button, &KColorButton::changed, this, [this] (QColor color)
    {
        ui->animation->set_color(chart_element::squares, color);
    });
    connect(ui->circle_color_button, &KColorButton::changed, this, [this] (QColor color)
    {
        ui->animation->set_color(chart_element::circle, color);
    });
    connect(ui->dots_color_button, &KColorButton::changed, this, [this] (QColor color)
    {
        ui->animation->set_color(chart_element::dots, color);
    });

    connect(ui->antialiasing_checkbox, &QCheckBox::toggled, this, [this] (bool checked)
    {
        ui->animation->set_antialiasing(checked);
    });

    connect(ui->animation, &star_widget::reset_to_default_triggered, this, [this]()
    {
        ui->denominator_spinbox->setValue(7);
        ui->numerator_spinbox->setValue(3);
        ui->sharpness_edit->setText("0.93");
        ui->visibility_combobox->setCurrentIndex(0);
    });

    denominator_changed();
    ui->animation->set_sharpness(ui->sharpness_edit->get_value());

    visibility_changed();
    ui->animation->set_color(chart_element::star, ui->star_color_button->color());
    ui->animation->set_color(chart_element::triangles, ui->triangles_color_button->color());
    ui->animation->set_color(chart_element::squares, ui->squares_color_button->color());
    ui->animation->set_color(chart_element::circle, ui->circle_color_button->color());
    ui->animation->set_color(chart_element::dots, ui->dots_color_button->color());

    ui->animation->set_antialiasing(ui->antialiasing_checkbox->isChecked());
    update_reset_to_default_action();
}

main_window::~main_window()
{
    delete ui;
}

void main_window::numerator_changed()
{
    ui->animation->set_desired_num(ui->numerator_spinbox->value());
    if (avc)
        avc->goto_star();
    update_labels();
    update_checkboxes();
    update_reset_to_default_action();
}

void main_window::denominator_changed()
{
    size_t denom = ui->denominator_spinbox->value();
    ui->numerator_spinbox->setMaximum(denom - 1);
    ui->animation->set_desired_denom(denom);
    if (avc)
        avc->goto_star();
    update_labels();
    update_checkboxes();
    update_reset_to_default_action();
}

void main_window::visibility_changed()
{
    switch (ui->visibility_combobox->currentIndex())
    {
    case 0:
        assert(!avc);
        avc = new auto_visibility_controller(this, ui->animation);
        update_checkboxes();
        break;
    case 1:
        delete avc;
        avc = nullptr;
        update_checkboxes();

        ui->animation->set_visibility({{
            ui->star_checkbox->isChecked(),
            ui->triangles_checkbox->isChecked(),
            ui->squares_checkbox->isChecked(),
            ui->circle_checkbox->isChecked(),
            ui->dots_checkbox->isChecked()
        }});

        break;
    default:
        assert(false);
        break;
    }
    update_reset_to_default_action();
}

void main_window::update_labels()
{
    size_t co_num = ui->animation->get_actual_co_num();

    ui->triangles_color_label->setText(polygon_name(ui->animation->get_actual_num()));
    ui->triangles_checkbox->setText(polygon_name(ui->animation->get_actual_num()));

    ui->squares_color_label->setText(polygon_name(co_num));
    ui->squares_checkbox->setText(polygon_name(co_num));
}

void main_window::update_checkboxes()
{
    bool custom = !avc;
    size_t co_num = ui->animation->get_actual_co_num();

    ui->star_checkbox->setEnabled(custom);
    ui->triangles_checkbox->setEnabled(custom && ui->animation->get_actual_num() != 1);
    ui->squares_checkbox->setEnabled(custom && co_num != 1);
    ui->circle_checkbox->setEnabled(custom);
    ui->dots_checkbox->setEnabled(custom);

    ui->triangles_color_label->setEnabled(ui->animation->get_actual_num() != 1);
    ui->squares_color_label->setEnabled(co_num != 1);
}

void main_window::update_reset_to_default_action()
{
    bool enabled = ui->numerator_spinbox->value()   != 3
                 | ui->denominator_spinbox->value() != 7
                 | ui->sharpness_edit->text()       != "0.93"
                 | ui->visibility_combobox->currentIndex() != 0;

    ui->animation->enable_reset_to_default_action(enabled);
}
