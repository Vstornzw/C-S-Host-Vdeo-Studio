#include "UserHandle.h"
#include "DataBase.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>


//= .cpp作用：作为AccountManage.cpp的辅助,执行数据库查找，注册，登录，删除，添加作用于数据库上 =//


UserHandle::UserHandle()
{

}

bool UserHandle::HasUser(const QString &name) {

    DataBase* instance = DataBase::GetInstance();
    instance->CreatConnection();

    QSqlQuery query;//定义一个数据查询的对象,执行命令的结果放在query
    query.prepare("select *from tb_user where user_name = :user_name");//数据库前置指令，与后面的指令配合使用；
    query.bindValue(":user_name",name);
    bool ret = query.exec();

    if(!ret) {
      qDebug()<<query.lastError().text();
    }
    ret = query.next();
    instance->RemoveConnection();
    return ret;
}

//====已确定没有注册，添加到数据库中===//
bool UserHandle::InsterUder(const User &user) {
  DataBase* instance = DataBase::GetInstance();
  instance->CreatConnection();

  QSqlQuery query;//定义一个数据查询的对象,执行命令的结果放在query
  query.prepare("insert into tb_user(user_name,user_pwd)"
                 " values(?, ?)");                                   //数据库前置指令，与后面的指令配合使用；
  query.addBindValue(user.GetUserName());
  query.addBindValue(user.GetUserPswd());

  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();
  }
  instance->RemoveConnection();
  return ret;
}

//=======验证账户，密码，online==========//
bool UserHandle::CheckUserAndPwd(const User& user) {

  DataBase* instance = DataBase::GetInstance();
  instance->CreatConnection();
  QSqlQuery query;
  bool ret;
  query.exec("select * from tb_user");//新建数据库操作索引；该仅表示该数据库语句执行结果
  while(query.next()) {               //指向查询结果的列表，重复执行相当于指针递增；遍历所有查询结果；
    QString ptr1 = query.value(1).toString();
    QString ptr2 = query.value(2).toString();
    int online = query.value(3).toInt();//数据库里面是NULL，也可以转成整型0
    if(user.GetUserName()==ptr1 && user.GetUserPswd()==ptr2 && online==0) {//QString类型是直接 等于
      ret = true;
      return ret;
    }
  }
  ret = false;
  return ret;
}

//=============修改在线状态============//
//===========需要遍历数据库找到name对应online的状态进行修改=========//
bool UserHandle::ModifyOnline(const QString& name, bool online) {

  DataBase* instance = DataBase::GetInstance();
  instance->CreatConnection();

  QSqlQuery query;
  //规定：数据库代码后面不可以有注释 :online不可以分开； onlin后面和whiere前面都要有空格
  query.prepare("update tb_user set online = :online "
                " where user_name = :user_name");
  query.bindValue(":user_name",name);
  //修改在线状态 设置成true，在之后的判断中false被转化成0，true转换成1
  query.bindValue(":online",online);

  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();
  }

  instance->RemoveConnection();
  return ret;

}
