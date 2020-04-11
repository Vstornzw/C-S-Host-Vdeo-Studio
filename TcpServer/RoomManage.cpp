#include "RoomManage.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
RoomManage::RoomManage()
{

}
//=====接受客户端发来的包的函数，用于解决非注册，登录，下线，注销的其他部分=====//
void RoomManage::Handle(QTcpSocket *socket, const Protocol &p) {
  qDebug()<<"服务器收到的数据包类型"<<p.GetType();
  switch(p.GetType()) {
    case Protocol::Charge:
      this->ChargHandle(socket,p);
      break;
    default:
      break;

  }
}

void RoomManage::ChargHandle(QTcpSocket *socket, const Protocol &p) {

  //实例化对象，用于调用room_manage里面的UpdataMoney()函数
  RoomManage room_manage;

  Protocol pRet(Protocol::Charge);

  QString name = p["user_name"].toString();
  double money = p["money"].toDouble();
  /*将客户端打包的充值现金加到数据库里面*/
  bool ret = room_manage.UpdataMoney(name,money);

  if(!ret) {
    pRet["result"] = "ChargeMoneyFalse";
  } else {
    DataBase* instance = DataBase::GetInstance();
    instance->CreatConnection();
    QSqlQuery query;

    double mon= 0;
    int level =0;
    query.prepare("select * from tb_user where user_name = :user_name");
    query.bindValue(":user_name",name);
    bool ret1 = query.exec();
    if(!ret1){
      qDebug()<<query.lastError().text();
    } else {
      while(query.next()) {
        mon = query.value(4).toDouble();
        level = query.value(5).toInt();
      }
      pRet["result"] = "ChargeMoneyTrue";
      pRet["user_name"] = name;
      pRet["money"] = mon;
      pRet["score"] = level;
    }
    instance->RemoveConnection();
    socket->write(pRet.pack());
  }
}

bool RoomManage::UpdataMoney(const QString &name, double money) {
  DataBase *instance = DataBase::GetInstance();
  instance->CreatConnection();

  QSqlQuery query;
  query.prepare("select * from tb_user where user_name = :user_name");
  query.bindValue(":user_name",name);
  bool ret = query.exec();

  if(!ret) {
    qDebug()<<query.lastError().text();
  }
  /*下面是充值金额到数据库里面，首先要得到数据库里面的金额*/
  double Money;
  while(query.next()){
    Money = query.value(4).toDouble();
    qDebug()<<"数据库原有金额"<<Money;
  }
  QSqlQuery query1;
  query1.prepare("update tb_user set money =:money where user_name =:user_name");
  query1.bindValue(":user_name",name);
  /*将客户端打包的充值现金加到数据库里*/
  query1.bindValue(":money",Money+money);
  qDebug()<<Money+money;
  bool ret1 = query1.exec();
  if(!ret1) {
    qDebug()<<query1.lastError().text();
  }
  instance->RemoveConnection();
  return ret1;
}
