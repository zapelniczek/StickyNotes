#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class ColorSelector;
}

class ColorSelector : public QDialog
{
    Q_OBJECT

public:
    enum CurrentColor : long  {yellow = 15652711, green = 9887633  , pink = 15639504,
                               purple  = 13214702 , blue  = 9687278, gray = 13750737 , black = 5263440};

public:
    explicit ColorSelector(CurrentColor color = CurrentColor::gray ,QWidget *parent = nullptr);
    ~ColorSelector();

signals:
    void newColorSelected(const long);

private slots:
    void yellowButton_clicked();

    void greenButton_clicked();

    void pinkButton_clicked();

    void purpleButton_clicked();

    void blueButton_clicked();

    void grayButton_clicked();

    void blackButton_clicked();

private:
    void uncheckPrevNoteColor();

    void closeAnimation_Fun();

    void hashButtonToColor();

    Ui::ColorSelector *ui;

    QHash<CurrentColor, QPushButton*> colorButton;

    CurrentColor noteColor = CurrentColor::gray;
};

#endif // COLORSELECTOR_H
