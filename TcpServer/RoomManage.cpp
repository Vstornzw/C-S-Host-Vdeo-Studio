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
  case Protocol::JoinRoom:
    this->JoinHostRoom(socket,p);
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
//对应着客户端中RoomListUi.cpp中HostRoomPla()函数//
void RoomManage::RoomListHandle(QTcpSocket *socket, const Protocol &p) {
  QString name =p["user_name"].toString();
  qDebug() << "房间名列表-服务器接收账户：" << name;

  /*构造一个返回在room房间里面的直播间列表的包*/
  Protocol pRet(Protocol::RoomList);

  RoomVector* rv = RoomVector::GetInstance();
  QVector<Room> chat_rooms = rv->GetAllChat();

  int count = 1;
  for(QVector<Room>::iterator it = chat_rooms.begin(); it!=chat_rooms.end(); it++) {
    pRet[QString::number(count)] = it->GetHostRoomName();//有五个直播间的话就返回五个包
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
        //确定是游客退出直播间，遍历该主播房间里面所有的游客
        for(QVector<Account_t>::iterator ip = per.begin(); ip != per.end(); ip++) {
          if(ip->socket == socket) {
            pRet["result"] = "CloseHostRoomTrue";
            socket->write(pRet.pack());                 //返回信号，关闭游客窗口
            this->AccountRefresh(socket,user_name);     //这是说通过遍历数据库，把最新的账户剩余多少钱传到大厅
            per.erase(ip);                              //从该直播间的"游客动态数组"中删除成员
            this->RoomListHandle(socket,p);             //退出直播间的游客返回大厅时候自动刷新左边栏的主播房间

            //下面几行都是将刷新该直播间还有多有游客，存到返回包里面
            int count = 1;
            for(QVector<Account_t>::iterator ia = per.begin(); ia != per.end(); ia++) {
              pUse[QString::number(count)] = ia->name;
              count++;
            }
            pUse["result"] = "UserListTrue";

            for(QVector<Account_t>::iterator ia = per.begin(); ia != per.end(); ia++) {
              qDebug()<<"关闭游客窗口，群成员列表："<<ia->name;
              //主播房间的所有游客，他们的ui界面上"群成员列表"刷新
              ia->socket->write(pUse.pack());

            }
            //主播房间的"群成员列表"刷新
            it->GetSocket()->write(pUse.pack());
            break;
          }
        }
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


void RoomManage::JoinHostRoom(QTcpSocket *socket, const Protocol &p) {

  DataBase *instance = DataBase::GetInstance();
  instance->CreatConnection();
  double money = 0.0f;
  int level = 0;
  QString host_room_name = p["host_room_name"].toString();
  QString name = p["user_name"].toString();

  QSqlQuery query;
  query.prepare("select * from tb_user where user_name = :user_name");
  query.bindValue(":user_name", name);
  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();
  } else {
    while(query.next()) {
      money = query.value(4).toDouble();
      level = query.value(5).toInt();
    }
  }
  qDebug()<<"服务器查询到金额/积分"<<money<<level;
  instance->RemoveConnection();

  RoomVector *room_vector = RoomVector::GetInstance();
  QVector<Room> chatrooms = room_vector->GetAllChat();

  Protocol pRet(Protocol::JoinRoom);
  QVector<Room>::iterator it;
  for(it = chatrooms.begin(); it != chatrooms.end(); it++) {
    if(host_room_name == it->GetHostRoomName()) {
      //此循环来防止重复加入
      QVector<Account_t> audience = it->GetAudience();
      for(QVector<Account_t>::iterator ip = audience.begin(); ip != audience.end(); ip++) {
        //遍历该房间的观众容器，如果容器里有名字与要加入的游客名字相同，则退出加入
        if(ip->name == name) {
          qDebug()<<"成员已经存在";
          return;
        }
      }

      Account_t visitor = {name,socket};//创建结构体，用来存放游客
      room_vector->InstertVisitor(host_room_name, visitor);//把游客加到对应的主播房间的游客容器里面去

      pRet["result"] = "JoinHostRoomTrue";
      pRet["host_room_name"] = host_room_name;
      pRet["user_name"] = it->GetHostPreson();
      pRet["audience"] = name;
      pRet["money"] = money;                                //游客账户余额
      pRet["level"] = level;
      pRet["address"] = it->GetAddress();
      pRet["camera_port"] = it->GetCameraPort();
      pRet["audio_port"] = it->GetAudioPort();
      socket->write(pRet.pack());
    }
  }

  //当加房间后，自动刷新直播间群成员列表
  this->VisitorBarRefresh(socket, p);
  return;
}

void RoomManage::VisitorBarRefresh(QTcpSocket *socket, const Protocol &p) {
  QString host_room_name = p["host_room_name"].toString();
  Protocol pRet(Protocol::UserLset);

  RoomVector *room_vector = RoomVector::GetInstance();
  QVector<Room> chatrooms = room_vector->GetAllChat();
  int count = 1;
  //在所有封装主播房间号的动态数组里找到该主播号
  for(QVector<Room>::iterator it = chatrooms.begin(); it != chatrooms.end(); it++) {
    if(host_room_name == it->GetHostRoomName() || socket == it->GetSocket()) {

      QVector<Account_t> users = it->GetAudience();
      qDebug() << "刷新群成员列表";
      //遍历该主播间放有全部观众的容器，每遍历出一个观众，就存到一个返回包里面
      for(QVector<Account_t>::iterator ia = users.begin(); ia != users.end(); ia++) {
        pRet[QString::number(count)] = ia->name;
        qDebug()<< ia->name<<";";
        count++;
      }

      pRet["result"] = "UserListTrue";

      //游客群成员(每个人)页面的列表刷新
      for(QVector<Account_t>::iterator ia = users.begin(); ia != users.end(); ia++) {
        ia->socket->write(pRet.pack());
      }
      // //主播群成员页面的列表刷新
      it->GetSocket()->write(pRet.pack());
    }
  }
  return;
}




















