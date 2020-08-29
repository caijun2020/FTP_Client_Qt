/**********************************************************************
PACKAGE:        Communication
FILE:           FtpClient.cpp
COPYRIGHT (C):  All rights reserved.

PURPOSE:        FTP client
**********************************************************************/

#include "FtpClient.h"
#include <QFileInfo>
#include <QDir>
#include "QUtilityBox.h"


FtpClient::FtpClient(QObject *parent):
    m_ftp(NULL),
    m_pUrl(new QUrl),
    m_pFile(NULL),
    m_connectedFlag(false),
    m_putDirFlag(false)
{
    m_statusMsg.clear();
    m_pUrl->setScheme("ftp");
}

FtpClient::~FtpClient()
{
    disconnectFromServer();

    delete m_pUrl;
}

bool FtpClient::connectToServer()
{
    bool ret = false;

    if(NULL == m_ftp)
    {
        m_ftp = new QFtp(this);
        connect(m_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
        connect(m_ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
        connect(m_ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
                this, SLOT(updateDataTransferProgress(qint64,qint64)));
        connect(m_ftp, SIGNAL(stateChanged(int)),
                this, SLOT(dealStateChanged(int)));
    }

    if (!m_pUrl->isValid() || m_pUrl->scheme().toLower() != QLatin1String("ftp"))
    {
        return ret;
    }
    else
    {
        m_ftp->connectToHost(m_pUrl->host(), m_pUrl->port());

        if (!m_pUrl->userName().isEmpty())
        {
            m_ftp->login(QUrl::fromPercentEncoding(m_pUrl->userName().toLatin1()), m_pUrl->password());
        }
        else
        {
            m_ftp->login();
        }

        if (!m_pUrl->path().isEmpty())
        {
            m_ftp->cd(m_pUrl->path());
        }
        else
        {
            m_ftp->cd("/");
        }

        ret = true;
    }

    return ret;
}

bool FtpClient::disconnectFromServer()
{
    bool ret = false;

    if (NULL != m_ftp)
    {
        m_ftp->abort();
        m_ftp->close();
        m_ftp->deleteLater();
        m_ftp = NULL;

        m_statusMsg = tr("Disconnected from FTP server %1...")
                .arg(m_pUrl->host());
        // Emit status message
        emit updateStatusMsg(m_statusMsg);

        ret = true;
    }

    return ret;
}

void FtpClient::ftpCommandFinished(int commandId, bool error)
{
    // Note: commandId != m_ftp->currentCommand()
    // Do not use commandId
    //qDebug() << "ftpCommandFinished commandId=" << commandId;
    //qDebug() << "ftpCommandFinished m_ftp->currentCommand()=" << m_ftp->currentCommand();

    switch(m_ftp->currentCommand())
    {
    case QFtp::ConnectToHost:
        if (error)
        {
            m_statusMsg = tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host name is correct.")
                                     .arg(m_pUrl->host());
            connectOrDisconnect();

            // Emit status message
            emit updateStatusMsg(m_statusMsg);
        }
        break;

    case QFtp::Login:
        break;

    case QFtp::Mkdir:
    case QFtp::Rmdir:
    case QFtp::Rename:
    case QFtp::Remove:
    case QFtp::Cd:
        refreshList();

        break;

    case QFtp::Get:
        if (error)
        {
            m_statusMsg = tr("Canceled download of %1")
                    .arg(m_pFile->fileName());

            m_pFile->close();
            m_pFile->remove();
        }
        else
        {
            m_statusMsg = tr("Downloaded at %1")
                    .arg(m_pFile->fileName());

            m_pFile->close();
        }

        // Emit status message
        emit updateStatusMsg(m_statusMsg);

        delete m_pFile;
        m_pFile = NULL;

        break;

    case QFtp::Put:
        refreshList();

        if (error)
        {
            m_statusMsg = tr("Failed to upload of %1")
                    .arg(m_pFile->fileName());
        }
        else
        {
            m_statusMsg = tr("Uploaded %1 to server")
                    .arg(m_pFile->fileName());
        }

        // Emit status message
        emit updateStatusMsg(m_statusMsg);

        m_pFile->close();
        delete m_pFile;
        m_pFile = NULL;

        // Check upload queue, if not empty send out the files in queue
        if(false == processUploadQueue() && m_putDirFlag)
        {
            // Reset flag
            m_putDirFlag = false;

            // file upload complete, go back to parent dir
            cdToParent();
        }

        break;

    case QFtp::List:
        break;

    default:
        break;
    }
}

void FtpClient::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    int progress = 100 * readBytes / totalBytes;

    // Emit signal
    emit updateProgressVal(progress);
}

void FtpClient::dealStateChanged(int state)
{
    m_statusMsg.clear();

    switch(state)
    {
    case QFtp::Unconnected:
        m_connectedFlag = false;

        m_statusMsg = tr("Disconnected from FTP server %1...")
                .arg(m_pUrl->host());
        break;
    case QFtp::HostLookup:
        break;
    case QFtp::Connecting:
        m_statusMsg = tr("Connecting to FTP server %1...")
                .arg(m_pUrl->host());
        break;
    case QFtp::Connected:
        m_connectedFlag = true;

        m_statusMsg = tr("Connected to FTP server %1...")
                .arg(m_pUrl->host());
        break;
    case QFtp::LoggedIn:
        m_statusMsg = tr("Logged onto %1")
                .arg(m_pUrl->host());
        break;
    case QFtp::Closing:
        m_connectedFlag = false;
//        m_statusMsg = tr("Closing FTP server %1...")
//                .arg(m_pUrl->host());
        break;
    default:
        break;
    }

    //Emit signal
    emit connectedStatus(m_connectedFlag);

    if(!m_statusMsg.isEmpty())
    {
        emit updateStatusMsg(m_statusMsg);
    }
}

void FtpClient::addToList(const QUrlInfo &urlInfo)
{
    // Emit signal
    emit updateListInfo(urlInfo);
}

void FtpClient::setUserInfo(QString user, QString pwd)
{
    m_pUrl->setUserName(user);
    m_pUrl->setPassword(pwd);
}

void FtpClient::setHostPort(QString ip, int port)
{
    m_pUrl->setHost(ip);
    m_pUrl->setPort(port);
}

void FtpClient::setPath(QString path)
{
    m_pUrl->setPath(path);
}

void FtpClient::get(QString fileName, QString dir)
{
    if (NULL == m_ftp)
    {
        return;
    }

    reConnectToServer();

    QString fullFileName = "";
    fullFileName.append(dir);
    fullFileName.append("/");
    fullFileName.append(fileName);

    if (QFile::exists(fullFileName))
    {
        m_statusMsg = tr("There already exists a file called %1 in the current directory")
                .arg(fileName);
    }
    else
    {
        m_pFile = new QFile(fullFileName);
        if (!m_pFile->open(QIODevice::WriteOnly))
        {
            m_statusMsg = tr("Unable to save the file %1: %2")
                    .arg(fullFileName)
                    .arg(m_pFile->errorString());

            delete m_pFile;
            m_pFile = NULL;
        }
        else
        {
            m_ftp->get(fileName, m_pFile);

            m_statusMsg = tr("Downloading %1...").arg(fileName);
        }
    }

    // Emit status message
    emit updateStatusMsg(m_statusMsg);
}

void FtpClient::put(QString fileName, QString dir)
{
    QByteArray data;

    if (NULL == m_ftp)
    {
        return;
    }

    reConnectToServer();

    QString fullFileName = "";
    fullFileName.append(dir);
    fullFileName.append("/");
    fullFileName.append(fileName);

    QFileInfo fileInfo(fullFileName);

    // If it's a file
    if(fileInfo.isFile())
    {
        if (!QFile::exists(fullFileName))
        {
            m_statusMsg = tr("No file called %1 in the current directory")
                    .arg(fileName);
        }
        else
        {
            m_pFile = new QFile(fullFileName);
            if (!m_pFile->open(QIODevice::ReadOnly))
            {
                m_statusMsg = tr("Unable to Open the file %1: %2")
                        .arg(fullFileName).arg(m_pFile->errorString());
            }
            else
            {
                data = m_pFile->readAll();

                m_ftp->put(data, fileName);

                m_statusMsg = tr("Uploading %1...").arg(fileName);
            }
        }
    }
    // If it's a directory
    else if(fileInfo.isDir())
    {
        mkdir(fileName);
        m_statusMsg = tr("Created directory %1").arg(fileName);

        if(!putFilesInDir(fullFileName))
        {
        }
    }

    // Emit status message
    emit updateStatusMsg(m_statusMsg);
}

void FtpClient::cancelDownload()
{
    if (NULL == m_ftp)
    {
        return;
    }

    m_ftp->abort();

    if (m_pFile->exists())
    {
        m_pFile->close();
        m_pFile->remove();
    }

    delete m_pFile;
    m_pFile = NULL;
}

void FtpClient::connectOrDisconnect()
{
    if (disconnectFromServer())
    {
        return;
    }

    connectToServer();
}

void FtpClient::cdTo(QString path)
{
    setPath(path);

    if(NULL != m_ftp)
    {
        reConnectToServer();
        m_ftp->cd(path);
    }
}

void FtpClient::cdToRoot()
{
    cdTo("/");
}

void FtpClient::cdToParent()
{
    QString path = m_pUrl->path();
    path = path.left(path.lastIndexOf('/'));

    qDebug() <<"cdToParent cdTo=" << path;
    cdTo(path);
}

void FtpClient::mkdir(QString dir)
{
    if(NULL != m_ftp)
    {
       reConnectToServer();
       m_ftp->mkdir(dir);
    }
}

bool FtpClient::getConnectionStatus() const
{
    return m_connectedFlag;
}

void FtpClient::reConnectToServer()
{
    if(!m_connectedFlag)
    {
        connectToServer();
    }
}

bool FtpClient::putFilesInDir(QString dir)
{
    bool ret = false;
    QUtilityBox toolBox;
    QFileInfoList infoList = toolBox.getFolderInfo(dir);

    if(NULL == m_ftp)
    {
        return ret;
    }

    for(int i = 0; i < infoList.size(); i++)
    {
        if(infoList.at(i).isFile())
        {
            pushUploadQueue(infoList.at(i).fileName(), infoList.at(i).canonicalPath());
        }
    }

    // Send out the 1st file in Queue
    // There are . and .. dir, so here infoList.size() at least >= 2
    if(infoList.size() > 2)
    {
        struct File_Info info;
        if(popUploadQueue(info))
        {
            // Set flag indicate putting files in dir
            m_putDirFlag = true;

            // Enter to created dir to upload files
            QDir dirInfo(dir);
            QString newDir = m_pUrl->path().append("/").append(dirInfo.dirName());
            cdTo(newDir);

            put(info.fileName, info.dirPath);

            ret = true;
        }
    }

    return ret;
}

void FtpClient::pushUploadQueue(QString fileName, QString dirPath)
{
    struct File_Info info;
    info.fileName = fileName;
    info.dirPath = dirPath;

    m_uploadFileQueue.push_back(info);
}

bool FtpClient::popUploadQueue(struct File_Info &in)
{
    bool ret = false;

    if(!m_uploadFileQueue.isEmpty())
    {
        in = m_uploadFileQueue.at(0);
        m_uploadFileQueue.pop_front();

        ret = true;
    }

    return ret;
}

bool FtpClient::processUploadQueue()
{
    bool ret = false;
    struct File_Info info;

    ret = popUploadQueue(info);
    if(ret)
    {
        put(info.fileName, info.dirPath);
    }

    return ret;
}

void FtpClient::refreshList()
{
    // Emit signal
    emit clearListInfo();

    m_ftp->list();
}
