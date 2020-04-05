#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>

//对数据库操作的封装
class DataBase
{

//public:
//    DataBase();

private:
  DataBase();//单例，在私有中构造
  static DataBase *instance;//创建数据库类(单例：保证数据库的唯一性)
  QSqlDatabase database;//数据库对象
public:
  static DataBase * GetInstance();//单例
  bool CreatConnection();//创建数据库链接
  void RemoveConnection();//移除数据库链接
};

#endif // DATABASE_H
