#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebSocket>
#include <QWebSocketServer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectServer();
    void disconnectServer();
    void portUpdated();
    void onNewConnection();
    void onMessageReceived(const QString &message);
    void onClientDisconnected();

private:
    void init();
    void setConnections();

    Ui::MainWindow *ui;
    qint16 m_port{0};
    QWebSocketServer *m_socketServer{nullptr};
    QList<QWebSocket *> m_sockets;
};
#endif // MAINWINDOW_H
