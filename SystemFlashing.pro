QT += quick core widgets xml

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

linux-g++ | linux-g++-64 | linux-g++-32 {
QMAKE_LFLAGS += -no-pie
# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += link_pkgconfig


# PKGCONFIG += libusb-1.0
# sudo apt install qml-module-qtquick-controls2
# sudo apt install qtquickcontrols2-5-dev
# sudo apt-get install  qtdeclarative5-dev
# sudo apt-get install qml-module-qtquick-dialogs
# sudo apt-get install qml-module-qtquick-extras
# sudo apt-get install qml-module-qt-labs-folderlistmodel
# sudo apt-get install qml-module-qt-labs-settings

}

SOURCES += \
        jetsontx2flashinginfo.cpp \
        main.cpp \
        terminalprocess.cpp \
    usbdetector.cpp

RESOURCES += qml.qrc \
    icon.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    SystemFlashing_resource.rc \
    jetsontx2flashinginfo.h \
    terminalprocess.h \
    usbdetector.h

DISTFILES += \
    init.xml \
    init_test.xml \
    last_log.xml \
    init_vmware.xml

RC_FILE = SystemFlashing_resource.rc
