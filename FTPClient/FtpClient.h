/**********************************************************************
PACKAGE:        Communication
FILE:           FtpClient.h
COPYRIGHT (C):  All rights reserved.

PURPOSE:        FTP client
**********************************************************************/

#ifndef FTPCLIENT_H
#define FTPCLIENT_H
#include <QObject>
#include <QFtp>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlInfo>
#include <QFile>

class FtpClient : public QObject
{
    Q_OBJECT
public:
    explicit FtpClient(QObject *parent = 0);
    ~FtpClient();

public:
    enum{
        FTP_DEFAULT_PORT = 21
    };

signals:
    void updateProgressVal(int);
    void updateStatusMsg(QString);
    void updateListInfo(const QUrlInfo&);
    void clearListInfo();
    void connectedStatus(bool);

public slots:

    void get(QString fileName, QString dir);
    void put(QString fileName, QString dir);

    void setUserInfo(QString user, QString pwd);
    void setHostPort(QString ip, int port = FTP_DEFAULT_PORT);
    bool getConnectionStatus() const;
    void setPath(QString path);

    bool connectToServer();
    bool disconnectFromServer();

    void cdTo(QString path);
    void cdToRoot();
    void cdToParent();
    void mkdir(QString dir);

private slots:

    void connectOrDisconnect();
    void cancelDownload();
    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void dealStateChanged(int state);

private:

    QFtp *m_ftp;
    QUrl *m_pUrl;

    QFile *m_pFile;

    QString m_statusMsg; // Report message to UI

    bool m_connectedFlag; // Connection flag

    struct File_Info
    {
        QString fileName;
        QString dirPath;
    };

    QList<struct File_Info> m_uploadFileQueue;

    bool m_putDirFlag;  // This flag is used to indicate upload dir to server

    // Re-connect to server
    void reConnectToServer();

    // Upload all files in dir to server
    bool putFilesInDir(QString dir);

    void pushUploadQueue(QString fileName, QString dirPath);
    bool popUploadQueue(struct File_Info &in);

    void refreshList();

    // Send out files in upload queue
    bool processUploadQueue();

};

#endif // FTPCLIENT_H
