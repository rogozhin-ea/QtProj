#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "cryptoprotection.h"

SecondWindow::SecondWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondWindow)
{
    ui->setupUi(this);
    //Путь к файлу с зашифрованным содержимым
    QString file = "D:/QtProj/Lab1/Lab1/credentials.json";

    cryptoprotection test;

    //Шифрование файла
    test.encryption(file);

    //Расшифровка файла
    QString data = test.decryption(file, (unsigned char *)key.data());
    doc = QJsonDocument::fromJson(QByteArray(data.toUtf8()),&docError);
    QJsonValue credentials = doc.object().value("credentials");

    //Копирование содержимого файла во временный объект
    for (int i = 0; credentials[i].isObject(); i++) {
        QJsonObject tempObject = credentials[i].toObject();
        QVector<QString> tempVector = {tempObject["site"].toString(), tempObject["data"].toString()};
        dataVector.push_back(tempVector);
    }

    //Создание полей для определения сигнала нажатия
    QSignalMapper *loginMapper = new QSignalMapper(this);
    QSignalMapper *passwordMapper = new QSignalMapper(this);

    QObject::connect(loginMapper,SIGNAL(mappedInt(int)),this,SLOT(show_data_log(int)));
    QObject::connect(passwordMapper,SIGNAL(mappedInt(int)),this,SLOT(show_data_pass(int)));

    for (int i = 0; i < dataVector.length(); i++) {
        QHBoxLayout *layout = new QHBoxLayout;

        //Создание формы с названием сайта
        QLineEdit *site = new QLineEdit();
        site->setReadOnly(true);
        site->setText(dataVector[i][0]);
        site->setStyleSheet("border: none;");

        //Создание формы с логином
        QLineEdit *login = new QLineEdit();
        login->setEchoMode(QLineEdit::Password);
        login->setReadOnly(true);
        login->setStyleSheet("border: none;");
        login->setText("1234567890");

        //Создание формы с паролем
        QLineEdit *password = new QLineEdit();
        password->setEchoMode(QLineEdit::Password);
        password->setReadOnly(true);
        password->setStyleSheet("border: none;");
        password->setText("1234567890");

        //Создание кнопок для копирования
        QPushButton *loginCopy = new QPushButton("copy");
        QPushButton *passwordCopy = new QPushButton("copy");

        loginCopy->setFixedSize(70,20);
        //Передача id строки логина и пароля
        loginMapper->setMapping(loginCopy,i);
        connect(loginCopy,SIGNAL(clicked()),loginMapper,SLOT(map()));

        passwordCopy->setFixedSize(70,20);
        //Передача id строки логина и пароля
        passwordMapper->setMapping(passwordCopy,i);
        connect(passwordCopy,SIGNAL(clicked()),passwordMapper,SLOT(map()));

        //Добавление всех виджетов в созданный слой
        layout->addWidget(site);
        layout->addWidget(login);
        layout->addWidget(loginCopy);
        layout->addWidget(password);
        layout->addWidget(passwordCopy);

        //Создание виджета для слоя
        QWidget *widget = new QWidget();
        widget->setObjectName(QString(dataVector[i][0]));
        widget->setLayout(layout);

        widgetList.push_back(widget);

    }

    for (int var = 0; var < widgetList.length(); ++var) {
        ui->verticalLayout->addWidget(widgetList[var]);
    }
}

SecondWindow::~SecondWindow()
{
    delete ui;
}

int SecondWindow::show_data_log(int id) {

    //Копирование зашифрованной строки с логином и паролем
    QString encryptedVector = dataVector[id][1];
    QByteArray encryptedVectorByte = QByteArray::fromBase64(encryptedVector.toUtf8());

    cryptoprotection decryptedVector;

    unsigned char decrypt[256] = {0};
    int decryptedVectorLen;

    //Расшифровка строки с логином и паролем
    decryptedVectorLen = decryptedVector.decrypt((unsigned char*)encryptedVectorByte.data(),encryptedVectorByte.length(),(unsigned char *)key.data(),decryptedVector.iv,decrypt);

    //Расшифрованные данные переводим в байты
    QByteArray decryptedVectorByte =  QByteArray::fromRawData((const char*)decrypt, decryptedVectorLen);

    //Создание json-документа
    QJsonDocument vectorFile = QJsonDocument::fromJson(decryptedVectorByte, &docError);
    QJsonObject vectorJson = vectorFile.object();

    //Копирование логина из файла в буфер обмена
    QClipboard* pcb = QApplication::clipboard();
    pcb->setText(vectorJson["login"].toString(), QClipboard::Clipboard);

    return 0;
}

int SecondWindow::show_data_pass(int id) {

    //Копирование зашифрованной строки с логином и паролем
    QString encryptedVector = dataVector[id][1];
    QByteArray encryptedVectorByte = QByteArray::fromBase64(encryptedVector.toUtf8());

    cryptoprotection decryptedVector;

    unsigned char decrypt[256] = {0};
    int decryptedVectorLen;

    //Расшифровка строки с логином и паролем
    decryptedVectorLen = decryptedVector.decrypt((unsigned char*)encryptedVectorByte.data(),encryptedVectorByte.length(),(unsigned char *)key.data(),decryptedVector.iv,decrypt);
    QByteArray decryptedVectorByte =  QByteArray::fromRawData((const char*)decrypt, decryptedVectorLen);

    //Создание json-документа
    QJsonDocument vectorFile = QJsonDocument::fromJson(decryptedVectorByte, &docError);
    QJsonObject vectorJson = vectorFile.object();

    //Копирование пароля из файла в буфер обмена
    QClipboard* pcb = QApplication::clipboard();
    pcb->setText(vectorJson["password"].toString(), QClipboard::Clipboard);

    return 0;
}


void SecondWindow::on_find_textChanged(const QString &arg1)
{
    //Отображение всех элементов
    for (int var = 0; var < widgetList.length(); var++) {
        widgetList[var]->show();
    }

    //Скрыть отображение элементов без содержимого поиска
    for (int var = 0; var < widgetList.length(); var++) {
        if(!widgetList[var]->objectName().contains(arg1)) {
            widgetList[var]->hide();
        }
    }
}
