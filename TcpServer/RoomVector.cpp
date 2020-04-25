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
    //确保一个客户端只能创建一个直播间,并且不能和已存在的房间重名
    if(room.GetHostRoomName() == it->GetHostRoomName() || room.GetSocket() == it->GetSocket()) {//判断条件就两条
      return true;
    }
  }
  return false;//此处与其他不同，当为假的时候才能创建房间
}

bool RoomVector::InserRoom(const Room &room) {
  chat_room.push_back(room);
  return true;
}

QVector<Room>& RoomVector::GetAllChat() {
  return this->chat_room;
}

bool RoomVector::EraseRoom(const Room &room) {
  QVector<Room>::iterator it;
  for(it = chat_room.begin(); it != chat_room.end(); it++) {
    if(room.GetHostRoomName() == it->GetHostRoomName()) {
      chat_room.erase(it);//-------------------------------------------直接删除it
      return true;
    }
  }
  return false;
}

void RoomVector::InstertVisitor(const QString &host_name, Account_t visitor) {
  QVector<Room>::iterator it;
  for(it =chat_room.begin(); it!=chat_room.end(); it++) {
    if(host_name == it->GetHostRoomName()) {
      it->InserAudience(visitor);
    }
  }
}























