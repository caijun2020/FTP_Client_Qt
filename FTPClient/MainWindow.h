#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FtpClient.h"
#include "FtpClientWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void resizeEvent(QResizeEvent *e);

private:
    Ui::MainWindow *ui;

    FtpClient *ftpClient;
    FtpClientWidget *ftpClientW;
};

#endif // MAINWINDOW_H
