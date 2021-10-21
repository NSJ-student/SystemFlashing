QT += quick core widgets xml

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

linux-g++ | linux-g++-64 | linux-g++-32 {
QMAKE_LFLAGS += -no-pie
# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += link_pkgconfig

PKGCONFIG += libusb-1.0

}

SOURCES += \
        jetsontx2flashinginfo.cpp \
        main.cpp \
        terminalprocess.cpp \
    usbdetector.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    jetsontx2flashinginfo.h \
    terminalprocess.h \
    usbdetector.h

DISTFILES += \
    init.xml \
    init_test.xml \
    last_log.xml
