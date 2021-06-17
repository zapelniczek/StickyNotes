#ifndef HEADER_H
#define HEADER_H
#include <QIcon>

enum class NoteCategory : int
{
    General = 0,
    Important = 1,
    Anniversary = 2,
    Work = 3,
    Meeting = 4,
    All = 5,
    None = -1 // Category hasn't been selected yet.
};

enum class DateFilter : int
{
    Ascending = 6,
    Descending = 7
};

enum class Theme : int
{
  Light = 0,
  Dark = 1
};

enum class SaveLocation
{
    Local = 0,
    Cloud = 1,
    LocalAndCloud = 2
};

QIcon getIconCategory(NoteCategory);
QString getTooltipCategory(NoteCategory);

#endif // HEADER_H
