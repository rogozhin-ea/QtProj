#include "mainwindow.h"

#include <QApplication>
#include "QMessageBox"
#include "mainwindow.h"
#include "pinwindow.h"
#include "cripto.h"
#include <windows.h>
#include "QByteArray"

#define DEBUG_PROTECT_ON
#define CHECK_SUM_OFF

int main(int argc, char *argv[])
{
#ifdef CHECK_SUM_ON
    unsigned long long moduleBase = (unsigned long long)GetModuleHandle(NULL);
    unsigned long long text_segment_start = moduleBase + 0x1000;
    PIMAGE_DOS_HEADER pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
    PIMAGE_NT_HEADERS pINH = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + pIDH->e_lfanew);
    unsigned long long size_of_text = pINH->OptionalHeader.SizeOfCode;
    QByteArray text_segment_contents = QByteArray((char * )text_segment_start, size_of_text);
    QByteArray current_hash = QCryptographicHash::hash(text_segment_contents, QCryptographicHash::Sha256);
    QByteArray current_hash_base64 = current_hash.toBase64();
    const QByteArray ref_hash_base64 = QByteArray("7PSII2uJFlXPKaLBokV5eOaGrj+kzZmZS8bvMwxV3lQ=");
    bool result_checksum = (current_hash_base64==ref_hash_base64);
#else
    bool result_checksum = true;
#endif

    QApplication a(argc, argv);
    if(!result_checksum) {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),QObject::tr("Хэш-код приложения изменен!!!"));
        return 0;
    }

    //Проверка на отладку
#ifdef DEBUG_PROTECT_ON
    if (IsDebuggerPresent()) {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),QObject::tr("Запущено приложение - отладчик !!!"));
        return 0;
    }
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
