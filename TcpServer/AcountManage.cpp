#include "AcountManage.h"
#include "protocol.h"
#include <QDebug>
#include <QString>
#include "UserHandle.h"
#include "DataBase.h"
#include "User.h"
#include <QSqlQuery>
#include <QSqlError>

AcountManage::AcountManage()
{

}

//==========接受客户端发来的包的函数，解决注册==========//
void AcountManage::Handle(QTcpSocket *socket, const Protocol &p) {

  switch(p.GetType()) {
  case Protocol::Regitst:
    this->RegistHandle(socket,p);
    break;

  default:
    break;
  }
}

//==============账户注册==============//
void AcountManage::RegistHandle(QTcpSocket *socket, const Protocol &p) {

  QString name = p["user_name"].toString();
  QString pswd = p["user_pwd"].toString();

  UserHandle user_handle;

  User user(name,pswd);

  //定义一个返回包regitst类型
  Protocol pRet(Protocol::Regitst);

  //判定数据库是否已注册该账户
  if(user_handle.HasUser(name)) {
    pRet["result"] = "registFalse";
  } else {
    //添加新账户到数据库
    bool ret = user_handle.InsterUder(user);

    if(ret) {
      qDebug()<<"服务器：账号注册成功";
      pRet["result"] = "registTrue";
    } else {
      qDebug() <<"服务器：账户注册失败";
      pRet["result"] = "registFalse";
    }
  }
  socket->write(pRet.pack());


}
