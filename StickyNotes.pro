QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HelperFunctions.cpp \
    azurelogindialog.cpp \
    categorylistwidget.cpp \
    categoryselector.cpp \
    colorselector.cpp \
    filterdialog.cpp \
    main.cpp \
    note.cpp \
    notessubwindow.cpp \
    notetypedialog.cpp \
    phrasehighlighter.cpp \
    settingsdialog.cpp \
    stickynotes.cpp \
    storagerestapi.cpp \
    switchbutton.cpp

HEADERS += \
    Header.h \
    azurelogindialog.h \
    categorylistwidget.h \
    categoryselector.h \
    colorselector.h \
    filterdialog.h \
    note.h \
    notessubwindow.h \
    notetypedialog.h \
    phrasehighlighter.h \
    settingsdialog.h \
    stickynotes.h \
    storagerestapi.h \
    switchbutton.h

FORMS += \
    azurelogindialog.ui \
    categoryselector.ui \
    colorselector.ui \
    filterdialog.ui \
    note.ui \
    notetypedialog.ui \
    settingsdialog.ui \
    stickynotes.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    IconRes.qrc
