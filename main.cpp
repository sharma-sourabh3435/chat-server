#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Chat Server");
    w.setWindowIcon(QIcon(":/server.png"));
    w.show();
    return a.exec();
}
