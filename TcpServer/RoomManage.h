#ifndef ROOMMANAGE_H
#define ROOMMANAGE_H
#include "Ihandler.h"

#include "DataBase.h"
#include "RoomVector.h"
#include "NewUserVector.h"

#include "protocol.h"
#include <QTcpSocket>
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
  //所有账户左侧刷新已创建的主播房间名
  void RoomListHandle(QTcpSocket *socket, const Protocol &p);
  //关闭主播房间
  void CloseHostRoom(QTcpSocket *socket, const Protocol &p);
  //刷新账户在大厅的信息，只要游客送了礼，再回去大厅以后，金钱等数据肯定要改变
  void AccountRefresh(QTcpSocket *socket, QString name);

  //加入主播直播房间
  void JoinHostRoom(QTcpSocket *socket,const Protocol &p);
  //主播和游客的视频间访客栏刷新
  void VisitorBarRefresh(QTcpSocket *socket,const Protocol &p);
};

#endif // ROOMMANAGE_H
