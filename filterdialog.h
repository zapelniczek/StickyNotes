#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include "Header.h"

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();

    int getFilterResult() const;

protected:
    void paintEvent(QPaintEvent*) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void allRadioButtonToggled(bool);

    void genRadioButtonToggled(bool);

    void impRadioButtonToggled(bool);

    void workRadioButtonToggled(bool);

    void anniverRadioButtonToggled(bool);

    void meetRadioButtonToggled(bool);

    void ascendDateRadioButtonToggled(bool);

    void descendDateRadioButtonToggled(bool);

private:
    void setDialogButtons();

private:
    Ui::FilterDialog *ui;

    int category;

    bool isMouseHoverFrame;
    bool isMousePressed;
    QPoint startPos;
};

#endif // FILTERDIALOG_H
