#ifndef IHANDLER_H
#define IHANDLER_H

#include "protocol.h"
#include <QTcpSocket>

//接口类--->抽象的策略
//接口类：(属于抽象类)，用来表达一种能力或协议
class Ihandler {
public:
  Ihandler();

  //处理数据包
  virtual void Handle(QTcpSocket *socket ,const Protocol &p) = 0;//纯虚函数
  virtual ~Ihandler() {}

};

#endif // IHANDLER_H

//纯虚函数是一种特殊的虚函数，在许多情况下，在基类中不能对虚函数给出有意义的实现，
//而把它申明为纯虚函数，它的实现留给派生类去实现，这就是纯虚函数的作用！
//同时含有纯虚函数的类 称为抽象类，它不能生成对象！
