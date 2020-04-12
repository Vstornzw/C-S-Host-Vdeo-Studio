#include "RoomManage.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include "Room.h"
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
    case Protocol::CreateRoom:
      this->CreateHostRoom(socket,p);
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

void RoomManage::CreateHostRoom(QTcpSocket *socket, const Protocol &p) {
  DataBase *instance = DataBase::GetInstance();
  instance->CreatConnection();
  double money = 0.0f;
  int level = 0;

  QString name = p["user_name"].toString();
  QString hostname = p["room_name"].toString();

  QSqlQuery query;
  query.prepare("select * from tb_user where user_name = :user_name");
  query.bindValue(":user_name",name);
  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();
  } else {
    while(query.next()) {
      money = query.value(4).toDouble();
      level = query.value(5).toInt();
    }
  }
  instance->RemoveConnection();

  int camera_port;//用来视频的端口，代表16位的无符号整型
  int audio_port;//用来音频的端口，代表16位的无符号整型
  QString room_ip;//定义一个中间变量来保存随机值
  QString ip = "224.0.";//组播地址(224.0.+随机值)

  camera_port = qrand()%255; //随机0~255的随机数
  audio_port = qrand()%255;  //随机0~255的随机数

  /*数字转换成字符串*/
  room_ip = room_ip.setNum(camera_port);
  ip += room_ip + ".";

  room_ip = room_ip.setNum(audio_port);
  ip += room_ip;

  camera_port = 6000 + qrand()%2000;//摄像头端口
  audio_port = camera_port + qrand()%10;//音频端口

  //创建一个房间实例
  Room host_room(hostname,name,socket,ip,camera_port,audio_port);
  /*房间容器*/
  RoomVector *room_vector = RoomVector::GetInstance();

  Protocol pRet(Protocol::CreateRoom);
  /*判断函数：确保一个客户端只能创建一个直播间*/
  if(room_vector->CheckRoom(host_room)) {
    pRet["result"] = "CreateRoomFalse";
  } else {
    bool ret = room_vector->InserRoom(host_room);
    if(!ret){
      pRet["result"] = "CreateRoomFalse";
    } else {
      pRet["result"] = "CreateRoomTrue";
      pRet["hostname"] = hostname;
      pRet["money"] = QString::number(money,'f',2);//QString 型
      pRet["level"] = QString::number(level,10);//QString 型
      pRet["address"] = ip;
      pRet["camera_port"] = camera_port; //int 型
      pRet["audio_port"] = audio_port;
    }
  }
  //直播间创建成功，直播间大厅自动刷新已存在的直播间
  NewUserVector* Ninstance = NewUserVector::GetInstance();//这是一个单例，因此只有一个对象


}


































