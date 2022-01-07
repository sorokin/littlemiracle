#include "sharpness_line_edit.h"
#include <cassert>
#include <QKeyEvent>

sharpness_line_edit::sharpness_line_edit(QWidget* parent)
    : QLineEdit(parent)
{
    setText(last_valid_text);
    connect(this, &QLineEdit::textChanged, this, &sharpness_line_edit::text_changed);
}

double sharpness_line_edit::get_value() const
{
    bool ok;
    double tmp = last_valid_text.toDouble(&ok);
    assert(ok && tmp >= 0.);
    return tmp;
}

void sharpness_line_edit::text_changed(QString const& text)
{
    double tmp;
    bool ok;
    tmp = text.toDouble(&ok);
    if (ok && tmp >= 0.)
    {
        setPalette(QPalette());
        last_valid_text = text;
        emit value_changed(tmp);
        return;
    }

    QPalette palette;
    palette.setColor(QPalette::Text, Qt::red);
    setPalette(palette);
}

void sharpness_line_edit::focusOutEvent(QFocusEvent* event)
{
    revert_edits();
    QLineEdit::focusOutEvent(event);
}

void sharpness_line_edit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        revert_edits();
        event->accept();
        return;
    }

    QLineEdit::keyPressEvent(event);
}

void sharpness_line_edit::revert_edits()
{
    setText(last_valid_text);
}
