#ifndef CRYPTOPROTECTION_H
#define CRYPTOPROTECTION_H

#include <openssl/evp.h>
#include "QDebug"
#include "QByteArray"
#include "QBuffer"
#include "QFile"
#include "QJsonParseError"
#include "QJsonObject"
#include "QJsonArray"

class cryptoprotection
{
public:
    QJsonDocument doc;
    QJsonObject json;
    QJsonParseError docError;
    QJsonValue credentials;
    unsigned char *iv = (unsigned char *)"0123456789012345";

    cryptoprotection();
    ~cryptoprotection();
    int encryption(QString file);
    int encrypt(unsigned char *text, int text_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext);
    QString decryption(QString file, unsigned char *key);
    int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *iv, unsigned char *decryptext);
    int cryptoerror(void);
};

#endif // CRYPTOPROTECTION_H
