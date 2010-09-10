#include <QtGui/QApplication>
#include <QtSql>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QApplication a(argc, argv);
    QString strLibPath(QDir::toNativeSeparators(QApplication::applicationDirPath())+QDir::separator()+
    "plugins");
    a.addLibraryPath(strLibPath);
    MainWindow w;
    w.show();
    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(&w, "addConnection", Qt::QueuedConnection);
    return a.exec();
}
