#include "stickynotes.h"
#include "ui_stickynotes.h"

#include <QApplication>
#include <QScreen>
#include <QDir>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>
#include <QGraphicsPixmapItem>

StickyNotes::StickyNotes(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StickyNotes)
{
    ui->setupUi(this);
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );
    setAttribute(Qt::WA_TranslucentBackground);

    ui->notesArea->setBackground(QBrush(QColor(255, 255, 255)));

    ui->stackedWidget->setCurrentIndex(0);
    prepareGraphicsView();

    azureStorage = new StorageRestApi(this);

    connect(azureStorage->getNetwork(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    location = SaveLocation::Local;

    connect(ui->newButton, &QPushButton::clicked,
            this, &StickyNotes::newNoteButton_clicked);

    connect(ui->closeButton, &QPushButton::clicked,
            this, &StickyNotes::closeButton_clicked);

    connect(ui->filterButton, &QPushButton::clicked,
            this, &StickyNotes::filterButton_clicked);

    connect(ui->optionsButton, &QPushButton::clicked,
            this, &StickyNotes::optionButtonClicked);

    connect(this, &StickyNotes::changePage,
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(qApp, &QApplication::aboutToQuit,
            this, &StickyNotes::closeButton_clicked);

    create_dialog_connections();
}

StickyNotes::~StickyNotes()
{
    delete creationdialog;
    delete filterdialog;
    delete settings;
    delete ui;
}

void StickyNotes::create_dialog_connections()
{
    creationdialog = new NoteTypeDialog;

    connect(creationdialog, &QDialog::finished,
            this, &StickyNotes::createNote);


    filterdialog = new FilterDialog();

    connect(filterdialog, &QDialog::finished,
            this, &StickyNotes::filterNotes);


    settings = new SettingsDialog();

    connect(settings, &SettingsDialog::moved,
            this, &StickyNotes::setNewPos);

    connect(settings, &SettingsDialog::requestTerminate,
            this, &StickyNotes::closeButton_clicked);

    connect(settings, &SettingsDialog::themeChanged,
            this, &StickyNotes::changeTheme);

    connect(settings, &SettingsDialog::saveLocationChanged,
            this, &StickyNotes::setSaveLocation);

    connect(settings, &SettingsDialog::safeDeleteChanged,
            this, &StickyNotes::setDeletionSafety);

    connect(settings, &SettingsDialog::creditentalsChanged,
            this, &StickyNotes::updateCreditentals);

}

void StickyNotes::readNotesFromFiles()
{
    QDir openDirectory(QDir::currentPath());
    openDirectory.cd("notes");

    QStringList filters;
    filters << "*.xml";

    for (QString& name : openDirectory.entryList(filters, QDir::Files))
    {
        QFile file(openDirectory.path() + "\\"+ name);
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        int colorID_B = 0;
        QColor colorID_F;
        NoteCategory category_ = NoteCategory::None;
        QString noteText;
        QDate date;

        QXmlStreamReader fileReader(&file);

        while(!fileReader.atEnd())
        {
            fileReader.readNextStartElement();

            if(fileReader.error() == QXmlStreamReader::PrematureEndOfDocumentError)
                break;

            if(fileReader.name() == "Text" && fileReader.tokenType() == QXmlStreamReader::StartElement)
                noteText = fileReader.readElementText();

            else if (fileReader.name() == "Color_B" && fileReader.tokenType() == QXmlStreamReader::StartElement)
                colorID_B = fileReader.readElementText().toUInt();

            else if (fileReader.name() == "Color_F" && fileReader.tokenType() == QXmlStreamReader::StartElement)
                colorID_F = QColor(fileReader.readElementText());

            else if (fileReader.name() == "Category" && fileReader.tokenType() == QXmlStreamReader::StartElement)
                category_ = static_cast<NoteCategory>(fileReader.readElementText().toUInt());

            else if (fileReader.name() == "Date" && fileReader.tokenType() == QXmlStreamReader::StartElement)
                date = QDate::fromString(fileReader.readElementText(), "dd/MM/yyyy");

        }// while

        recreateOldNote(noteText, colorID_B, colorID_F, category_, date);
    }
}

void StickyNotes::saveNoteToFile()
{
    QDir saveDirectory(QDir::currentPath());
    saveDirectory.cd("notes");

    for (Note* note : allNotes)
    {
        if (note->getNoteText()->document()->isEmpty())
            continue;

        QFile file(saveDirectory.path() + "\\" + note->getFileName());
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);

        QXmlStreamWriter stream(&file);
        stream.writeStartDocument();

        stream.writeStartElement("Note");
        stream.writeStartElement("Text");
        stream.writeCharacters(note->getNoteText()->toHtml());
        stream.writeEndElement();//Text;

        stream.writeStartElement("Color_B");
        stream.writeCharacters(QString::number(note->getColorTheme())); // change format later
        stream.writeEndElement(); //Background

        stream.writeStartElement("Color_F");
        stream.writeCharacters(note->getTextColor().name());
        stream.writeEndElement(); //Text color

        stream.writeStartElement("Category");
        stream.writeCharacters(QString::number(static_cast<int>(note->getCategory())));
        stream.writeEndElement(); //Category

        stream.writeStartElement("Date");
        stream.writeCharacters(note->getDate().toString("dd/MM/yyyy"));
        stream.writeEndElement(); //Date

        stream.writeEndElement(); //Note
        stream.writeEndDocument();
    }
}

void StickyNotes::downloadNotes()
{
    azureStorage->listFiles();
}

void StickyNotes::uploadFilesFromLocal()
{
    QDir openDirectory(QDir::currentPath());
    openDirectory.cd("notes");

    QStringList filters;
    filters << "*.xml";

    for (const QString& name : openDirectory.entryList(filters, QDir::Files))
    {
        QString filePath = openDirectory.path() + "/" + name;
        azureStorage->uploadFile(filePath, name);
    }
}

void StickyNotes::uploadFilesToCloud()
{
    QByteArray data;
    for (const Note* note : allNotes)
    {
        if (note->getNoteText()->document()->isEmpty())
            continue;

        QXmlStreamWriter stream(&data);
        stream.writeStartDocument();

        stream.writeStartElement("Note");
        stream.writeStartElement("Text");
        stream.writeCharacters(note->getNoteText()->toHtml());
        stream.writeEndElement();//Text;

        stream.writeStartElement("Color_B");
        stream.writeCharacters(QString::number(note->getColorTheme())); // change format later
        stream.writeEndElement(); //Background

        stream.writeStartElement("Color_F");
        stream.writeCharacters(note->getTextColor().name());
        stream.writeEndElement(); //Text color

        stream.writeStartElement("Category");
        stream.writeCharacters(QString::number(static_cast<int>(note->getCategory())));
        stream.writeEndElement(); //Category

        stream.writeStartElement("Date");
        stream.writeCharacters(note->getDate().toString("dd/MM/yyyy"));
        stream.writeEndElement(); //Date

        stream.writeEndElement(); //Note
        stream.writeEndDocument();
        azureStorage->uploadFile(data, note->getFileName());
    }
}

SaveLocation StickyNotes::getSaveLocation()
{
    return location;
}

void StickyNotes::getFileListInCloud(const QByteArray& ba)
{
    QXmlStreamReader reader(ba);

    while (!reader.atEnd())
    {
        reader.readNextStartElement();
        if(reader.name() == "Name")
            cloudNotes.push_back(reader.readElementText());
    }
    downloadNotesFromCloud();
}

void StickyNotes::downloadNotesFromCloud()
{
    int total = cloudNotes.count();
    for (int i = 0; i < total; ++i)
    {
        azureStorage->downloadFile(cloudNotes[i]);
    }

}

void StickyNotes::deleteFromCloud(const QString& fileName)
{
    azureStorage->deleteFile(fileName);
}

void StickyNotes::deleteFromLocal(const QString& fileName)
{
    QDir filePath(QDir::currentPath());
    filePath.cd(QStringLiteral("notes"));
    QFile fileNote(filePath.filePath(fileName));
    fileNote.remove();
}

void StickyNotes::readSettings()
{
    QSettings options(QStringLiteral("XD"), QStringLiteral("StickyNotes"));
    options.beginGroup(QStringLiteral("Settings"));

    settings->setAccName(options.value(QStringLiteral("AccName")).toString());
    settings->setSafetyKey(options.value(QStringLiteral("AccKey")).toString());
    settings->setContainer(options.value(QStringLiteral("Container")).toString());
    settings->setDeleteWarning(options.value(QStringLiteral("AskDelete")).toBool());
    settings->setTheme(static_cast<Theme>(options.value(QStringLiteral("Theme")).toInt()));
    settings->setLocationSave(static_cast<SaveLocation>(options.value(QStringLiteral("Location")).toInt()));

    options.endGroup();
}

void StickyNotes::showCloudNote(const QByteArray& ba)
{
    int colorID_B = 0;
    QColor colorID_F;
    NoteCategory category_ = NoteCategory::None;
    QString noteText;
    QDate date;

    QXmlStreamReader fileReader(ba);

    while(!fileReader.atEnd())
    {
        fileReader.readNextStartElement();

        if(fileReader.error() == QXmlStreamReader::PrematureEndOfDocumentError)
            break;

        if(fileReader.name() == "Text" && fileReader.tokenType() == QXmlStreamReader::StartElement)
            noteText = fileReader.readElementText();

        else if (fileReader.name() == "Color_B" && fileReader.tokenType() == QXmlStreamReader::StartElement)
            colorID_B = fileReader.readElementText().toUInt();

        else if (fileReader.name() == "Color_F" && fileReader.tokenType() == QXmlStreamReader::StartElement)
            colorID_F = QColor(fileReader.readElementText());

        else if (fileReader.name() == "Category" && fileReader.tokenType() == QXmlStreamReader::StartElement)
            category_ = static_cast<NoteCategory>(fileReader.readElementText().toUInt());

        else if (fileReader.name() == "Date" && fileReader.tokenType() == QXmlStreamReader::StartElement)
            date = QDate::fromString(fileReader.readElementText(), "dd/MM/yyyy");

    }// while

    recreateOldNote(noteText, colorID_B, colorID_F, category_, date);
}

void StickyNotes::recreateOldNote(QString text, int colorID, QColor color_F,
                                NoteCategory category_, const QDate& date)
{
    if (allNotes.size() == 0)
        emit changePage(1);

    Note* note = new Note(text, colorID, color_F, category_, date);
    connect(note, SIGNAL(deleteNote(Note*)),
            this, SLOT(deleteNote(Note*)));

    allNotes.push_back(note);

    QString name = createFileName();
    note->setFileName(name);

    QTextEdit* note_widget = note->getNoteText();
    QTextCursor cursor = note_widget->textCursor();
    cursor.movePosition(QTextCursor::End);
    note_widget->setTextCursor(cursor);

    note->show();
    setNoteGeometry();

    addNoteToMdiArea(note);

    if(!allNotes.isEmpty())
    {
        allNotes[0]->activateWindow();
        allNotes[0]->getNoteText()->setFocus();
    }
}

QString StickyNotes::createFileName()
{
    QString name = "nnn";
    int number = allNotes.count();
    name += QString::number(number) + QStringLiteral(".xml");
    return name;
}

void StickyNotes::setNoteGeometry()
{
    if(allNotes.count() == 0)
        qFatal("Empty vector!");

    if(!previousNotePosition.isNull())
    {
        Note* note = allNotes.last();
        note->setGeometry(previousNotePosition.x(), previousNotePosition.y(), note->width(), note->height());
        previousNotePosition.setX(0);
        previousNotePosition.setY(0);
        return;
    }

    if (allNotes.count() % 2)
        setOnRight();

    else
        setOnLeft();

}

void StickyNotes::setOnRight()
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int screenWidth = screenGeometry.width();

    Note* note = allNotes.last();

    if(allNotes.count() > 2)
        noteOffSetY *= -80;

    int notePosX = x() + width() + 10;

    if(notePosX + note->width() < screenWidth) // check if it will fit on the screen
        note->setGeometry(notePosX, y() + noteOffSetY,
                          note->width(), note->height());
    else
    {
        notePosX = screenWidth - 10 - note->width();
        note->setGeometry(notePosX, y() + noteOffSetY,
                          note->width(), note->height());
    }

    if(allNotes.count() > 2)
        noteOffSetY /= 80;

}

void StickyNotes::setOnLeft()
{
    Note* note = allNotes.last();

    if(allNotes.count() > 2)
        noteOffSetY *= 80;


    int notePosX = x() - 10 - note->width();

    if(notePosX > 0) // check if it will fit on the screen
        note->setGeometry(notePosX, y() + noteOffSetY,
                          note->width(), note->height());
    else
    {
        notePosX = 10;
        note->setGeometry(notePosX, y() + noteOffSetY,
                          note->width(), note->height());
    }

    if(allNotes.count() > 2)
        noteOffSetY /= 80;

}

void StickyNotes::addNoteToMdiArea(Note* note)
{
    QTextEdit* m_widget = new QTextEdit();
    m_widget->setReadOnly(true);
    m_widget->setDisabled(true);
    m_widget->setText(note->getNoteText()->toHtml());
    m_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    add_NL(m_widget);
    makeSyntaxSearchReady(m_widget);

    NotesSubWindow* noteWindow = new NotesSubWindow(ui->notesArea);
    noteWindow->setWidget(m_widget);
    noteWindow->setWindowFlag(Qt::FramelessWindowHint);
    noteWindow->setGeometry(0, 0, ui->notesArea->width() - 25, 100);
    noteWindow->setMinimumHeight(100);
    noteWindow->setMaximumHeight(136);
    noteWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    noteWindow->setNote(note);
    noteWindow->updateWindowColor();

    QMdiSubWindow* subWindow =  ui->notesArea->addSubWindow(noteWindow);
    hashSubWindow(note, subWindow);

    noteWindow->show();

    int block_height =  m_widget->document()->lastBlock().layout()->boundingRect().height()/
            m_widget->document()->lastBlock().layout()->lineCount();

    noteWindow->setBlockHeight(block_height);

    coordinateSubWindow(noteWindow);

    connect(noteWindow, &NotesSubWindow::sendPriorityWindowRequest,
            this,  &StickyNotes::rearrangeSubWindows);

    connect(noteWindow, &NotesSubWindow::expand,
            this, &StickyNotes::subwindowGrows);

    connect(noteWindow, &NotesSubWindow::shrink,
            this, &StickyNotes::subwindowShrink);

}

void StickyNotes::add_NL(QTextEdit* note)
{
    QString text = note->document()->toHtml();
    text.insert(0, "<br>");
    note->setText(text);
}

void StickyNotes::hashSubWindow(Note* note, QMdiSubWindow* subWindow)
{
    hashNoteWindow.insert(note, subWindow);
}

void StickyNotes::coordinateSubWindow(NotesSubWindow* subWindow)
{
    int new_height = getSubWinHeight(subWindow);

    if(allNotes.count() > 1)
    {
        Note* prev_note = allNotes[allNotes.count()-2];
        QMdiSubWindow * prevWindow = hashNoteWindow[prev_note];
        QPoint prevWindowPoint = prevWindow->mapTo(ui->notesArea, QPoint(0, 0));

        subWindow->setGeometry(prevWindowPoint.x(), prevWindowPoint.y() + prevWindow->height() + 10,
                               ui->notesArea->width() - 25, new_height);
    }
    else
    {
        QPoint basePoint = subWindow->mapTo(ui->notesArea, QPoint(0, 0));

        subWindow->setGeometry(basePoint.x() + 10, basePoint.y(),
                               ui->notesArea->width() - 25, new_height);
    }

}

int StickyNotes::getSubWinHeight(NotesSubWindow* subWindow)
{
    QTextEdit* m_widget = static_cast<QTextEdit*>(subWindow->widget());

    int windows_line = m_widget->document()->documentLayout()->documentSize().height()/
            subWindow->getBlockHeight() - 1;

    int bns_height = 0;

    if(windows_line > 2)
    {
        int expand_vis_lines;

        windows_line > 4 ? expand_vis_lines = 2 : expand_vis_lines =  windows_line - 2;
        subWindow->add_visibile_lines(expand_vis_lines);

        int expand = (subWindow->maximumHeight() - subWindow->minimumHeight()) /
                (subWindow->getMaxVisLines() - subWindow->getMinVisLines());

        bns_height = expand * expand_vis_lines;
    }

    return subWindow->height() + bns_height;
}

void StickyNotes::updateMDI_Theme(const QColor& color)
{
    ui->notesArea->setBackground(color);

    QString newStyleSheet = ui->notesArea->styleSheet();
    QString bColor("background:");
    int index = newStyleSheet.indexOf(bColor);
    if(index != - 1)
    {
        newStyleSheet.remove(index, bColor.size() + 8);
        newStyleSheet.insert(index, bColor + color.name(QColor::HexRgb) + ";");
        ui->notesArea->setStyleSheet(newStyleSheet);
    }
}

void StickyNotes::updateSearch_Theme(const QColor& textColor)
{
    ui->searcher->setStyleSheet(QStringLiteral("QLineEdit{border: 1px solid grey;") +
                                QStringLiteral("color:") + textColor.name(QColor::HexRgb)+ QStringLiteral(";}"));
}

void StickyNotes::updateBase_Theme(const QColor& base)
{
    QString newStyleSheet = styleSheet();
    QString mainColor("background-color:");
    int index = newStyleSheet.indexOf(mainColor);
    if(index != - 1)
    {
        newStyleSheet.remove(index, mainColor.size() + 8);
        newStyleSheet.insert(index, mainColor + base.name(QColor::HexRgb) + ";");
        setStyleSheet(newStyleSheet);
    }
}

void StickyNotes::updateWindowsGeometry(int startPos, QPoint base_point)
{
    if(startPos >= allNotes.size())
        return;

    updateOffset(startPos);

    move_animations = new QParallelAnimationGroup(this);

    ui->notesArea->verticalScrollBar()->blockSignals(true);

    connect(move_animations, &QParallelAnimationGroup::finished,
            this, &StickyNotes::release_MDI_Scrollbar);

    for (int pos = startPos; pos < hashNoteWindow.size(); ++pos)
    {
        QMdiSubWindow* window = hashNoteWindow[allNotes[pos]];

        QPropertyAnimation * window_animation = new QPropertyAnimation(window, "pos");
        window_animation->setDuration(150);
        window_animation->setStartValue(window->pos());
        window_animation->setEndValue(QPoint(base_point.x(), base_point.y()));

        move_animations->addAnimation(window_animation);
        base_point = QPoint(window->x(), base_point.y() + window->height() + 10);
    }

    move_animations->start(QAbstractAnimation::DeleteWhenStopped);
}

void StickyNotes::updateOffset(const int startPos)
{
    if(startPos % 2)
        noteOffSetY *= -1;

    else
        noteOffSetY *= 1;
}

void StickyNotes::deleteSubWindow(Note* note)
{
   previousNotePosition = note->pos();

   ui->notesArea->removeSubWindow(hashNoteWindow[note]);
   hashNoteWindow[note]->close();
   hashNoteWindow.remove(note);

   int index = allNotes.indexOf(note);
   phraseMatchVector.remove(index);
}

void StickyNotes::filterByCategory (const NoteCategory& filter)
{
    if(filter == NoteCategory::All)
    {
        for (Note* note : allNotes)
            hashNoteWindow.value(note)->setVisible(true);

        return;
    }

    auto it_end =  std::partition(allNotes.begin(), allNotes.end(),
                                  [&filter] (const Note* note) {return note->getCategory() == filter;});

    QList<QMdiSubWindow*> windows = ui->notesArea->subWindowList();

    std::for_each(allNotes.begin(), it_end,
                  [this] (Note* note) {hashNoteWindow.value(note)->setVisible(true);});
    std::for_each(it_end, allNotes.end(),
                  [this] (Note* note) {hashNoteWindow.value(note)->setVisible(false);});

    std::for_each(windows.begin(), windows.end(),
                  [this](QMdiSubWindow* noteWindow){noteWindow->setGeometry(0, 0, ui->notesArea->width() - 25, 100);});

    setWindowAfterFiltering();
}

void StickyNotes::filterByDate (const DateFilter& filter)
{
    if (filter == DateFilter::Ascending)
    {
        std::sort(allNotes.begin(), allNotes.end(),
                  [] (const Note* lhs, const Note* rhs) {return lhs->getDate() < rhs->getDate();});
    }
    else
    {
        std::sort(allNotes.begin(), allNotes.end(),
                  [] (const Note* lhs, const Note* rhs) {return lhs->getDate() > rhs->getDate();});
    }

    QList<QMdiSubWindow*> windows = ui->notesArea->subWindowList();

    std::for_each(windows.begin(), windows.end(),
                  [this](QMdiSubWindow* noteWindow){noteWindow->setGeometry(0, 0, ui->notesArea->width() - 25, 100);});

    setWindowAfterFiltering();
}

void StickyNotes::setWindowAfterFiltering()
{
    for (int i = 0; i < allNotes.size(); ++i )
    {
        NotesSubWindow* subWindow = static_cast<NotesSubWindow*>(hashNoteWindow.value(allNotes[i]));
        QPoint mdiAreaNoteCoordiante  = subWindow->mapTo(ui->notesArea, QPoint(0, 0));

        int new_height = getSubWinHeight(subWindow);

        if(i > 0)
        {
            Note* prev_note = allNotes[i-1];
            QMdiSubWindow * prevWindow = hashNoteWindow[prev_note];
            QPoint mdiAreaPrevNoteCoordiante  = prevWindow->mapTo(ui->notesArea, QPoint(0, 0));

            subWindow->setGeometry(mdiAreaPrevNoteCoordiante.x(), mdiAreaPrevNoteCoordiante.y()+ prevWindow->height() + 10,
                                   ui->notesArea->width() - 25, new_height);
        }
        else
            subWindow->setGeometry(mdiAreaNoteCoordiante.x() + 10, mdiAreaNoteCoordiante.y(),
                                   ui->notesArea->width() - 25, new_height);
    }
}

void StickyNotes::updateNotesNames(int index)
{
    for (int i = index; i < allNotes.size() ;++i )
    {
        QString fileName = "nnn";
        fileName += QString::number(i+1) + QStringLiteral(".xml");

        Note* note = allNotes[i];
        note->setFileName(fileName);
    }
    trashFiles.push_back(allNotes.size() + 1);
}

void StickyNotes::removeTrashFile()
{
    while(!trashFiles.isEmpty())
    {
        switch(location)
        {
        case SaveLocation::Cloud:
        {
            QString fileName = "nnn" +QString::number(trashFiles.takeFirst()) + QStringLiteral(".xml");
            azureStorage->deleteFile(fileName);
            break;
        }
        default:
        {
            QDir file(QDir::currentPath());
            file.cd(QStringLiteral("notes"));
            QString fileName = "nnn" +QString::number(trashFiles.takeFirst()) + QStringLiteral(".xml");
            QFile fileNote(file.filePath(fileName));
            fileNote.remove();
            break;
        }
        }
    }
}

void StickyNotes::makeSyntaxSearchReady(const QTextEdit* text)
{
    phraseMatchVector.push_back(new PhraseHighlighter(text->document()));

    connect(ui->searcher, &QLineEdit::textEdited,
            phraseMatchVector.last(), &PhraseHighlighter::setPhraseToLook);

    connect(ui->searcher, &QLineEdit::textEdited,
            this, &StickyNotes::searchPhrase);
}

void StickyNotes::prepareGraphicsView()
{
    QGraphicsScene* scene = new QGraphicsScene(ui->entryLabel);
    QRect rect(0, 0, ui->entryLabel->width(), 300);
    scene->setSceneRect(rect);

    const QPixmap icon(QStringLiteral(":/StickyNotes/Entry/images/entry.png"), "PNG");
    QGraphicsPixmapItem * item_icon = scene->addPixmap(icon);

    int x = ui->entryLabel->width()/2 - (item_icon->boundingRect().width()/2);
    item_icon->setPos(x, 0);

    QFont font = qApp->font();
    font.setPointSize(14);

    QGraphicsTextItem* text_item_1 = scene->addText(QObject::tr("In order to add new note,\n"), font);
    x = ui->entryLabel->width()/2 - (text_item_1->boundingRect().width()/2);
    text_item_1->setPos(x, icon.height() + 10);

    QGraphicsTextItem* text_item_2 = scene->addText(QObject::tr("click plus button at top left corner."), font);
    x = ui->entryLabel->width()/2 - (text_item_2->boundingRect().width()/2);
    text_item_2->setPos(x, icon.height() +  font.pointSize()+ 25);

    ui->entryLabel->setScene(scene);

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void StickyNotes::mousePressEvent(QMouseEvent *event)
{
    QRect frameRect = ui->button_layout->contentsRect();
    isMouseHoverFrame = frameRect.contains(event->pos());

    isMousePressed = true;

    startPos = event->pos();

    QMainWindow::mousePressEvent(event);
}

void StickyNotes::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    QMainWindow::mouseReleaseEvent(event);
}

void StickyNotes::mouseMoveEvent(QMouseEvent *event)
{
    if(isMousePressed  && isMouseHoverFrame )
    {
        QPoint deltaPos = event->pos() - startPos;
        move(pos() + deltaPos);
    }
    QMainWindow::mouseMoveEvent(event);
}

void StickyNotes::closeEvent(QCloseEvent* event)
{
    if(!trashFiles.isEmpty())
        removeTrashFile();

    switch (location)
    {
    case SaveLocation::Local:
        saveNoteToFile();
        break;

    case SaveLocation::Cloud:
        uploadFilesToCloud();
        break;

    case SaveLocation::LocalAndCloud:
        saveNoteToFile();
        uploadFilesFromLocal();
        break;
    }

    QMainWindow::closeEvent(event);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
///////////////////////////////////////////////////////////SLOTS
void StickyNotes::newNoteButton_clicked()
{
    creationdialog->open();
}

void StickyNotes::createNote()
{
    if(creationdialog->result() == QDialog::Accepted)
    {
        if (allNotes.size() == 0)
            emit changePage(1);

        NoteCategory category = creationdialog->getCategory();

        Note* note = new Note;
        note->setCategory(category);

        connect(note, SIGNAL(deleteNote(Note*)),
                this, SLOT(deleteNote(Note*)));

        allNotes.push_back(note);

        QString name = createFileName();
        note->setFileName(name);

        setNoteGeometry();

        note->show();
        addNoteToMdiArea(note);
        rearrangeSubWindows(note);
    }
}

void StickyNotes::closeButton_clicked()
{
    qApp->closeAllWindows();
}

void StickyNotes::filterButton_clicked()
{
    filterdialog->open();
}

void StickyNotes::filterNotes()
{
    if(filterdialog->result() == QDialog::Accepted)
    {
        int filter_type = filterdialog->getFilterResult();

        if (filter_type < static_cast<int>(DateFilter::Ascending))
        {
            NoteCategory filter = static_cast<NoteCategory>(filter_type);
            filterByCategory(filter);
        }
        else
        {
            DateFilter filter = static_cast<DateFilter>(filter_type);
            filterByDate(filter);
        }
    }
}

void StickyNotes::optionButtonClicked()
{
    QPoint point = mapToGlobal(QPoint(0, 0));
    settings->setGeometry(QRect(point, QSize(width(), height())));
    settings->show();

    QSettings options("XD", "StickyNotes");
    options.beginGroup("Settings");
    settings->setDeleteWarning(options.value("AskDelete").toBool());
    options.endGroup();
}

void StickyNotes::setNewPos(QPoint pos)
{
    move(pos);
}

void StickyNotes::changeTheme(Theme theme_)
{
    theme = theme_;
    QColor base, textColor;

    if(theme == Theme::Dark)
    {
        base.setRgb(61, 61, 61);
        textColor.setRgb(255, 255, 255);
    }
    else
    {
        base.setRgb(255, 255, 255);
        textColor.setRgb(0, 0, 0);
    }

    updateBase_Theme(base);
    updateMDI_Theme(base);
    updateSearch_Theme(textColor);
}

void StickyNotes::setSaveLocation(SaveLocation location_)
{
    location = location_;
}

void StickyNotes::setDeletionSafety(bool ask)
{
    NotesSubWindow::setSafeDeletion(ask);
}

void StickyNotes::updateCreditentals (std::tuple<QString, QString, QString> creditentals)
{
    auto [name, key, container] = creditentals;
    azureStorage->updateCredentials(name, key, container);
}

void StickyNotes::rearrangeSubWindows(Note* primalNote)
{
    using std::swap;

    if(allNotes.indexOf(primalNote) == 0)
            return;

    ui->notesArea->verticalScrollBar()->setSliderPosition(0);
    ui->notesArea->verticalScrollBar()->blockSignals(true);

    QMdiSubWindow* primal_subWindow = hashNoteWindow[primalNote];
    primal_subWindow->hide();
    hidden_window = primal_subWindow;

    int indexPrime = allNotes.indexOf(primalNote);
    QPoint base_point = primal_subWindow->pos();

    move_animations = new QParallelAnimationGroup(this);

    for (auto i = indexPrime - 1; i >= 0; --i)
    {
        QMdiSubWindow* current_window = hashNoteWindow[allNotes[i]];

        QPropertyAnimation* window_animation = new QPropertyAnimation(current_window, "pos");
        window_animation->setDuration(150);
        window_animation->setStartValue(current_window->pos());
        window_animation->setEndValue(QPoint(base_point.x(),
                                             base_point.y() + (primal_subWindow->height() - current_window->height())));

        move_animations->addAnimation(window_animation);

        base_point = current_window->pos();

        allNotes[i]->swapFileNames(allNotes[i+1]);
        allNotes.swapItemsAt(i, i+1);
     }

     move_animations->start(QAbstractAnimation::DeleteWhenStopped);


     primal_subWindow->move(primal_subWindow->x(), 0);
     primalNote->getNoteText()->setFocus();

     connect(move_animations, &QParallelAnimationGroup::finished,
             this, &StickyNotes::release_MDI_Scrollbar);

     connect(move_animations, &QParallelAnimationGroup::finished,
            this, &StickyNotes::animationDone);
}

void StickyNotes::release_MDI_Scrollbar()
{
    ui->notesArea->verticalScrollBar()->blockSignals(false);
}

void StickyNotes::animationDone()
{
    disconnect(move_animations, &QParallelAnimationGroup::finished,
               hidden_window, &QMdiSubWindow::show);

    hidden_window->show();
    hidden_window = nullptr;
}

void StickyNotes::subwindowGrows(int bnsHeight, NotesSubWindow* start_window)
{
    int start_pos = allNotes.indexOf(hashNoteWindow.key(start_window));

    int expand = (start_window->maximumHeight() - start_window->minimumHeight()) /
                 (start_window->getMaxVisLines() - start_window->getMinVisLines());

    bnsHeight *= expand;
    start_window->resize(start_window->width(), start_window->height() + bnsHeight);

    for (int i = start_pos + 1; i < allNotes.size(); ++i)
    {
        QMdiSubWindow* window = hashNoteWindow[allNotes[i]];
        window->move(window->x(), window->y()+ bnsHeight);
    }
}

void StickyNotes::subwindowShrink(int mnsHeight, NotesSubWindow* start_window)
{
    int start_pos = allNotes.indexOf(hashNoteWindow.key(start_window));

    int expand = (start_window->maximumHeight() - start_window->minimumHeight()) /
                 (start_window->getMaxVisLines() - start_window->getMinVisLines());

    mnsHeight *= expand;
    start_window->resize(start_window->width(), start_window->height() - mnsHeight);

    for (int i = start_pos + 1; i < allNotes.size(); ++i)
    {
        QMdiSubWindow* window = hashNoteWindow[allNotes[i]];
        window->move(window->x(), window->y() - mnsHeight);
    }
}

void StickyNotes::deleteNote(Note* note)
{
    if(ui->notesArea->verticalScrollBar()->isVisible())
       ui->notesArea->verticalScrollBar()->setSliderPosition(0);

    QPoint subwindow_pos = hashNoteWindow[note]->pos();
    deleteSubWindow(note);

    switch(location)
    {
    case SaveLocation::Local:
        deleteFromLocal(note->getFileName());
        break;

    case SaveLocation::Cloud:
        deleteFromCloud(note->getFileName());
        break;

    case SaveLocation::LocalAndCloud:
        deleteFromLocal(note->getFileName());
        deleteFromCloud(note->getFileName());
        break;
    }

    int index = allNotes.indexOf(note);
    allNotes.remove(index);

    updateNotesNames(index);

    if(hashNoteWindow.count() != 0)
        updateWindowsGeometry(index, subwindow_pos);

    delete note;

    if(allNotes.count() == 0)
        emit changePage(0);
}

void StickyNotes::searchPhrase()
{
    if (allNotes.count() == 0)
        return;

    for (PhraseHighlighter* syntaxSearcher : phraseMatchVector)
        syntaxSearcher->rehighlight();

}

void StickyNotes::replyFinished(QNetworkReply* reply)
{
    StorageRestApi::RequestType typeOfRequest = azureStorage->getAzureRequestType();

    QByteArray ba;

    switch (typeOfRequest)
    {
    case StorageRestApi::RequestType::GetList:
        ba = reply->readAll();
        getFileListInCloud(ba);
        break;

    case StorageRestApi::RequestType::Download:
        ba = reply->readAll();
        showCloudNote(ba);
        break;

    case StorageRestApi::RequestType::Upload:
        qApp->quit();
        break;

    default:
        break;
    }

    reply->deleteLater();
}
