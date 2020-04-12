#ifndef ROOMMANAGE_H
#define ROOMMANAGE_H
#include "Ihandler.h"

#include "DataBase.h"
#include "RoomVector.h"
#include "NewUserVector.h"
class RoomManage :public Ihandler
{
public:
  RoomManage();
  ~RoomManage(){}
  void Handle(QTcpSocket *socket,const Protocol &p);


  //账户充值
  void ChargHandle(QTcpSocket *socket,const Protocol &p);
  //充值金额加到数据库
  bool UpdataMoney(const QString & name,double money);
  //创建主播房间
  void CreateHostRoom(QTcpSocket *socket,const Protocol &p);

};

#endif // ROOMMANAGE_H
