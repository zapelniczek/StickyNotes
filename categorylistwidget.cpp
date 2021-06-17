#include "categorylistwidget.h"

CategoryListWidget::CategoryListWidget(QWidget* parent) :
    QListWidget(parent)
{
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize CategoryListWidget::sizeHint() const
{
    QSize hint;
    hint.setWidth(maximumWidth());
    hint.setHeight(maximumHeight());
    return hint;
}
