#ifndef NOTESSUBWINDOW_H
#define NOTESSUBWINDOW_H

#include <QMdiSubWindow>
#include <QToolButton>
#include <QMenu>
#include <QDateEdit>
#include "note.h"
#include "notetypedialog.h"

class NotesSubWindow : public QMdiSubWindow
{
    Q_OBJECT

private:
    Note* note;

    QToolButton* menuButton;
    QMenu* menu;

    QAction* openNote;
    QAction* closeAction;
    QAction* deleteNote;

    QPushButton* categoryButton;

    QDateEdit* date;

    NoteTypeDialog* typeDialog = nullptr;

    NoteCategory category;

    int visible_lines;
    constexpr static int min_line_visible = 2;
    constexpr static int max_line_visible = 4;

    static inline int block_height;

    inline static bool safeDelete = false;

private:
    void setUpButtons();

    void createActions();

    void setUpCategoryButton();

    void setUpDate(const QDate&);

private slots:
    void showNote();

    void closeNote();

    void prepareToDelateNote();

    void updateToolBarMenu(bool);

    void categoryButtonClicked();

    void setUpButtonCategory(const NoteCategory&);

    void recieveCategory();

protected:
    void mouseDoubleClickEvent(QMouseEvent* mouseEvent) override;

    void enterEvent(QEvent* event) override;

    void leaveEvent(QEvent* leaveEvent) override;

    void mouseMoveEvent(QMouseEvent* mouseEvent) override;

    void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

    void mousePressEvent(QMouseEvent* mouseEvent) override;

signals:
    void expand(int, NotesSubWindow*);

    void shrink(int, NotesSubWindow*);

    void sendPriorityWindowRequest(Note*);

    void categoryChanged(NoteCategory);

public slots:
    void setText(QString);

    void updateWindowColor();

public:
    explicit NotesSubWindow(QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::WindowFlags());

    void add_visibile_lines(const int);

    void setNote(Note*);

    static void setBlockHeight(const int);
    static int getBlockHeight();

    int getMinVisLines() const;
    int getMaxVisLines() const;

    static void setSafeDeletion (bool);
};

#endif // NOTESSUBWINDOW_H
