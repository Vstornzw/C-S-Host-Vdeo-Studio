#ifndef NEWUSERVECTOR_H
#define NEWUSERVECTOR_H

#include <QVector>
#include "SocketHandle.h"

//设计这个NewUserVector作用：
//将客户端套接字放到一个容器里面，需要将容器放到一个单例类NewUserVector里面
//通过NewUserVector类的指针 指向GetAllSockets() 返回唯一一个容器的指针，
//并将客户端信息插入容器里面

class NewUserVector
{
public:
  static NewUserVector* GetInstance();

  //获取
  QVector<SocketHandle*>& GetAllSockets() {return this->sockets;}

  //添加
  void InsertSocket(SocketHandle *sh) { sockets.push_back(sh);}


private:
  NewUserVector();
  static NewUserVector *instance;
  //保存所有客户端套接字信息的动态数组
  QVector<SocketHandle*> sockets;

};

#endif // NEWUSERVECTOR_H
