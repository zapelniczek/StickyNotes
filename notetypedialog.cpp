#include "notetypedialog.h"
#include "ui_notetypedialog.h"
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>

NoteTypeDialog::NoteTypeDialog(NoteCategory category_ ,QWidget *parent) :
   QDialog(parent), ui(new Ui::NoteTypeDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->listWidget->setCurrentRow(static_cast<int>(category_));
    category = category_;

    adjustButtons();

    connect(ui->listWidget, &QListWidget::itemSelectionChanged,
            this, &NoteTypeDialog::setCategory);

    connect(ui->listWidget, &QListWidget::doubleClicked,
            this, &NoteTypeDialog::setCategoryAndClose);
}

NoteTypeDialog::~NoteTypeDialog()
{
    delete ui;
}

void NoteTypeDialog::setSizeHintForItems()
{
    int cnt_items = ui->listWidget->count();
    QSize hint = ui->listWidget->size();
    hint.setHeight(hint.height()/cnt_items);

    for (int i = 0; i < cnt_items; ++i)
    {
        ui->listWidget->item(i)->setSizeHint(hint);
    }
}
NoteCategory NoteTypeDialog::getCategory() const
{
    return category;
}

void NoteTypeDialog::adjustButtons()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setStyleSheet(QStringLiteral("QPushButton{border: 1 solid gray;\n"
    "border-radius:15;\n width:90;\n height:30;\n"
    "background-color:rgb(222, 222, 222);\n}\n"
    "QPushButton:hover { background-color:#B2B2AE;}"));

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setStyleSheet(QStringLiteral("QPushButton{border: 1 solid gray;\n"
    "border-radius:15;\n width:90;\n height:30;\n"
    "background-color:rgb(222, 222, 222);\n}\n"
    "QPushButton:hover { background-color:#B2B2AE;}"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void NoteTypeDialog::paintEvent([[maybe_unused]] QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(palette().base());

    QRectF rec(rect().x() + 1 / 2,
                    rect().y() + 1 / 2,
                    rect().width() - 1,
                    rect().height() - 1);

    painter.drawRoundedRect(rec.translated(0.5, 0.5), 20, 20);
}

void NoteTypeDialog::mousePressEvent(QMouseEvent *event)
{
    QRect frameRect = ui->label->contentsRect();
    isMouseHoverFrame = frameRect.contains(event->pos());

    isMousePressed = true;

    startPos = event->pos();

    QDialog::mousePressEvent(event);
}

void NoteTypeDialog::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    QDialog::mouseReleaseEvent(event);
}

void NoteTypeDialog::mouseMoveEvent(QMouseEvent* event)
{
    if(isMousePressed  && isMouseHoverFrame )
    {
        QPoint deltaPos = event->pos() - startPos;
        move(pos() + deltaPos);
    }
    QDialog::mouseMoveEvent(event);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
///////////////////////////////////////////////////////SLOTS
void NoteTypeDialog::setCategoryAndClose(const QModelIndex& index)
{
    category = static_cast<NoteCategory>(index.row());
    accept();
}
void NoteTypeDialog::setCategory()
{
    category = static_cast<NoteCategory>(ui->listWidget->currentIndex().row());
}
///////////////////////////////////////////////////////SLOTS
