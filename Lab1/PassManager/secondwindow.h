#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QStandardItem>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include "QSignalMapper"
#include <QJsonDocument>
#include <QJsonArray>
#include "QClipboard"
#include <openssl/evp.h>

namespace Ui {
class SecondWindow;
}

class SecondWindow : public QDialog
{
    Q_OBJECT



public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();
    QJsonDocument doc;
    QByteArray key = "8b4066554730ddfaa0266346bdc1b202";
    QJsonParseError docError;
    QFile file;

    QVector<QVector<QString>> dataVector;

    QList<QWidget *> widgetList;

private slots:

    int show_data_log(int id);
    int show_data_pass(int id);

    void on_find_textChanged(const QString &arg1);

private:
    Ui::SecondWindow *ui;
};

#endif // SECONDWINDOW_H
