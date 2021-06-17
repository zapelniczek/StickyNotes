#include "notessubwindow.h"
#include <QAbstractTextDocumentLayout>
#include <QMessageBox>

NotesSubWindow::NotesSubWindow(QWidget* parent,  Qt::WindowFlags flags) :
    QMdiSubWindow(parent, flags), visible_lines(min_line_visible)
{

}

void NotesSubWindow::add_visibile_lines(const int a)
{
    visible_lines += a;
}

void NotesSubWindow::setNote(Note* sourceNote)
{
    note = sourceNote;
    setUpDate(note->getDate());
    setUpButtons();
    setUpButtonCategory(note->getCategory());

    connect(note, &Note::themeColorChanged,
            this, &NotesSubWindow::updateWindowColor);

    connect(note, &Note::sendText,
            this, &NotesSubWindow::setText);

    connect(note, &Note::categoryChanged,
            this, &NotesSubWindow::setUpButtonCategory);

    connect(this, &NotesSubWindow::categoryChanged,
            note, &Note::setCategory);
}

void NotesSubWindow::setBlockHeight(const int a)
{
    block_height = a;
}

int NotesSubWindow::getBlockHeight()
{
    return block_height;
}

int NotesSubWindow::getMinVisLines() const
{
    return min_line_visible;
}

int NotesSubWindow::getMaxVisLines() const
{
    return max_line_visible;
}

void NotesSubWindow::setSafeDeletion (bool ask)
{
    safeDelete = ask;
}

void NotesSubWindow::setUpButtons()
{
    menuButton = new QToolButton(this);
    menuButton->setIcon(QIcon(QStringLiteral(":/nSubWindow/Button/images/more.png")));
    menuButton->setFixedSize(30, 30);

    menuButton->setStyleSheet(QStringLiteral("QToolButton {border: 0px;}"
    "QToolButton::menu-indicator{image:none;}"));

    menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    menuButton->setGeometry(width() - menuButton->width() - 1,  1,
                            menuButton->width(), menuButton->height());

    menuButton->setToolTip(tr("Menu"));
    menuButton->setVisible(false);

    createActions();

    menu = new QMenu(menuButton);

    QFont& font = const_cast<QFont&>(menu->font());
    font.setPixelSize(18);
    menu->setFont(font);

    menu->setStyleSheet(QStringLiteral("QMenu { border:none; }"
    "QMenu::item:selected {  background: rgba(100, 100, 100, 150);}"));

    menu->addAction(closeAction);
    menu->addAction(deleteNote);
    menuButton->setMenu(menu);

    connect(menuButton, &QToolButton::clicked,
            menuButton, &QToolButton::showMenu);

    setUpCategoryButton();
}

void NotesSubWindow::createActions()
{
    openNote = new QAction(QIcon(QStringLiteral(":/nSubWindow/Button/images/view.png")), tr("Open Note"), this);
    openNote->setToolTip(tr("Open note's window"));

    connect(openNote, &QAction::triggered,
            this, &NotesSubWindow::showNote);

    closeAction = new QAction(QIcon(QStringLiteral(":/nSubWindow/Button/images/hide.png")), tr("Close Note"), this);
    closeAction->setToolTip(tr("Close selected window"));

    connect(closeAction, &QAction::triggered,
            this, &NotesSubWindow::closeNote);

    connect(note, &Note::noteHidden,
            this, &NotesSubWindow::updateToolBarMenu);

    deleteNote = new QAction(QIcon(QStringLiteral(":/nSubWindow/Button/images/delete.png")), tr("Delete Note"), this);
    deleteNote->setToolTip(tr("Delete note"));

    connect(deleteNote, &QAction::triggered,
            this, &NotesSubWindow::prepareToDelateNote);
}

void NotesSubWindow::setUpCategoryButton()
{
    categoryButton = new QPushButton(this);
    categoryButton->setFixedSize(QSize(30, 30));
    categoryButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    categoryButton->setStyleSheet(QStringLiteral("QPushButton {border: 0px;}"));
    categoryButton->setGeometry(width() - (menuButton->width() + categoryButton->width()) - 4,  1,
                                categoryButton->width(), categoryButton->height());
    categoryButton->show();
    categoryButton->setVisible(false);

    connect(categoryButton, &QPushButton::clicked,
            this, &NotesSubWindow::categoryButtonClicked);
}

void NotesSubWindow::setUpDate(const QDate& date_)
{
    date = new QDateEdit(this);
    date->setDisabled(true);
    date->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    date->setButtonSymbols(QAbstractSpinBox::NoButtons);
    date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    date->setDate(date_);


    QFont font = date->font();
    font.setPointSize(10);
    date->setFont(font);
    date->setStyleSheet(QStringLiteral("QDateEdit\n{\nbackground:transparent;\n"
                        "border:none;\nborder-radius:10px;\n}"));
    date->setToolTip(tr("Date of note creation"));
    date->show();
}

/////////////////////////////////////////////////////////////////EVENTS
void NotesSubWindow::mouseDoubleClickEvent(QMouseEvent* mouseEvent)
{
    if(!note->isVisible())
        note->show();

    note->activateWindow();
    note->getNoteText()->setFocus();

    QMdiSubWindow::mouseDoubleClickEvent(mouseEvent);
}

void NotesSubWindow::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    mouseEvent->accept();
}

void NotesSubWindow::enterEvent(QEvent *event)
{
    menuButton->show();
    categoryButton->show();

    QMdiSubWindow::enterEvent(event);
}

void NotesSubWindow::leaveEvent(QEvent *leaveEvent)
{
    menuButton->hide();
    categoryButton->hide();

    QMdiSubWindow::leaveEvent(leaveEvent);
}

void NotesSubWindow::mousePressEvent(QMouseEvent *mouseEvent)
{
    mouseEvent->ignore();
}

void NotesSubWindow::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    mouseEvent->ignore();
}
/////////////////////////////////////////////////////////////////EVENTS
//**************************************************************SLOTS
void NotesSubWindow::showNote()
{
    if (!note->isVisible())
    {
        note->show();
        note->getNoteText()->setFocus();
    }
}

void NotesSubWindow::closeNote()
{
    if (note->isVisible())
        note->close();
}

void NotesSubWindow::prepareToDelateNote()
{
    if(safeDelete)
    {
        QMessageBox* question = new QMessageBox(tr("Warning"),
                                                tr("Are you sure you want delete the note?\n"
                                       "You won't be able to restore it."), QMessageBox::Question,
                                                QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::NoButton);

        connect(question, &QMessageBox::finished,
                question, &QMessageBox::deleteLater);

        if (question->exec() == QMessageBox::Ok)
            emit note->deleteNote(note);
    }
    else
        emit note->deleteNote(note);
}

void NotesSubWindow::updateToolBarMenu(bool show)
{
    if(show)
    {
        menu->removeAction(closeAction);
        menu->insertAction(deleteNote, openNote);
    }
    else
    {
        menu->removeAction(openNote);
        menu->insertAction(deleteNote, closeAction);
    }
}

void NotesSubWindow::categoryButtonClicked()
{
    typeDialog = new NoteTypeDialog(category);
    typeDialog->open();

    connect(typeDialog, &QDialog::finished,
            this, &NotesSubWindow::recieveCategory);
}

void NotesSubWindow::setUpButtonCategory(const NoteCategory& category_)
{
    if(category == category_)
        return;

    category = category_;
    categoryButton->setIcon(getIconCategory(category_));
    categoryButton->setToolTip(getTooltipCategory(category_));
    emit categoryChanged(category);
}

void NotesSubWindow::recieveCategory()
{
    if (typeDialog->result() == QDialog::Accepted)
    {
        NoteCategory new_category = typeDialog->getCategory();
        setUpButtonCategory(new_category);
    }

    typeDialog->close();
    typeDialog->deleteLater();
}

void NotesSubWindow::setText(QString text)
{    
    QTextEdit* m_widget = static_cast<QTextEdit*>(widget());
    text.insert(0, "<br>");
    m_widget->setText(text);

    int real_lines = m_widget->document()->documentLayout()->documentSize().height()/
            getBlockHeight() - 1;

    if (visible_lines != real_lines)
    {
        if(visible_lines < max_line_visible && real_lines > visible_lines )
        {
            int line_grow;
            (real_lines <= max_line_visible) ? line_grow = real_lines - visible_lines : line_grow = max_line_visible - visible_lines;

            visible_lines += line_grow;
            emit expand(line_grow, this);
        }

        else if (real_lines < visible_lines && visible_lines > min_line_visible)
        {
            int line_shrink;
            (real_lines < 2) ? line_shrink = visible_lines - min_line_visible : line_shrink = visible_lines - real_lines;

            visible_lines -= line_shrink;
            emit shrink(line_shrink, this);
        }
    }

    emit sendPriorityWindowRequest(note);
}

void NotesSubWindow::updateWindowColor()
{
    QTextEdit* textNote = note->getNoteText();
    QColor baseColor = textNote->palette().base().color();
    QColor textColor = textNote->palette().text().color();

    setStyleSheet(QStringLiteral("QWidget{ background-color:")+ baseColor.name(QColor::HexRgb)+ QStringLiteral(";\n"
                                 "color: ") + textColor.name(QColor::HexRgb)+ QStringLiteral(";\nborder-radius:10;}"));
}
//**************************************************************SLOTS
