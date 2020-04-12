#ifndef ROOMM_H
#define ROOMM_H
#include "User.h"
#include <QTcpSocket>
#include <QString>
#include <QVector>



typedef struct Account {
  QString name;      //姓名
  QTcpSocket *socket;//套接字
}Account_t;

class Room
{
public:
  Room();
  //这个构造函数用于创建直播间
  Room(const QString& hostname,const QString& name,QTcpSocket *socket);

  Room(const QString& hostname,const QString& name,QTcpSocket *socket,
       QString address,int camera_port,int audio_port);

  //这里的返回值值得推敲-----------------------------------------------------------------------------------------------------
  const QString& GetHostRoomName() const { return this->host_name; }

  const QString& GetHostPreson() const {return this->host_person;}

  const QString& GetAddress() const {return this->address;}

  int GetCameraPort() const {return this->camera_port;}

  int GetAudioPort() const {return this->audio_port;}

  QVector<Account_t>& GetAudience() {return this->audience;}

  void InserAudience(Account_t per) { this->audience.push_back(per);}

  QTcpSocket * GetSocket() const { return this->socket;}





private:
  QString host_name;          //房间名
  QString host_person;        //主播，直播间与主播绑定，金钱和等级会被记录
  QVector<Account_t> audience;  //直播间观看者
  QString address;            //组播地址
  int camera_port;            //视频端口
  int audio_port;             //音频端口
  QTcpSocket *socket;         //房间套接字
};

#endif // ROOMM_H
