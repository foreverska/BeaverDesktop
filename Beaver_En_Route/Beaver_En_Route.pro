QT += quick serialbus

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        BeaverBoot/bootping.cpp \
        BeaverBoot/bootreboot.cpp \
        BeaverBoot/bootwrite.cpp \
        firmware.cpp \
        gui.cpp \
        logger.cpp \
        main.cpp \
        section.cpp \
        sectioniterator.cpp \
        updater.cpp

RESOURCES += qml.qrc

LIBS += -lelf++

INCLUDEPATH += /usr/include/libelfin/

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    BeaverBoot/bootping.h \
    BeaverBoot/bootreboot.h \
    BeaverBoot/bootwrite.h \
    firmware.h \
    gui.h \
    logger.h \
    section.h \
    sectioniterator.h \
    updater.h

STATECHARTS +=
