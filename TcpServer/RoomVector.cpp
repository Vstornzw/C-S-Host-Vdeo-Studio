#include "RoomVector.h"

RoomVector::RoomVector()
{

}
RoomVector* RoomVector::instance =0;
RoomVector* RoomVector::GetInstance() {
  if(0 == instance) {
    instance = new RoomVector();
  }
  return instance;
}

bool RoomVector::CheckRoom(const Room &room) {
  QVector<Room>::iterator it;
  for(it = chat_room.begin(); it!=chat_room.end(); it++) {
    //确保一个客户端只能创建一个直播间
    if(room.GetHostRoomName() == it->GetHostRoomName() || room.GetSocket() == it->GetSocket()) {
      return true;
    }
  }
  return false;//此处与其他不同，当为假的时候才能创建房间
}

bool RoomVector::InserRoom(const Room &room) {
  chat_room.push_back(room);
  return true;
}
