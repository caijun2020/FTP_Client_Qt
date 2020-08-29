#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>
#include <windows.h>
#include <QDebug>

#define USE_SYS_QTEXTCODEC 1

void getSysLanguage();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Add the plugins path, so the picture will be displayed normally
    // SQL driver and TextCodec will be parsed normally
    QApplication::addLibraryPath("./plugins");

    // Check Sys language
    getSysLanguage();

#ifdef USE_SYS_QTEXTCODEC
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForTr(codec);

#if 1
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
#else
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
#endif

#endif

    w.show();
    
    return a.exec();
}

void getSysLanguage()
{
    UINT  nLanID   =   GetSystemDefaultLangID();
    WORD  PriLan   =   PRIMARYLANGID(nLanID);
    WORD  SubLan   =   SUBLANGID(nLanID);

    if( PriLan == LANG_ENGLISH )
    {
        qDebug() << "Sys language is English";
    }
    else if( PriLan == LANG_CHINESE )
    {
        if( SubLan == SUBLANG_CHINESE_SIMPLIFIED )
        {
            qDebug() << "Sys language is Chinese Simplified";
        }
        else if( SubLan == SUBLANG_CHINESE_TRADITIONAL )
        {
            qDebug() << "Sys language is Chinese Traditional";
        }
    }
}
