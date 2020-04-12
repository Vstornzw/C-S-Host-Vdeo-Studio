#ifndef NEWUSERVECTOR_H
#define NEWUSERVECTOR_H

#include <QVector>
#include "SocketHandle.h"

class NewUserVector
{
public:
  static NewUserVector* GetInstance();
  //添加
  void InsertSocket(SocketHandle *sh) { sockets.push_back(sh);}

private:
  NewUserVector();
  static NewUserVector *instance;
  //保存所有客户端套接字信息的动态数组
  QVector<SocketHandle*> sockets;

};

#endif // NEWUSERVECTOR_H
