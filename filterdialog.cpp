#include "filterdialog.h"
#include "ui_filterdialog.h"
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    category = static_cast<int>(NoteCategory::All);

    setDialogButtons();

    connect(ui->allRadioButton, &QRadioButton::toggled,
            this, &FilterDialog::allRadioButtonToggled);

    connect(ui->genRadioButton, &QRadioButton::toggled,
            this, &FilterDialog::genRadioButtonToggled);

    connect(ui->anniverRadiotButton, &QRadioButton::toggled,
            this, &FilterDialog::anniverRadioButtonToggled);

    connect(ui->meetRadioButton, &QRadioButton::toggled,
            this, &FilterDialog::meetRadioButtonToggled);

    connect(ui->workRadioButton, &QRadioButton::toggled,
            this, &FilterDialog::workRadioButtonToggled);

    connect(ui->importantRadioButton, &QRadioButton::toggled,
            this, &FilterDialog::impRadioButtonToggled);

    connect(ui->ascendRadio, &QRadioButton::toggled,
            this, &FilterDialog::ascendDateRadioButtonToggled);

    connect(ui->descendRadio, &QRadioButton::toggled,
            this, &FilterDialog::descendDateRadioButtonToggled);
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::setDialogButtons()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setStyleSheet(QStringLiteral("QPushButton{border: 1 solid gray;"
    "border-radius:15; width:90; height:30;"
    "background-color:rgb(222, 222, 222);}"
    "QPushButton:hover {background-color:#B2B2AE;}"));

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setStyleSheet(QStringLiteral("QPushButton{border: 1 solid gray;"
    "border-radius:15; width:90; height:30;"
    "background-color:rgb(222, 222, 222);}"
    "QPushButton:hover {background-color:#B2B2AE;}"));
}

int FilterDialog::getFilterResult() const
{
    return category;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void FilterDialog::paintEvent([[maybe_unused]] QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);;
    painter.setPen(Qt::black);
    painter.setBrush(palette().base());

    QRectF rec(rect().x() + 1 / 2,
                    rect().y() + 1 / 2,
                    rect().width() - 1,
                    rect().height() - 1);

    painter.drawRoundedRect(rec.translated(0.5, 0.5), 20, 20);
}

void FilterDialog::mousePressEvent(QMouseEvent *event)
{
    QRect frameRect = ui->label->contentsRect();
    isMouseHoverFrame = frameRect.contains(event->pos());

    isMousePressed = true;

    startPos = event->pos();

    QDialog::mousePressEvent(event);
}

void FilterDialog::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    QDialog::mouseReleaseEvent(event);
}

void FilterDialog::mouseMoveEvent(QMouseEvent* event)
{

    if(isMousePressed  && isMouseHoverFrame )
    {
        QPoint deltaPos = event->pos() - startPos;
        move(pos() + deltaPos);
    }
    QDialog::mouseMoveEvent(event);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
//////////////////////////////////////////////////////////////////SLOTS
void FilterDialog::allRadioButtonToggled(bool toggled)
{
    if(toggled)
        category  = static_cast<int>(NoteCategory::All);
}

void FilterDialog::genRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(NoteCategory::General);
}

void FilterDialog::impRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(NoteCategory::Important);
}

void FilterDialog::workRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(NoteCategory::Work);
}

void FilterDialog::anniverRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(NoteCategory::Anniversary);
}

void FilterDialog::meetRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(NoteCategory::Meeting);
}

void FilterDialog::ascendDateRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(DateFilter::Ascending);
}

void FilterDialog::descendDateRadioButtonToggled(bool toggled)
{
    if(toggled)
        category = static_cast<int>(DateFilter::Descending);
}
//////////////////////////////////////////////////////////////////SLOTS

