#include "Header.h"

QIcon getIconCategory(NoteCategory category)
{
    switch (category)
    {
    case NoteCategory::General:
        return QIcon(":/Note/Category/images/g.png");

    case NoteCategory::Important:
        return QIcon(":/Note/Category/images/exclamation.png");

    case NoteCategory::Anniversary:
        return QIcon(":/Note/Category/images/wine.png");

    case NoteCategory::Work:
        return QIcon(":/Note/Category/images/work.png");

    case NoteCategory::Meeting:
        return QIcon(":/Note/Category/images/chat.png");

    default:
        return QIcon();
    }
}

QString getTooltipCategory(NoteCategory category)
{
    switch (category)
    {
    case NoteCategory::General:
        return QObject::tr("General note\nChange category by clicking");

    case NoteCategory::Important:
        return  QObject::tr("Important note\nChange category by clicking");

    case NoteCategory::Anniversary:
        return QObject::tr("Anniversary note\nChange category by clicking");

    case NoteCategory::Work:
        return QObject::tr("Work note\nChange category by clicking");

    case NoteCategory::Meeting:
        return QObject::tr("Meeting note\nChange category by clicking");

    default:
        return QString();
    }
}
