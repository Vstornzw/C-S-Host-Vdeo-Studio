#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QHostAddress>
#include <QTcpSocket>


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::MainWindow)
{
  ui_->setupUi(this);
  tcp_server = new QTcpServer(this);
  connect(tcp_server,SIGNAL(newConnection()),this,SLOT(onNewConnection()));

  connect(ui_->pushButton,SIGNAL(clicked(bool)),this,SLOT(onBtnClicked()));
}


void MainWindow::onNewConnection() {

  QTcpSocket *tcp_socket = tcp_server->nextPendingConnection();

  QString ip = tcp_socket->peerAddress().toString();
  QString port = QString::number(tcp_socket->peerPort());
  QString info = "IP: " + ip + ", port: " + port + " 链接服务器";
  ui_->textEdit->append(info);

}

void MainWindow::onBtnClicked() {
  if(tcp_server->isListening()) {
    ui_->textEdit->append("已经启用监听");
    return;
  }
  bool ret = tcp_server->listen(QHostAddress::LocalHost,8888);
  if(!ret) {
      ui_->textEdit->append("监听失败");
  } else {
      ui_->textEdit->append("监听成功");
  }
}

MainWindow::~MainWindow() {
  delete ui_;
}


