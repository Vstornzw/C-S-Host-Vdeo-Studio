#include "RoomManage.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include "Room.h"
#include "DataBase.h"
#include "RoomVector.h"
#include "NewUserVector.h"
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
      break;
  case Protocol::CloseRoom:
      this->CloseHostRoom(socket,p);
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

void RoomManage::CreateHostRoom(QTcpSocket *socket, const Protocol &p) {
  DataBase *instance = DataBase::GetInstance();
  instance->CreatConnection();
  double money = 0.0f;
  int level = 0;
  qDebug()<<"aaa";
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

  qDebug()<< "随机产生的IP地址:" << ip;

  camera_port = 6000 + qrand()%2000;//摄像头端口
  audio_port = camera_port + qrand()%10;//音频端口

  //创建一个主播房间实例
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
  socket->write(pRet.pack());
  //直播间创建成功，直播间大厅自动刷新已存在的直播间
  NewUserVector* Ninstance = NewUserVector::GetInstance();//这是一个单例，因此只有一个对象
  QVector<SocketHandle*>& sh =  Ninstance->GetAllSockets();
  for(QVector<SocketHandle*>::iterator it = sh.begin(); it != sh.end(); it++) {
    QTcpSocket* socketTmp = (*it)->GetSocket();//-------------------------------------这里有待商榷
    this->RoomListHandle(socketTmp,p);
  }

}

//=====账户登录,下线，注销后，刚申请完主播房间后 刷新 账户 左边的正直播的房间=====//
void RoomManage::RoomListHandle(QTcpSocket *socket, const Protocol &p) {
  QString name =p["user_name"].toString();
  qDebug() << "房间名列表-服务器接收账户：" << name;

  /*构造一个返回在room房间里面的直播间列表的包*/
  Protocol pRet(Protocol::RoomList);

  RoomVector* rv = RoomVector::GetInstance();
  QVector<Room> chat_rooms = rv->GetAllChat();

  int count = 1;
  for(QVector<Room>::iterator it = chat_rooms.begin(); it!=chat_rooms.end(); it++) {
    pRet[QString::number(count)] = it->GetHostRoomName();
    count++;
  }
  pRet["result"] = "RoomListTrue";
  socket->write((pRet.pack()));
  return;
}


//====关闭直播账户====//
void RoomManage::CloseHostRoom(QTcpSocket *socket, const Protocol &p) {
  QString host_name = p["host_name"].toString();//房间名
  QString user_name = p["user_name"].toString();//账户名
  Protocol pRet(Protocol::CloseRoom);//-------------------------------------------------------------
  Protocol pUse(Protocol::UserLset);
  qDebug() <<"服务器接收到房间名/账户名:" <<host_name<<user_name;

  RoomVector *rv = RoomVector::GetInstance();
  QVector<Room>& chatrooms = rv->GetAllChat();
  for(QVector<Room>::iterator it = chatrooms.begin(); it!=chatrooms.end(); it++) {
    if(it->GetHostRoomName() == host_name) {//找到主播房间名
      QVector<Account_t>& per = it->GetAudience();//找到这个主播房间下对应的所有观众（放在容器里面）

      //判断是否为主播关闭窗口：QVector<Room>存了所有关于主播房间的类，找到这个含有房间名/账户名的类，与传过来的user_name 对比
      if(it->GetHostPreson() == user_name) {
        for(QVector<Account_t>::iterator ip = per.begin(); ip!= per.end(); ip++) {
          //下面两行的意思是(要关闭的主播房间)里面所的观众窗口全部退出观看窗口，返回到大厅
          pRet["result"] = "CloseHostRoomTrue";
          ip->socket->write(pRet.pack());//遍历，所有游客关闭窗口

          this->AccountRefresh(ip->socket,ip->name);
        }

        //防止直播间只有主播自己的情况，无法进入上述for循环，则直接读到如下语句
        pRet["result"] = "CloseHostRoomTrue";
        it->GetSocket()->write(pRet.pack());
        this->AccountRefresh(it->GetSocket(),it->GetHostPreson());

        //当主播退出该直播房间后，将该直播房间从QVector<Room>容器中删除
        rv->EraseRoom(*it);

        //主播房间关闭成功，直播间大厅自动刷新已存在的主播间
        NewUserVector* instance = NewUserVector::GetInstance();
        QVector<SocketHandle*>& sh = instance->GetAllSockets();
        for(QVector<SocketHandle*>::iterator it = sh.begin(); it != sh.end(); it++) {
          QTcpSocket* socketTmp = (*it)->GetSocket();
          this->RoomListHandle(socketTmp,p);
        }
        break;
      } else {

      }

    }

  }
  return ;
}

void RoomManage::AccountRefresh(QTcpSocket *socket, QString name) {
  DataBase* instance = DataBase::GetInstance();
  instance->CreatConnection();

  Protocol pRet(Protocol::RoomListPer);

  QSqlQuery query;
  double money = 0.0f;
  int level = 0;
  query.prepare("select * from tb_user where user_name = :user_name");
  query.bindValue(":user_name", name);
  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();
    pRet["result"] = "RoomListPerFalse";
  } else {
    while(query.next()) {
      money = query.value(4).toDouble();
      level = query.value(5).toInt();
    }
    pRet["result"] = "RoomListPerTrue";
    pRet["user_name"] = name;
    pRet["money"] = money;
    pRet["score"] = level;
  }
  instance->RemoveConnection();
  socket->write(pRet.pack());
  return ;
}

























