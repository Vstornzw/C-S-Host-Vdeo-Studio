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

    QSqlQuery query;
    query.prepare("select *from tb_user where user_name = :user_name");
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

  QSqlQuery query;
  query.prepare("insert into tb_user(user_name,user_pwd)"
                 " values(?, ?)");
  query.addBindValue(user.GetUserName());
  query.addBindValue(user.GetUserPswd());

  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();
  }
  instance->RemoveConnection();
  return ret;
}
