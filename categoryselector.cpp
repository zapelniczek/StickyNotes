#include "categoryselector.h"
#include "ui_categoryselector.h"

#include <QPainter>

CategorySelector::CategorySelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CategorySelector)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->listWidget, &QListWidget::clicked,
            this, &CategorySelector::setCategory);
}

CategorySelector::~CategorySelector()
{
    delete ui;
}

void CategorySelector::setSizeHintItems()
{
    int cnt_items = ui->listWidget->count();
    QSize hint = ui->listWidget->size();
    hint.setHeight(hint.height()/ cnt_items);

    for (int i = 0; i < cnt_items; ++i)
        ui->listWidget->item(i)->setSizeHint(hint);
}

NoteCategory CategorySelector::getNoteCategory() const
{
    return std::move(category);
}

void CategorySelector::prepareWindow(const QColor& baseColor, const QColor& textColor)
{
    QString format = QStringLiteral("QListWidget{background-color:")+
            baseColor.name(QColor::HexRgb) + QStringLiteral(";}")+
            QStringLiteral("QListWidget::item{color:") + textColor.name(QColor::HexRgb) + QStringLiteral(";};");

    QString newStyleSheet = ui->listWidget->styleSheet();

    if(newStyleSheet.contains(QStringLiteral("background-color")))
    {
        int index = newStyleSheet.indexOf(QStringLiteral("QListWidget{background-color:"));
        newStyleSheet.remove(index, format.size());
    }
    newStyleSheet.append(format);
    ui->listWidget->setStyleSheet(newStyleSheet);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void CategorySelector::paintEvent([[maybe_unused]] QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(ui->listWidget->palette().base());
    painter.drawRoundedRect(rect(), 20, 20);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
///////////////////////////////////////////////////////SLOTS
void CategorySelector::setCategory(const QModelIndex& index)
{
    category = static_cast<NoteCategory>(index.row());
    accept();
}
///////////////////////////////////////////////////////SLOTS
