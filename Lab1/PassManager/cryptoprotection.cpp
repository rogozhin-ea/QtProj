#include "cryptoprotection.h"
#include <openssl/evp.h>

cryptoprotection::cryptoprotection(){

}

cryptoprotection::~cryptoprotection(){

}

int cryptoprotection::encryption(QString file) {

    unsigned char *key = (unsigned char *)"8b4066554730ddfaa0266346bdc1b202";
    unsigned char *iv = (unsigned char *)"0123456789012345";

    //Проверка на существование файла с зашифрованным содержимом
    QFile encryptedFile(file);
    if (encryptedFile.exists()) {
        return 0;
    }

    //Путь к файлу с незашифрованным содержимом
    QFile decryptedFile;
    decryptedFile.setFileName("D:/QtProj/Lab1/Lab1/open_credentials.json");

    decryptedFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!decryptedFile.isOpen()) {
        return 0;
    }

    QString decryptedData = decryptedFile.readAll();

    decryptedFile.close();

    doc = QJsonDocument::fromJson(QByteArray(decryptedData.toUtf8()), &docError);
    json = doc.object();
    credentials = json.value("credentials");

    QJsonObject secondChipherJson;
    QJsonArray secondChipherCredentials;
    QJsonObject secondChipherData;

    for (int i = 0; credentials[i].isObject(); i++) {
        //Создание объекта из значений в массиве
        QJsonObject obj = credentials[i].toObject();
        //Объединение логина и пароля в одну строку
        QString secondChipherString = QString("{") + "\"login\":\"" +  obj["login"].toString() + '"' + ',' + "\"password\":\"" + obj["password"].toString() + '"' + QString("}");

        //Перевод строки в байтовое значение
        QByteArray secondChiphrStringByte = secondChipherString.toUtf8();

        unsigned char chipherChar[512];
        //Шифрование строки с логином и паролем
        int chipherCharLen = encrypt((unsigned char *)secondChiphrStringByte.data(), secondChiphrStringByte.length(), key, iv, chipherChar);
        //Перевод результата в байтовое значение
        QByteArray chipherTextByte =  QByteArray::fromRawData((const char*)chipherChar, chipherCharLen).toBase64();

        secondChipherData.insert("site",obj["site"].toString());
        secondChipherData.insert("data",chipherTextByte.data());
        secondChipherCredentials.push_back(secondChipherData);

    }

    secondChipherJson.insert("credentials", secondChipherCredentials);
    //Json с открытым названием сайта и зашифрованными логином и паролем
    QJsonDocument chipherData(secondChipherJson);

    encryptedFile.open(QIODevice::WriteOnly | QIODevice::Text);
    encryptedFile.write(chipherData.toJson());
    encryptedFile.close();

    //Шифрование всего содержимого файла
    unsigned char *chipherFile = new unsigned char[chipherData.toJson().length() * 2];
    int chipherFileLen = encrypt((unsigned char *)chipherData.toJson().data(), chipherData.toJson().length(), key, iv, chipherFile);
    QByteArray chipherFileByte =  QByteArray::fromRawData((const char*)chipherFile, chipherFileLen).toBase64();

    delete [] chipherFile;

    encryptedFile.open(QIODevice::WriteOnly | QIODevice::Text);
    encryptedFile.write(chipherFileByte);
    encryptedFile.close();
}

int cryptoprotection::encrypt(unsigned char *text, int text_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int lenght;
    int cipherTextLenght;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        cryptoerror();

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        cryptoerror();

    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &lenght, text, text_len))
        cryptoerror();
    cipherTextLenght = lenght;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + lenght, &lenght))
        cryptoerror();
    cipherTextLenght += lenght;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return cipherTextLenght;
}

QString cryptoprotection::decryption(QString file, unsigned char *key) {

    QFile encryptedFile;

    encryptedFile.setFileName(file);

    encryptedFile.open(QFile::ReadOnly);

    if (!encryptedFile.isOpen()) {
        return 0;
    }

    //Считывание из файла байтовых данных
    QByteArray fileEncode = QByteArray::fromBase64(encryptedFile.readAll());
    //Создание буфер из считанных байтов
    QBuffer chipherBuffer(&fileEncode);
    chipherBuffer.open(QBuffer::ReadOnly);

    //Инициализирующий вектор
    unsigned char *iv = (unsigned char *)"0123456789012345";

    //Строка, содержащая расшифрованные данные
    QString decryptedString;
    //Создание структуры
    EVP_CIPHER_CTX *ctx;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        cryptoerror();

    //Инициализация созданной структуры
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        cryptoerror();

    //Количество считанных из буфера символов
    int bufferNum = 0;
    //Количество сдвигов курсора буфера
    int cursorShift = 0;
    //Длина расшифрованного текста
    int decryptedStringLen = 0;

    while(true) {
        char cipherChar[256] = {0}; //
        unsigned char decryptedChar[512] = {0};

        //Сдвиг курсора буфера
        chipherBuffer.seek(cursorShift);

        //Считывание из буфера блоков по 256 символов
        bufferNum = chipherBuffer.read(cipherChar,256);

        //Расшифровка блоков
        if(1 != EVP_DecryptUpdate(ctx, decryptedChar, &decryptedStringLen, (unsigned char*)cipherChar, bufferNum))
            cryptoerror();

        //Строка с текущим блоком расшифрованных символов
        QString currentBlock;

        if (bufferNum < 256) {
            int temp;
            if(1 != EVP_DecryptFinal_ex(ctx, decryptedChar + decryptedStringLen, &temp))
                cryptoerror();

            //Вывод расшифрованных символов в строку
            for (int var = 0; var < decryptedStringLen + temp; var++) {
                currentBlock.append((char)decryptedChar[var]);
            }

            EVP_CIPHER_CTX_free(ctx);

            decryptedChar[decryptedStringLen] = '\0';
            //Добавление текущего блока в строку
            decryptedString.append(currentBlock);

            break;
        }


        //Вывод расшифрованных символов в строку
        for (int var = 0; var < decryptedStringLen; var++) {
            currentBlock.append((char)decryptedChar[var]);
        }

        //Сдвиг курсора
        cursorShift += 256;
        //Добавление текущего блока в строку
        decryptedString.append(currentBlock);

    }
    return decryptedString;
}

int cryptoprotection::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *decryptext)
{
    EVP_CIPHER_CTX *ctx;

    int lenght;
    int decryptTextLenght;

    if(!(ctx = EVP_CIPHER_CTX_new()))
       cryptoerror();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        cryptoerror();

    if(1 != EVP_DecryptUpdate(ctx, decryptext, &lenght, ciphertext, ciphertext_len))
        cryptoerror();
    decryptTextLenght = lenght;

    if(1 != EVP_DecryptFinal_ex(ctx, decryptext + lenght, &lenght))
       cryptoerror();
    decryptTextLenght += lenght;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return decryptTextLenght;
}

int cryptoprotection::cryptoerror(void)
{
    return 1;
}
