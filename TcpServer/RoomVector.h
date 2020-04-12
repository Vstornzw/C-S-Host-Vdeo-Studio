#ifndef ROOMVECTOR_H
#define ROOMVECTOR_H

#include <QVector>
#include "Room.h"

class RoomVector
{
public:
  static RoomVector* GetInstance();
  /*检验是否已经创建直播间*/
  bool CheckRoom(const Room& room);

  bool InserRoom(const Room& room);


private:
  RoomVector();

  static RoomVector* instance;

  QVector<Room> chat_room;

};

#endif // ROOMVECTOR_H
