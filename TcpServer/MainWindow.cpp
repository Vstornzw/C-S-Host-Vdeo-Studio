#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QHostAddress>
#include <QTcpSocket>
#include "SocketHandle.h"



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

  // nextPendingConnection是干什么用的？--->获取连接上的套接字对象，也叫客户端的套接号，也叫客户端在服务器链接时的端口号，用于和客户端联系用的。
  // 例如在接收信号的槽内，接收对象用它来找到发送对象的
  QTcpSocket *tcp_socket = tcp_server->nextPendingConnection();//获取客户端的套接字信息

  QString ip = tcp_socket->peerAddress().toString();
  QString port = QString::number(tcp_socket->peerPort());
  QString info = "IP: " + ip + ", port: " + port + " 链接服务器";
  ui_->textEdit->append(info);

  //将与客户端的通信交给另一个对象处理
  SocketHandle *socket_handler = new SocketHandle(tcp_socket,this);//专门处理套接字的类





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


