#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString pin = ui->pin->text();
    QByteArray hashedPin = QCryptographicHash::hash(QByteArray(pin.toUtf8()), QCryptographicHash::Md5).toHex();

    if (hashedPin == hash){
        hide();
        //window->key = hashedPin;
        window = new SecondWindow(this);
        window->show();
    }
    else {
        QMessageBox::information(this, "Ошибка", "Неверный пин!");
    }
}

