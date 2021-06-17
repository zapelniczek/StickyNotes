#ifndef NOTE_H
#define NOTE_H

#include <QDialog>
#include <QTextEdit>
#include "colorselector.h"
#include "categoryselector.h"

namespace Ui {
class Note;
}

class Note : public QDialog
{
    Q_OBJECT
public:
    explicit Note(QWidget *parent = nullptr);

    explicit Note(const QString&, int, const QColor&, const NoteCategory&,
                  const QDate&, QWidget* = nullptr);
    ~Note();

    QTextEdit* getNoteText() const;

    int getColorTheme() const;

    QColor getTextColor() const;

    QDate getDate() const;

    QString getFileName() const;

    void setFileName(const QString&);

    void setCategory(const NoteCategory&);

    NoteCategory getCategory() const;

    void swapFileNames(Note*);

public slots:
    void show();
    void close();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

    bool event (QEvent * event ) override;

signals:
    void deleteNote(Note*);

    void themeColorChanged();

    void sendText(const QString);

    void noteHidden(const bool);

    void categoryChanged(const NoteCategory);

private slots:
    void colorButton_clicked();

    void setNewColor(long);

    void closeButton_clicked();

    void categoryButton_clicked();

    void acquireCategory();

    void boldButton_clicked();

    void isBold();

    void italicButton_clicked();

    void isItalic();

    void underlineButton_clicked();

    void isUnderline();

    void listButton_clicked();

    void isList();

    void removeEmptyItem();

    void textChanged();

private:
    void makeConnections();

    void setNewPalette();

    void updateTextColor();

    void updateDateColor();

    void setDateOfCreation(const QDate&);

private:
    Ui::Note *ui;

    ColorSelector::CurrentColor themeColor;

    QString fileName;
    NoteCategory category;

    ColorSelector* colorSelector = nullptr;
    CategorySelector* categorySelector = nullptr;

    QPoint startPos;
    bool isMousePressed = false;
    bool isMouseHoverFrame = false;


};
#endif // NOTE_H
