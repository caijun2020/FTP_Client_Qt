/**********************************************************************
PACKAGE:        Communication
FILE:           FtpClientWidget.cpp
COPYRIGHT (C):  All rights reserved.

PURPOSE:        FTP client widget UI
**********************************************************************/

#include "FtpClientWidget.h"
#include "ui_FtpClientWidget.h"
#include "QtBaseType.h"
#include <QIcon>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include "QUtilityBox.h"
#include <QDebug>

FtpClientWidget::FtpClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FtpClientWidget),
    ftpClient(NULL)
{
    ui->setupUi(this);

    // Init Widget Font type and size
    initWidgetFont();

    // Init Widget Style
    initWidgetStyle();

    // Connect signals & slots
    connect(ui->listWidget_local, SIGNAL(itemActivated(QListWidgetItem*)),
                this, SLOT(processLocalListItem(QListWidgetItem*)));
    connect(ui->listWidget_local, SIGNAL(pressed(QModelIndex)),
                this, SLOT(enableUploadButton()));

    connect(ui->listWidget_server, SIGNAL(itemActivated(QListWidgetItem*)),
                this, SLOT(processServerListItem(QListWidgetItem*)));
    connect(ui->listWidget_server, SIGNAL(pressed(QModelIndex)),
            this, SLOT(enableDownloadButton()));

}

FtpClientWidget::~FtpClientWidget()
{
    delete ui;
}

void FtpClientWidget::resizeEvent(QResizeEvent *e)
{
    QWidget *pWidget = static_cast<QWidget*>(this->parent());

    if(pWidget != NULL)
    {
        this->resize(pWidget->size());
    }
}

void FtpClientWidget::bindModel(FtpClient *modelP)
{
    if(NULL != modelP)
    {
        unbind();

        ftpClient = modelP;
        connect(ftpClient, SIGNAL(updateListInfo(QUrlInfo)), this, SLOT(addToServerList(QUrlInfo)));
        connect(ftpClient, SIGNAL(updateProgressVal(int)), this, SLOT(updateProgress(int)));
        connect(ftpClient, SIGNAL(updateStatusMsg(QString)), this, SLOT(updateStatusBar(QString)));
        connect(ftpClient, SIGNAL(connectedStatus(bool)), this, SLOT(updateConnectionStatus(bool)));
        connect(ftpClient, SIGNAL(clearListInfo()), this, SLOT(clearServerList()));
    }
}

void FtpClientWidget::unbind()
{
    if(NULL != ftpClient)
    {
        disconnect(ftpClient, 0 , this , 0);
    }

    ftpClient = NULL;
}

void FtpClientWidget::initWidgetFont()
{
}

void FtpClientWidget::initWidgetStyle()
{
    ui->pushButton_connect->setText(tr("Connect"));

    // Update Status Color
    ui->label_connectStatus->setStyleSheet(BG_COLOR_RED);
    ui->label_connectStatus->setText("");

    // Init FTP default port
    ui->lineEdit_port->setText(QString::number(21));

    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_userName->setText("caijun");
    ui->lineEdit_password->setText("123456");

    // Progress bar
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

    // Pushbutton
    ui->pushButton_serverBack->setIcon(QPixmap(":/images/cdtoparent.png"));

    // Set exe running dir
    ui->lineEdit_localDir->setText(QDir::currentPath());

    ui->pushButton_download->setEnabled(false);
    ui->pushButton_upload->setEnabled(false);
}

void FtpClientWidget::on_pushButton_connect_clicked()
{
    // Clear list widget of server dir
    clearServerList();

    if(NULL != ftpClient)
    {
        if(ftpClient->getConnectionStatus())
        {
            ftpClient->disconnectFromServer();
        }
        else
        {
            ftpClient->setHostPort(ui->lineEdit_IP->text(), ui->lineEdit_port->text().toInt());
            ftpClient->setUserInfo(ui->lineEdit_userName->text(), ui->lineEdit_password->text());
            ftpClient->connectToServer();
        }
    }
}


void FtpClientWidget::addToServerList(const QUrlInfo &urlInfo)
{
    qDebug() << "addToServerList " << urlInfo.name();

    QListWidgetItem* item = new QListWidgetItem(urlInfo.name());

    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(QIcon(pixmap));

    // Search the item in the list
    QList<QListWidgetItem *> listItems = ui->listWidget_server->findItems(urlInfo.name(), Qt::MatchExactly);

    // Not found in the list, add to list
    if(0 == listItems.size())
    {
        ui->listWidget_server->addItem(item);

        isServerDirectory[urlInfo.name()] = urlInfo.isDir();
    }
}


void FtpClientWidget::updateProgress(int value)
{
    ui->progressBar->setValue(value);

    // If file transfer complete, refresh the local directory
    if(value == ui->progressBar->maximum())
    {
        showLocalDir();
    }
}

void FtpClientWidget::updateStatusBar(QString str)
{
    ui->label_status->setText(str);

    // Update log
    updateLogData(str);

    qDebug() << str;
}

void FtpClientWidget::updateConnectionStatus(bool isConnected)
{
    if(isConnected)
    {
        ui->pushButton_connect->setText(tr("Disconnect"));

        // Update Status Color
        ui->label_connectStatus->setStyleSheet(BG_COLOR_GREEN);
        ui->label_connectStatus->setText("");
    }
    else
    {
        ui->pushButton_connect->setText(tr("Connect"));

        // Update Status Color
        ui->label_connectStatus->setStyleSheet(BG_COLOR_RED);
        ui->label_connectStatus->setText("");
    }
}

void FtpClientWidget::showLocalDir()
{
    QUtilityBox toolBox;
    m_localFileInfoList = toolBox.getFolderInfo(ui->lineEdit_localDir->text());

    // Clear list widget of local dir
    clearLocalList();

    for(int i = 0; i < m_localFileInfoList.size(); i++)
    {
        // Hide the folder named by .
        if("." == m_localFileInfoList.at(i).fileName())
        {
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(m_localFileInfoList.at(i).fileName());

        QPixmap pixmap(m_localFileInfoList.at(i).isDir() ? ":/images/dir.png" : ":/images/file.png");
        item->setIcon(QIcon(pixmap));
        ui->listWidget_local->addItem(item);

        isLocalDirectory[m_localFileInfoList.at(i).fileName()] = m_localFileInfoList.at(i).isDir();
    }
}

void FtpClientWidget::on_pushButton_browseLocal_clicked()
{
    QString defaultLocalDir = ui->lineEdit_localDir->text();

    QString directory = QFileDialog::getExistingDirectory( this,
                                                 tr("Select the FTP Root Directory for Data Transfer"),
                                                 defaultLocalDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!directory.isEmpty())
    {
        ui->lineEdit_localDir->setText(directory);
    }
}

void FtpClientWidget::on_pushButton_download_clicked()
{
    if(NULL == ftpClient)
    {
        return;
    }

    if(enableDownloadButton())
    {
        QString fileName = ui->listWidget_server->currentItem()->text();
        ftpClient->get(fileName, ui->lineEdit_localDir->text());
    }
}

void FtpClientWidget::on_pushButton_upload_clicked()
{
    if(NULL == ftpClient)
    {
        return;
    }

    if(enableUploadButton())
    {
        QString fileName = ui->listWidget_local->currentItem()->text();
        ftpClient->put(fileName, ui->lineEdit_localDir->text());
    }
}

void FtpClientWidget::processLocalListItem(QListWidgetItem *item)
{
    QString name = item->text();
    if (isLocalDirectory.value(name))
    {
        for(int i = 0; i < m_localFileInfoList.size(); i++)
        {
            if(name == m_localFileInfoList.at(i).fileName())
            {
                // Get path without .. and .
                QString path = m_localFileInfoList.at(i).canonicalFilePath();

                ui->lineEdit_localDir->setText(path);
                break;
            }
        }
    }
}

void FtpClientWidget::processServerListItem(QListWidgetItem *item)
{
    QString name = item->text();
    if (isServerDirectory.value(name))
    {
        // Clear list widget of server dir
        clearServerList();

        QString path = ui->lineEdit_serverDir->text();
        path.append("/");
        path.append(name);

        ftpClient->cdTo(path);
        ui->lineEdit_serverDir->setText(path);
    }
}


void FtpClientWidget::on_lineEdit_localDir_textChanged(const QString &arg1)
{
    showLocalDir();
}

void FtpClientWidget::cdToParent()
{
    QString path = ui->lineEdit_serverDir->text();

    if(NULL == ftpClient)
    {
        return;
    }

    path = path.left(path.lastIndexOf('/'));
    ui->lineEdit_serverDir->setText(path);

    if (path.isEmpty())
    {
        ftpClient->cdTo("/");
    }
    else
    {
        ftpClient->cdTo(path);
    }
}

void FtpClientWidget::on_pushButton_serverBack_clicked()
{
    cdToParent();
}

void FtpClientWidget::on_pushButton_clear_clicked()
{
    ui->textEdit_log->clear();
}

void FtpClientWidget::updateLogData(QString logStr)
{
    QDateTime time = QDateTime::currentDateTime();
    QString timeStr = time.toString("[yyyy-MM-dd hh:mm:ss:zzz] ");

    // Add time stamp
    logStr.prepend(timeStr);

    //logFile->addLogToFile(logStr);
    ui->textEdit_log->insertPlainText(logStr.append("\n")); //Display the log in the textBrowse
    ui->textEdit_log->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

bool FtpClientWidget::enableDownloadButton()
{
    bool ret = false;
    int current = ui->listWidget_server->currentRow();

    //qDebug() << "ui->listWidget_server->currentRow(); = " << ui->listWidget_server->currentRow();
    if (current >= 0)
    {
        ret = true;
    }

    ui->pushButton_download->setEnabled(ret);

    return ret;
}

bool FtpClientWidget::enableUploadButton()
{
    bool ret = false;
    int current = ui->listWidget_local->currentRow();

    //qDebug() << "ui->listWidget_local->currentRow(); = " << ui->listWidget_local->currentRow();
    if (current >= 0)
    {
        ret = true;
    }

    ui->pushButton_upload->setEnabled(ret);

    return ret;
}

void FtpClientWidget::clearServerList()
{
    // Clear list widget
    ui->listWidget_server->clear();
    isServerDirectory.clear();
}

void FtpClientWidget::clearLocalList()
{
    // Clear list widget
    ui->listWidget_local->clear();
    isLocalDirectory.clear();
}
