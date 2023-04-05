#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "secondwindow.h"
#include "QCryptographicHash"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    const QByteArray hash = "8b4066554730ddfaa0266346bdc1b202";
    Ui::MainWindow *ui;
    SecondWindow *window;
};
#endif // MAINWINDOW_H
