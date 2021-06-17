#ifndef NOTETYPEDIALOG_H
#define NOTETYPEDIALOG_H

#include <QDialog>
#include "categorylistwidget.h"
#include "Header.h"

namespace Ui {
class NoteTypeDialog;
}

class NoteTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteTypeDialog(NoteCategory = NoteCategory::General, QWidget *parent = nullptr );
    ~NoteTypeDialog();

    void setSizeHintForItems();

    NoteCategory getCategory() const;

protected:
    void paintEvent(QPaintEvent*) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void setCategoryAndClose(const QModelIndex&);

    void setCategory();

private:
    void adjustButtons();

private:
    Ui::NoteTypeDialog *ui;

    NoteCategory category;

    bool isMouseHoverFrame;
    bool isMousePressed;
    QPoint startPos;
};

#endif // NOTETYPEDIALOG_H
