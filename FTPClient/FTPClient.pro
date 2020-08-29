#-------------------------------------------------
#
# Project created by QtCreator 2020-08-24T16:28:54
#
#-------------------------------------------------

QT       += core gui network

TARGET = FTPClient
TEMPLATE = app


SOURCES += main.cpp\
    FtpClient.cpp \
    FtpClientWidget.cpp \
    MainWindow.cpp \
    QUtilityBox.cpp

HEADERS  += \
    FtpClient.h \
    FtpClientWidget.h \
    MainWindow.h \
    QtBaseType.h \
    QUtilityBox.h

FORMS    += \
    FtpClientWidget.ui \
    MainWindow.ui

RESOURCES += \
    ftp.qrc

RC_FILE = icon.rc

OTHER_FILES += \
    images/file.png \
    images/dir.png \
    images/cdtoparent.png
