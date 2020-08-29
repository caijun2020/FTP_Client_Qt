/**********************************************************************
PACKAGE:        Utility
FILE:           QUtilityBox.cpp
COPYRIGHT (C):  All rights reserved.

PURPOSE:        General tool box, provide buffer convert function and etc.
**********************************************************************/

#include "QUtilityBox.h"
#include <QStringList>
#include <QRegExp>
#include <QDir>
#include <QDebug>

QUtilityBox::QUtilityBox()
{
}

QUtilityBox::~QUtilityBox()
{
}

uint32_t QUtilityBox::convertHexStringToDataBuffer(uint8_t *convertedDataBuffer, const QString inputStr)
{
    int len = 0;
    bool ok;

    QStringList dataList;
    dataList = inputStr.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    len = dataList.size();
    for(int i = 0; i < dataList.size(); i++)
    {
        //qDebug() << "dataList.at(" << i << ") = " << dataList.at(i);
        *(convertedDataBuffer + i) = dataList.at(i).toInt(&ok, 16);
    }

    return len;
}

uint32_t QUtilityBox::convertDecStringToDataBuffer(uint8_t *convertedDataBuffer, const QString inputStr)
{
    int len = 0;
    bool ok;

    QStringList dataList;
    dataList = inputStr.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    len = dataList.size();
    for(int i = 0; i < dataList.size(); i++)
    {
        //qDebug() << "dataList.at(" << i << ") = " << dataList.at(i);
        *(convertedDataBuffer + i) = dataList.at(i).toInt(&ok, 10);
    }

    return len;
}

QString QUtilityBox::convertDataToHexString(QByteArray data)
{
    QString retStr = "";

    for(int i = 0; i < data.size(); i++)
    {
    #if 1
        retStr.append(QString::number((uint8_t)data.at(i), 16).rightJustified(2, '0').toUpper());
        retStr.append(" ");
    #else
        char tmpBuf[2048] = {0};
        sprintf(tmpBuf, "%02X ", (uint8_t)data.at(i));
        retStr.append(tmpBuf);
    #endif
    }

    return retStr;
}

QString QUtilityBox::convertDataToHexString(const uint8_t *data, int len)
{
    QByteArray tmpData((char *)data, len);

    return convertDataToHexString(tmpData);
}

QFileInfoList QUtilityBox::getFolderInfo(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters.clear();

    QFileInfoList fileDirs = dir.entryInfoList(nameFilters, QDir::Dirs|QDir::Files|QDir::Readable, QDir::Name);

    return fileDirs;
}
