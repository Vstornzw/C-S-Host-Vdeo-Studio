#ifndef ROOMVECTOR_H
#define ROOMVECTOR_H

#include <QVector>
#include "Room.h"

//RoomVector是一个单例，管理主播创建房间的类，QVector<Room>是所有主播创立的房间的容器

class RoomVector
{
public:
  static RoomVector* GetInstance();
  /*检验是否已经创建直播间*/
  bool CheckRoom(const Room& room);

  bool InserRoom(const Room& room);

  QVector<Room>& GetAllChat();

  //主播退出直播间之后，将该房间从QVector<Room>容器中删除
  bool EraseRoom(const Room& room);


private:
  RoomVector();

  static RoomVector* instance;

  QVector<Room> chat_room;

};

#endif // ROOMVECTOR_H
