#include "note.h"
#include "ui_note.h"

#include <QPropertyAnimation>
#include <QTextList>
#include <QPainter>
#include <QScrollBar>
#include <QCommonStyle>

Note::Note(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Note)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() |Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    themeColor = ColorSelector::CurrentColor::gray;

    ui->noteText->verticalScrollBar()->setStyle(new QCommonStyle);
    ui->noteText->setFocus();

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setDisabled(true);

    setNewPalette();
    makeConnections();
}

Note::Note(const QString& text , int colorID, const QColor& color_F,
           const NoteCategory& category_, const QDate& date, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Note)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setCategory(category_);
    themeColor = static_cast<ColorSelector::CurrentColor>(colorID);

    ui->noteText->setText(text);
    ui->noteText->verticalScrollBar()->setStyle(new QCommonStyle);
    QPalette pal = ui->noteText->palette();
    pal.setColor(QPalette::Text, color_F);
    ui->noteText->setPalette(pal);

    setDateOfCreation(date);
    ui->dateEdit->setDisabled(true);

    setNewPalette();
    makeConnections();
}

Note::~Note()
{
    delete ui;
}

QTextEdit* Note::getNoteText() const
{
    return ui->noteText;
}

int Note::getColorTheme() const // change to color
{
    return static_cast<int>(themeColor);
}

QColor Note::getTextColor() const
{
    return ui->noteText->textColor();
}

QString Note::getFileName() const
{
    return fileName;
}

void Note::setFileName(const QString& fileName_)
{
    fileName = fileName_;
}

QDate Note::getDate() const
{
    return ui->dateEdit->date();
}

void Note::setCategory(const NoteCategory& category_)
{
    if(category == category_)
        return;

    category = category_;
    ui->categoryButton->setIcon(getIconCategory(category_));
    ui->categoryButton->setToolTip(getTooltipCategory(category_));
    emit categoryChanged(category);
}

NoteCategory Note::getCategory() const
{
    return category;
}

void Note::swapFileNames(Note* rhs)
{
    using std::swap;
    swap(fileName, rhs->fileName);
}

void Note::makeConnections()
{
    connect(ui->noteText, &QTextEdit::cursorPositionChanged,
            this, &Note::isBold);

    connect(ui->noteText, &QTextEdit::textChanged,
            this, &Note::isBold);

    connect(ui->noteText, &QTextEdit::cursorPositionChanged,
            this, &Note::isItalic);

    connect(ui->noteText, &QTextEdit::textChanged,
            this, &Note::isItalic);

    connect (ui->noteText, &QTextEdit::cursorPositionChanged,
             this, &Note::isUnderline);

    connect(ui->noteText, &QTextEdit::textChanged,
            this, &Note::isUnderline);

    connect(ui->noteText, &QTextEdit::cursorPositionChanged,
            this, &Note::isList);

    connect(ui->noteText, &QTextEdit::textChanged,
            this, &Note::isList);

    connect(ui->noteText, &QTextEdit::textChanged,
            this, &Note::removeEmptyItem);

    connect(ui->noteText->document(), &QTextDocument::contentsChanged,
            this, &Note::textChanged);

    connect(ui->boldButton, &QPushButton::clicked,
            this, &Note::boldButton_clicked);

    connect(ui->italicButton, &QPushButton::clicked,
            this, &Note::italicButton_clicked);

    connect(ui->underlineButton, &QPushButton::clicked,
            this, &Note::underlineButton_clicked);

    connect(ui->listButton, &QPushButton::clicked,
            this, &Note::listButton_clicked);

    connect(ui->closeButton, &QPushButton::clicked,
            this, &Note::closeButton_clicked);

    connect(ui->colorButton, &QPushButton::clicked,
            this, &Note::colorButton_clicked);

    connect(ui->categoryButton, &QPushButton::clicked,
            this, &Note::categoryButton_clicked);
}

void Note::setNewPalette()
{   
    QRgb theme = static_cast<QRgb>(themeColor);
    QColor color(theme);
    color.setAlphaF(0.90);

    QPalette pal = palette();
    pal.setColor(QPalette::Base, color);
    setPalette(pal);

    QString textStyleSheet = ui->noteText->styleSheet();
    int index = textStyleSheet.indexOf(QStringLiteral("QTextEdit"));
    if (index != -1)
    {
        textStyleSheet.remove(index, 38);
        ui->noteText->setStyleSheet(textStyleSheet);
    }
    ui->noteText->setStyleSheet(QStringLiteral("QTextEdit{ background-color: ")+
                                color.name(QColor::HexRgb) + QStringLiteral("}\n")+
                                ui->noteText->styleSheet());

    updateTextColor();
    updateDateColor();
    emit themeColorChanged();
}

void Note::updateTextColor()
{
    QPalette pal = ui->noteText->palette();

    if (themeColor == ColorSelector::black)
        pal.setColor(QPalette::Text, Qt::white);
    else
        pal.setColor(QPalette::Text, Qt::black);

    ui->noteText->setPalette(pal);
}

void Note::updateDateColor()
{
    QString new_StyleSheet = ui->dateEdit->styleSheet();
    QString colorSheet;

    if (themeColor == ColorSelector::CurrentColor::black)
        colorSheet = QStringLiteral("QDateEdit{\ncolor:white;\n}");
    else
        colorSheet = QStringLiteral("QDateEdit{\ncolor:black;\n}");

    int index = new_StyleSheet.indexOf(colorSheet);

    if (index == -1)
    {
        index = new_StyleSheet.indexOf(QStringLiteral("QDateEdit{\ncolor:"));
        new_StyleSheet.remove(index, colorSheet.size());
        new_StyleSheet.append(colorSheet);
        ui->dateEdit->setStyleSheet(new_StyleSheet);
    }
}

void Note::setDateOfCreation(const QDate& date)
{
    ui->dateEdit->setDate(date);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void Note::mousePressEvent(QMouseEvent *event)
{
    QRect frameRect = ui->menuLayout->contentsRect();
    isMouseHoverFrame = frameRect.contains(event->pos());

    isMousePressed = true;

    startPos = event->pos();

    QDialog::mousePressEvent(event);
}

void Note::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    QDialog::mouseReleaseEvent(event);
}

void Note::mouseMoveEvent(QMouseEvent* event)
{
    if(isMousePressed  && isMouseHoverFrame )
    {
        QPoint deltaPos = event->pos() - startPos;
        move(pos() + deltaPos);
    }
    QDialog::mouseMoveEvent(event);
}

void Note::paintEvent([[maybe_unused]] QPaintEvent* event)
{
    QPainter painter (this);
    painter.setPen(Qt::transparent);

    QColor base_color = palette().base().color();
    painter.setBrush(QBrush(base_color.darker(140)));
    painter.drawRoundedRect(rect(), 20, 20);
}

bool Note::event (QEvent * event )
{
    if (event->type() == QEvent::ActivationChange)
    {
        if(colorSelector != nullptr && QApplication::activeWindow() != colorSelector)
        {
            colorSelector->close();
            colorSelector->deleteLater();
            colorSelector = nullptr;
        }
        if(categorySelector != nullptr && QApplication::activeWindow() != categorySelector)
        {
            categorySelector->close();
            delete categorySelector;
            categorySelector = nullptr;
        }
    }
    return QWidget::event(event);
}
///////////////////////////////////////////////////////SLOTS
void Note::show()
{
    emit noteHidden(false);
    QDialog::show();
}

void Note::close()
{
    emit noteHidden(true);
    QDialog::close();
}

void Note::colorButton_clicked()
{
    colorSelector = new ColorSelector(themeColor ,this);
    colorSelector->show();
    colorSelector->setGeometry(x(), y(), width(), colorSelector->height());

    QPropertyAnimation * animation = new QPropertyAnimation(colorSelector, "size");
    animation->setDuration(300);

    animation->setStartValue(QSize(width(), 0));
    animation->setEndValue(QSize(width(), colorSelector->height()));
    animation->start();

    colorSelector->activateWindow();

    connect(colorSelector, &ColorSelector::newColorSelected,
            this, &Note::setNewColor);

    connect(animation, &QPropertyAnimation::finished,
            animation, &QPropertyAnimation::deleteLater);
}

void Note::setNewColor(long color)
{
    themeColor = static_cast<ColorSelector::CurrentColor>(color);

    setNewPalette();

    colorSelector->deleteLater();
    colorSelector = nullptr;

    ui->noteText->setFocus();
}

void Note::closeButton_clicked()
{
    if (ui->noteText->document()->isEmpty())
        emit deleteNote(this);
    else
        close();
}

void Note::categoryButton_clicked()
{
    if(categorySelector != nullptr)
        return;

    categorySelector = new CategorySelector(this);
    categorySelector->show();
    categorySelector->setGeometry(x(), y(),
                                  width(), categorySelector->height());

    QPalette pal = ui->noteText->palette();
    categorySelector->prepareWindow(pal.base().color(), pal.text().color());

    QPropertyAnimation * animation = new QPropertyAnimation(categorySelector, "size");
    animation->setDuration(300);

    animation->setStartValue(QSize(0, categorySelector->height()));
    animation->setEndValue(QSize(width() - 150, categorySelector->height()));
    animation->start();

    categorySelector->activateWindow();

    connect(categorySelector, &QDialog::finished,
            this, &Note::acquireCategory);

    connect(animation, &QPropertyAnimation::finished,
            animation, &QPropertyAnimation::deleteLater);
}

void Note::acquireCategory()
{
    if(categorySelector->result() == QDialog::Accepted)
    {
        NoteCategory category = categorySelector->getNoteCategory();
        setCategory(category);
    }
    categorySelector->deleteLater();
    categorySelector = nullptr;
}

void Note::boldButton_clicked()
{
    QTextCursor selection = ui->noteText->textCursor();
    QTextCharFormat textFormat = selection.charFormat();

    if (ui->boldButton->isChecked())
        textFormat.setFontWeight(99);
    else
        textFormat.setFontWeight(50);

    selection.setCharFormat(textFormat);
    ui->noteText->setTextCursor(selection);

    ui->noteText->setFocus();
}

void Note::isBold()
{
    QTextCursor selection = ui->noteText->textCursor();
    selection.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    QTextCharFormat textFormat = selection.charFormat();

    if(textFormat.font().bold())
        ui->boldButton->setChecked(true);
    else
        ui->boldButton->setChecked(false);
}

void Note::italicButton_clicked()
{
    QTextCursor selection = ui->noteText->textCursor();
    QTextCharFormat textFormat = selection.charFormat();

    if(ui->italicButton->isChecked())
        textFormat.setFontItalic(true);
    else
        textFormat.setFontItalic(false);

    selection.setCharFormat(textFormat);
    ui->noteText->setTextCursor(selection);

    ui->noteText->setFocus();
}

void Note::isItalic()
{
    QTextCursor selection = ui->noteText->textCursor();
    selection.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    QTextCharFormat textFormat = selection.charFormat();

    if(textFormat.font().italic())
        ui->italicButton->setChecked(true);
    else
        ui->italicButton->setChecked(false);
}

void Note::underlineButton_clicked()
{
    QTextCursor selection = ui->noteText->textCursor();
    QTextCharFormat textFormat = selection.charFormat();

    if(ui->underlineButton->isChecked())
        textFormat.setFontUnderline(true);
    else
        textFormat.setFontUnderline(false);

    selection.setCharFormat(textFormat);
    ui->noteText->setTextCursor(selection);

    ui->noteText->setFocus();
}

void Note::isUnderline()
{
    QTextCursor selection = ui->noteText->textCursor();
    selection.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    QTextCharFormat textFormat = selection.charFormat();

    if(textFormat.font().underline())
        ui->underlineButton->setChecked(true);
    else
        ui->underlineButton->setChecked(false);
}

void Note::listButton_clicked()
{
    QTextCursor cursor = ui->noteText->textCursor();

    if(ui->listButton->isChecked())
    {
        cursor.createList(QTextListFormat::ListDisc);
    }
    else
    {
        QTextList* crrList = cursor.currentList();
        crrList->remove(cursor.block());

        QTextBlock block = cursor.block();
        QTextBlockFormat blockFormat = block.blockFormat();
        blockFormat.setIndent(0);

        cursor.setBlockFormat(blockFormat);
    }
    ui->noteText->setFocus();
}

void Note::isList()
{
    QTextCursor cursor = ui->noteText->textCursor();

    if(cursor.currentList() == nullptr)
        ui->listButton->setChecked(false);
    else
        ui->listButton->setChecked(true);
}

void Note::removeEmptyItem()
{
    if (ui->noteText->document()->isEmpty())
        return;

    if(ui->noteText->document()->lineCount() <= 1)
        return;

    QTextCursor cursor = ui->noteText->textCursor();

    QTextBlock crrBlock = cursor.block();
    QTextBlock prvBlock = crrBlock.previous();

    if(prvBlock.text().isEmpty())
    {
        QTextList* crrTextList = crrBlock.textList();
        QTextList* prvTextList = prvBlock.textList();

        if (crrTextList == nullptr || prvTextList == nullptr)
            return;

        crrTextList->remove(crrBlock);
        prvTextList->remove(prvBlock);

        QTextBlockFormat format = crrBlock.blockFormat();
        format.setIndent(0);
        cursor.setBlockFormat(format);
    }
}

void Note::textChanged()
{
    emit sendText(getNoteText()->document()->toHtml());
}
///////////////////////////////////////////////////////SLOTS
