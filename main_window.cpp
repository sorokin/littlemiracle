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
    ui->kcolorbutton_2->setColor(QColor::fromRgbF(0.25, 0.6, 0.225));
    ui->kcolorbutton_3->setColor(QColor::fromRgbF(0.9, 0.6, 0.1));
    ui->kcolorbutton_4->setColor(QColor::fromRgbF(0.9, 0.3, 0.45));
    ui->kcolorbutton->setColor(QColor::fromRgbF(64. / 255., 163. / 255., 199. / 255.));
    ui->kcolorbutton_5->setColor(QColor::fromRgbF(71. / 255., 64. / 255., 199. / 255.));
    connect(ui->spinBox, &QSpinBox::textChanged, this, &main_window::numerator_changed);
    connect(ui->spinBox_2, &QSpinBox::textChanged, this, &main_window::denominator_changed);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &main_window::smoothness_changed);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(visibility_changed()));
    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_2, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_3, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_4, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_5, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->kcolorbutton, SIGNAL(changed(QColor)), this, SLOT(color_changed()));
    connect(ui->kcolorbutton_2, SIGNAL(changed(QColor)), this, SLOT(color_changed()));
    connect(ui->kcolorbutton_3, SIGNAL(changed(QColor)), this, SLOT(color_changed()));
    connect(ui->kcolorbutton_4, SIGNAL(changed(QColor)), this, SLOT(color_changed()));
    connect(ui->kcolorbutton_5, SIGNAL(changed(QColor)), this, SLOT(color_changed()));
    connect(ui->checkBox_6, SIGNAL(clicked(bool)), this, SLOT(antialiasing_changed()));
    denominator_changed();
    smoothness_changed();
    visibility_changed();
    color_changed();
    antialiasing_changed();
}

main_window::~main_window()
{
    delete ui;
}

void main_window::numerator_changed()
{
    ui->widget->set_desired_num(ui->spinBox->value());
    if (avc)
        avc->goto_star();
    update_labels();
    update_checkboxes();
}

void main_window::denominator_changed()
{
    size_t denom = ui->spinBox_2->value();
    ui->widget->set_desired_denom(denom);
    ui->spinBox->setMaximum(denom - 1);
    if (avc)
        avc->goto_star();
    update_labels();
    update_checkboxes();
}

void main_window::smoothness_changed()
{
    double tmp;
    bool ok;
    tmp = ui->lineEdit_3->text().toDouble(&ok);
    if (!ok)
        tmp = 0.93;
    else
    {
        if (tmp < 0.)
            tmp = 0.;
    }
    ui->widget->set_sharpness(tmp);
    if (avc)
        avc->goto_star();
}

void main_window::visibility_changed()
{
    switch (ui->comboBox->currentIndex())
    {
    case 0:
        assert(!avc);
        avc = new auto_visibility_controller(this, ui->widget);
        update_checkboxes();
        break;
    case 1:
        delete avc;
        avc = nullptr;
        update_checkboxes();
        checkbox_changed();
        break;
    default:
        assert(false);
        break;
    }
}

void main_window::checkbox_changed()
{
    assert(ui->comboBox->currentIndex() == 1);

    ui->widget->visibility[chart_element_id::triangles] = ui->checkBox->isChecked();
    ui->widget->visibility[chart_element_id::squares] = ui->checkBox_2->isChecked();
    ui->widget->visibility[chart_element_id::circles] = ui->checkBox_3->isChecked();
    ui->widget->visibility[chart_element_id::stars] = ui->checkBox_4->isChecked();
    ui->widget->visibility[chart_element_id::dots] = ui->checkBox_5->isChecked();
}

void main_window::color_changed()
{
    ui->widget->colors[chart_element_id::triangles] = ui->kcolorbutton_2->color();
    ui->widget->colors[chart_element_id::squares] = ui->kcolorbutton_3->color();
    ui->widget->colors[chart_element_id::circles] = ui->kcolorbutton_4->color();
    ui->widget->colors[chart_element_id::stars] = ui->kcolorbutton->color();
    ui->widget->colors[chart_element_id::dots] = ui->kcolorbutton_5->color();
}

void main_window::antialiasing_changed()
{
    ui->widget->enable_antialiasing = ui->checkBox_6->isChecked();
    ui->widget->update();
}

void main_window::update_labels()
{
    size_t co_num = ui->widget->get_actual_denom() - ui->widget->get_actual_num();

    ui->label_7->setText(polygon_name(ui->widget->get_actual_num()));
    ui->checkBox->setText(polygon_name(ui->widget->get_actual_num()));

    ui->label_8->setText(polygon_name(co_num));
    ui->checkBox_2->setText(polygon_name(co_num));
}

void main_window::update_checkboxes()
{
    bool custom = !avc;
    size_t co_num = ui->widget->get_actual_denom() - ui->widget->get_actual_num();

    ui->checkBox->setEnabled(custom && ui->widget->get_actual_num() != 1);
    ui->checkBox_2->setEnabled(custom && co_num != 1);
    ui->checkBox_3->setEnabled(custom);
    ui->checkBox_4->setEnabled(custom);
    ui->checkBox_5->setEnabled(custom);

    ui->label_7->setEnabled(ui->widget->get_actual_num() != 1);
    ui->label_8->setEnabled(co_num != 1);
}
