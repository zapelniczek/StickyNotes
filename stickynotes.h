#ifndef STICKYNOTES_H
#define STICKYNOTES_H

#include "storagerestapi.h"
#include "note.h"
#include "phrasehighlighter.h"
#include "notessubwindow.h"
#include "filterdialog.h"
#include "settingsdialog.h"
#include "notetypedialog.h"

#include <QMainWindow>
#include <QParallelAnimationGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class StickyNotes; }
QT_END_NAMESPACE

class StickyNotes : public QMainWindow
        // revision only left storage rest api
        //upload to github
{
    Q_OBJECT

public:
    StickyNotes(QWidget *parent = nullptr);

    ~StickyNotes();

    void readNotesFromFiles();

    void downloadNotes();

    void uploadFilesFromLocal();

    void uploadFilesToCloud();

    static SaveLocation getSaveLocation();

    void readSettings();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void closeEvent(QCloseEvent* event) override;

signals:
    void changePage(int);

private slots:
    void newNoteButton_clicked();

    void createNote();

    void closeButton_clicked();

    void filterButton_clicked();

    void filterNotes();

    void optionButtonClicked();

    void setNewPos(QPoint);

    void changeTheme(Theme);

    void setSaveLocation(SaveLocation);

    void setDeletionSafety(bool);

    void updateCreditentals (std::tuple<QString, QString, QString>);

    void rearrangeSubWindows(Note*);

    void release_MDI_Scrollbar();

    void animationDone();

    void subwindowGrows(int, NotesSubWindow*);

    void subwindowShrink(int, NotesSubWindow*);

    void deleteNote(Note*);

    void searchPhrase();

    void replyFinished(QNetworkReply*);

private:
    void create_dialog_connections();

    void saveNoteToFile();

    void getFileListInCloud(const QByteArray&);

    void downloadNotesFromCloud();

    void deleteFromCloud(const QString&);

    void deleteFromLocal(const QString&);

    void showCloudNote(const QByteArray&);

    void recreateOldNote(QString text, int colorID,
                       QColor color_F, NoteCategory category,
                       const QDate&);

    QString createFileName();

    void setNoteGeometry();

    void setOnRight();

    void setOnLeft();

    void addNoteToMdiArea(Note*); // check

    void add_NL(QTextEdit*);

    void hashSubWindow(Note*, QMdiSubWindow*);

    void coordinateSubWindow(NotesSubWindow*);

    int getSubWinHeight(NotesSubWindow*);

    void updateMDI_Theme(const QColor& color);

    void updateSearch_Theme(const QColor& color);

    void updateBase_Theme(const QColor& color);

    void updateWindowsGeometry(int startPos, QPoint);

    void updateOffset(const int startPos);

    void deleteSubWindow(Note*);

    void filterByCategory (const NoteCategory&);

    void filterByDate (const DateFilter&);

    void setWindowAfterFiltering();

    void updateNotesNames(int);

    void removeTrashFile();

    void makeSyntaxSearchReady(const QTextEdit*);

    void prepareGraphicsView(); // check

private:
    Ui::StickyNotes *ui;

    QPoint startPos;

    bool isMousePressed = false;
    bool isMouseHoverFrame = false;

    bool showNotesOnRight = true;
    bool showNotesOnLeft = false;

    int noteOffSetY = 1;
    QPoint previousNotePosition;

    QColor themeColor;

    QString lookedPhrase;

    QVector<Note*> allNotes;

    QVector<PhraseHighlighter*> phraseMatchVector;

    QHash <Note*, QMdiSubWindow*> hashNoteWindow;

    QVector<QString> cloudNotes;

    Theme theme;
    static inline SaveLocation location;

    QParallelAnimationGroup* move_animations;

    QMdiSubWindow* hidden_window = nullptr;

    SettingsDialog* settings = nullptr;
    FilterDialog* filterdialog = nullptr;
    NoteTypeDialog* creationdialog = nullptr;

    QList<int> trashFiles;

    QVector<QString> notesInCloud;
    StorageRestApi* azureStorage = nullptr;
};
#endif // STICKYNOTES_H
