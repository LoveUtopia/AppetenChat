QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -lWs2_32

SOURCES += \
    chat.cpp \
    chatgroup.cpp \
    ckernel.cpp \
    friendwidget.cpp \
    groupitem.cpp \
    inet/TcpClient.cpp \
    inet/UdpNet.cpp \
    inetMediator/TcpClientMediator.cpp \
    inetMediator/UdpNetMediator.cpp \
    inetMediator/inetMediator.cpp \
    main.cpp \
    chatdialog.cpp \
    mdialog.cpp \
    modifyprofile.cpp

HEADERS += \
    chat.h \
    chatdialog.h \
    chatgroup.h \
    ckernel.h \
    friendwidget.h \
    groupitem.h \
    inet/TcpClient.h \
    inet/UdpNet.h \
    inet/inet.h \
    inet/packDef.h \
    inetMediator/TcpClientMediator.h \
    inetMediator/UdpNetMediator.h \
    inetMediator/inetMediator.h \
    mdialog.h \
    modifyprofile.h

FORMS += \
    chat.ui \
    chatdialog.ui \
    chatgroup.ui \
    friendwidget.ui \
    groupitem.ui \
    mdialog.ui \
    modifyprofile.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
