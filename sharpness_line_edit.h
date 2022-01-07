#pragma once
#include <QLineEdit>
#include <QPalette>

struct sharpness_line_edit : QLineEdit
{
private:
    Q_OBJECT

public:
    explicit sharpness_line_edit(QWidget* parent = nullptr);
    double get_value() const;

signals:
    void value_changed(double value);

private slots:
    void text_changed(QString const& text);

private:
    void focusOutEvent(QFocusEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void revert_edits();

private:
    QString last_valid_text = "0";
    QColor default_text_color;
};
