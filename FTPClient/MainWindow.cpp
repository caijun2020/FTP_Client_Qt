#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ftpClient(new FtpClient),
    ftpClientW(new FtpClientWidget)
{
    ui->setupUi(this);

    ftpClientW->setParent(ui->centralWidget);
    ftpClientW->bindModel(ftpClient);

    // Set Window Title
    this->setWindowTitle( tr("FTP Client") );

    // Set Menu Bar Version Info
    ui->menuVersion->addAction("V1.0 2020-Aug-28");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete ftpClientW;
    delete ftpClient;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    ftpClientW->resize(ui->centralWidget->size());
}
