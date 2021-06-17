#ifndef CATEGORYSELECTOR_H
#define CATEGORYSELECTOR_H

#include <QDialog>
#include "Header.h"

namespace Ui {
class CategorySelector;
}

class CategorySelector : public QDialog
{
    Q_OBJECT

public:
    explicit CategorySelector(QWidget *parent = nullptr);

    ~CategorySelector();

    void setSizeHintItems();

    NoteCategory getNoteCategory() const;

    void prepareWindow(const QColor&, const QColor&);

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void setCategory(const QModelIndex&);

private:
    Ui::CategorySelector *ui;

    NoteCategory category;
};

#endif // CATEGORYSELECTOR_H
