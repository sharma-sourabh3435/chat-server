#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_socketServer)
    {
        m_socketServer->close();
        delete m_socketServer;
    }
    qDeleteAll(m_sockets.begin(), m_sockets.end());
}

void MainWindow::init()
{
    ui->textEdit->setReadOnly(true);
    ui->portEdit->setText("8080");
    m_port = 8080;
    setConnections();
}

void MainWindow::setConnections()
{
    connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::connectServer);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::disconnectServer);
    connect(ui->portEdit, &QLineEdit::editingFinished, this, &MainWindow::portUpdated);
}

void MainWindow::connectServer()
{
    if(m_socketServer)
    {
        ui->textEdit->insertPlainText("Server already running\n");
        return;
    }
    m_socketServer = new QWebSocketServer("chat server", QWebSocketServer::NonSecureMode, this);

    if(m_socketServer->listen(QHostAddress::Any, m_port))
    {
        ui->textEdit->insertPlainText(QString("Server started on port %1\n").arg(m_port));

        connect(m_socketServer, &QWebSocketServer::newConnection, this, &MainWindow::onNewConnection);
    } else
    {
        ui->textEdit->insertPlainText("Failed to start server\n");
        delete m_socketServer;
        m_socketServer = nullptr;
    }
}

void MainWindow::disconnectServer()
{
    if(!m_socketServer)
    {
        ui->textEdit->insertPlainText("Server not running\n");
        return;
    }

    foreach (QWebSocket * client, m_sockets) {
        client->close();
    }
    m_sockets.clear();

    m_socketServer->close();
    ui->textEdit->insertPlainText("server disconnected\n");

    delete m_socketServer;
    m_socketServer = nullptr;
}

void MainWindow::portUpdated()
{
    bool ok;
    qint16 port = (qint16) ui->portEdit->text().toInt(&ok);
    if(ok && port > 0)
    {
        m_port = port;
    } else
    {
        ui->textEdit->insertPlainText("invalid port number.\n");
        ui->portEdit->setText(QString::number(m_port));
    }
}

void MainWindow::onNewConnection()
{
    QWebSocket *socket = m_socketServer->nextPendingConnection();
    if(socket)
    {
        m_sockets.append(socket);

        connect(socket, &QWebSocket::textMessageReceived, this, &MainWindow::onMessageReceived);
        connect(socket, &QWebSocket::disconnected, this, &MainWindow::onClientDisconnected);

        ui->textEdit->insertPlainText(QString("new client connected. total clients: %1\n").arg(m_sockets.size()));
        socket->sendTextMessage("Welcome to the chat server!");
    }
}

void MainWindow::onMessageReceived(const QString &message)
{
    QWebSocket *sender = qobject_cast<QWebSocket *>(this->sender());

    foreach (QWebSocket *client, m_sockets)
    {
        if(client != sender)
        {
            client->sendTextMessage(message);
        }
    }
}

void MainWindow::onClientDisconnected()
{
    QWebSocket * client = qobject_cast<QWebSocket *>(sender());
    if(client)
    {
        m_sockets.removeAll(client);
        client->deleteLater();
        ui->textEdit->insertPlainText(QString("client disconnected. total clients: %1\n").arg(m_sockets.size()));
    }
}
