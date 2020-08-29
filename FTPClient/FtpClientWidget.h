/**********************************************************************
PACKAGE:        Communication
FILE:           FtpClientWidget.h
COPYRIGHT (C):  All rights reserved.

PURPOSE:        FTP client widget UI
**********************************************************************/

#ifndef FTPCLIENTWIDGET_H
#define FTPCLIENTWIDGET_H

#include <QWidget>
#include <QHash>
#include <QFileInfoList>
#include <QListWidgetItem>
#include "FtpClient.h"

namespace Ui {
class FtpClientWidget;
}

class FtpClientWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FtpClientWidget(QWidget *parent = 0);
    ~FtpClientWidget();

    /*-----------------------------------------------------------------------
    FUNCTION:		bindModel
    PURPOSE:		Bind a FtpClient model
    ARGUMENTS:		FtpClient *modelP -- FtpClient pointer
    RETURNS:		None
    -----------------------------------------------------------------------*/
    void bindModel(FtpClient *modelP);

    /*-----------------------------------------------------------------------
    FUNCTION:		unbind
    PURPOSE:		Unbind the FtpClient model
    ARGUMENTS:		None
    RETURNS:		None
    -----------------------------------------------------------------------*/
    void unbind();

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void on_pushButton_connect_clicked();

    void updateProgress(int value);
    void addToServerList(const QUrlInfo &urlInfo);
    void updateStatusBar(QString str);
    void updateConnectionStatus(bool isConnected);

    void on_pushButton_browseLocal_clicked();

    void on_pushButton_download_clicked();

    void on_pushButton_upload_clicked();

    void processLocalListItem(QListWidgetItem *item);
    void processServerListItem(QListWidgetItem *item);

    void on_lineEdit_localDir_textChanged(const QString &arg1);

    void on_pushButton_serverBack_clicked();

    void on_pushButton_clear_clicked();

    bool enableDownloadButton();
    bool enableUploadButton();

    void clearServerList();
    void clearLocalList();

private:
    Ui::FtpClientWidget *ui;

    FtpClient *ftpClient;

    QHash<QString, bool> isServerDirectory;
    QHash<QString, bool> isLocalDirectory;

    QFileInfoList m_localFileInfoList;

    void initWidgetFont();  // Init the Font type and size of the widget
    void initWidgetStyle(); // Init Icon of the widget

    void addToLocalList(QString dirInfo);

    void showLocalDir();

    // cd to parent on server
    void cdToParent();

    // Update log
    void updateLogData(QString logStr);
};

#endif // FTPCLIENTWIDGET_H
