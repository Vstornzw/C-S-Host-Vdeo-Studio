#include "SocketHandle.h"
#include "protocol.h"
#include "StrategyHandle.h"
SocketHandle::SocketHandle(QObject *parent) : QObject(parent)
{

}

SocketHandle::SocketHandle(QTcpSocket *client_socket, QObject *parent)
  :QObject(parent),
  client_socket(client_socket) {

    connect(client_socket,SIGNAL(readyRead()),this,SLOT(onReadSlot()));
}

void SocketHandle::onReadSlot() {

  QByteArray byte = client_socket->readAll();

  Protocol p;
  int len = 0;
  while((len = p.unpack(byte)) > 0) {
    byte = byte.mid(len);//解析后面的数据
    //处理输出包
    qDebug() << "服务器接收数据："<< "类型:" << p.GetType() << "；账户名:"
             << p["user_name"].toString()<< "；密码:" << p["user_pwd"].toString()
             << "；金额:" <<p["money"].toDouble() << "；积分:" << p["score"].toInt();

    //调整策略
    StrategyHandle sh;
    sh.Handle(this->client_socket,p);
  }



}
