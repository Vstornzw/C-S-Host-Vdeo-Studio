#include "DataBase.h"
#include <QDebug>

#include <QSqlError>
#include <QSqlQuery>

DataBase::DataBase()
{
  //链接数据库系统
  database = QSqlDatabase::addDatabase("QMYSQL");
  database.setHostName("localhost");
  database.setPort(3306);
  database.setUserName("root");
  database.setPassword("123456");
}

//静态成员初始化(单例）
DataBase* DataBase::instance = 0;
//产生单例：
DataBase* DataBase::GetInstance() {

  if(0 == instance) {
    instance = new DataBase();//注意！！！！
  }
  return instance;
}

//链接数据库
bool DataBase::CreatConnection() {

  database.setDatabaseName("stu2020");//设置要打开数据库文件路径
  bool ret = database.open();//打开数据库文件
  if(!ret) {
    qDebug()<<database.lastError().text();
    return false;
  }
  return true;
}

void DataBase::RemoveConnection() {
  database.close();
  database.removeDatabase("stu2020");//设置关闭的数据库路径
}



