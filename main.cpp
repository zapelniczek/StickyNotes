#include "stickynotes.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StickyNotes w;
    w.show();
    w.readSettings();

    switch (w.getSaveLocation())
    {
    case SaveLocation::Local:
        w.readNotesFromFiles();
        break;
    case SaveLocation::Cloud:
        w.downloadNotes();
        break;
    case SaveLocation::LocalAndCloud:
        w.downloadNotes();
        w.readNotesFromFiles();
    }

    return a.exec();
}
