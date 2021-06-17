#ifndef CATEGORYLISTWIDGET_H
#define CATEGORYLISTWIDGET_H

#include <QListWidget>

class CategoryListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CategoryListWidget(QWidget* parent = nullptr);

    QSize sizeHint() const override;
};

#endif // CATEGORYLISTWIDGET_H
