#include "colorselector.h"
#include "ui_colorselector.h"

ColorSelector::ColorSelector(CurrentColor color, QWidget *parent) :
    QDialog(parent), ui(new Ui::ColorSelector), noteColor(color)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint |Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);

    hashButtonToColor();
    colorButton[noteColor]->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));

    connect(ui->blueButton, &QPushButton::clicked,
            this, &ColorSelector::blueButton_clicked);

    connect(ui->blackButton, &QPushButton::clicked,
            this, &ColorSelector::blackButton_clicked);

    connect(ui->yellowButton, &QPushButton::clicked,
            this, &ColorSelector::yellowButton_clicked);

    connect(ui->grayButton, &QPushButton::clicked,
            this, &ColorSelector::grayButton_clicked);

    connect(ui->greenButton, &QPushButton::clicked,
            this, &ColorSelector::greenButton_clicked);

    connect(ui->purpleButton, &QPushButton::clicked,
            this, &ColorSelector::purpleButton_clicked);

    connect(ui->pinkButton, &QPushButton::clicked,
            this, &ColorSelector::pinkButton_clicked);
}

ColorSelector::~ColorSelector()
{
    delete ui;
}

void ColorSelector::hashButtonToColor()
{
    colorButton.insert(CurrentColor::yellow, ui->yellowButton);
    colorButton.insert(CurrentColor::green, ui->greenButton);
    colorButton.insert(CurrentColor::pink, ui->pinkButton);
    colorButton.insert(CurrentColor::purple, ui->purpleButton);
    colorButton.insert(CurrentColor::blue, ui->blueButton);
    colorButton.insert(CurrentColor::gray, ui->grayButton);
    colorButton.insert(CurrentColor::black, ui->blackButton);
}

void ColorSelector::uncheckPrevNoteColor()
{
    colorButton[noteColor]->setIcon(QIcon());
}
///////////////////////////////////////////////////////SLOTS
void ColorSelector::yellowButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::yellow;
    ui->yellowButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    close();
    emit newColorSelected(static_cast<long>(CurrentColor::yellow));
}

void ColorSelector::greenButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::green;
    ui->greenButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    emit newColorSelected(static_cast<long>(CurrentColor::green));
}

void ColorSelector::pinkButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::pink;
    ui->pinkButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    close();
    emit newColorSelected(static_cast<long>(CurrentColor::pink));
}

void ColorSelector::purpleButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::purple;
    ui->purpleButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    close();
    emit newColorSelected(static_cast<long>(CurrentColor::purple));
}

void ColorSelector::blueButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::blue;
    ui->blueButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    close();
    emit newColorSelected(static_cast<long>(CurrentColor::blue));
}

void ColorSelector::grayButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::gray;
    ui->grayButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    close();
    emit newColorSelected(static_cast<long>(CurrentColor::gray));
}

void ColorSelector::blackButton_clicked()
{
    uncheckPrevNoteColor();
    noteColor = CurrentColor::black;
    ui->blackButton->setIcon(QIcon(QStringLiteral(":/ColorSelector/Icons/images/checked.png")));
    close();
    emit newColorSelected(static_cast<long>(CurrentColor::black));
}
///////////////////////////////////////////////////////SLOTS
