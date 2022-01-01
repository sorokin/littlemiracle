#include "main_window.h"
#include "auto_visibility_controller.h"
#include "./ui_main_window.h"
#include <cassert>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::main_window)
{
    ui->setupUi(this);
    connect(ui->spinBox, &QSpinBox::textChanged, this, &main_window::numerator_changed);
    connect(ui->spinBox_2, &QSpinBox::textChanged, this, &main_window::denominator_changed);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &main_window::smoothness_changed);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(visibility_changed()));
    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_2, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_3, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_4, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    connect(ui->checkBox_5, SIGNAL(clicked(bool)), this, SLOT(checkbox_changed()));
    denominator_changed();
    smoothness_changed();
    visibility_changed();
}

main_window::~main_window()
{
    delete ui;
}

void main_window::numerator_changed()
{
    ui->widget->num = ui->spinBox->value();
    if (avc)
        avc->goto_star();
}

void main_window::denominator_changed()
{
    ui->widget->denom = ui->spinBox_2->value();
    ui->spinBox->setMaximum(ui->widget->denom - 1);
    if (ui->widget->denom != 1 && ui->spinBox->value() == 0)
        ui->spinBox->setValue(1);
    if (avc)
        avc->goto_star();
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
    ui->widget->sharpness = tmp;
    if (avc)
        avc->goto_star();
}

void main_window::visibility_changed()
{
    switch (ui->comboBox->currentIndex())
    {
    case 0:
        ui->checkBox->setEnabled(false);
        ui->checkBox_2->setEnabled(false);
        ui->checkBox_3->setEnabled(false);
        ui->checkBox_4->setEnabled(false);
        ui->checkBox_5->setEnabled(false);
        assert(!avc);
        avc = new auto_visibility_controller(this, ui->widget);
        break;
    case 1:
        ui->checkBox->setEnabled(true);
        ui->checkBox_2->setEnabled(true);
        ui->checkBox_3->setEnabled(true);
        ui->checkBox_4->setEnabled(true);
        ui->checkBox_5->setEnabled(true);
        delete avc;
        avc = nullptr;
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
