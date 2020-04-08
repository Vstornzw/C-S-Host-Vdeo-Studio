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
    this->RegistHandle(socket,p);//注意这里和30行区别
    break;
  case Protocol::Login:
    qDebug()<<"测试2";
    this->LoginHandle(socket,p);
    break;
  case Protocol::QuitClient:
    this->QuitAcount(socket,p);
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

//==============账户登录==============//

void AcountManage::LoginHandle(QTcpSocket *socket, const Protocol &p) {

  DataBase* instance = DataBase::GetInstance();
  instance->CreatConnection();
  //账户参数
  double moneytmp = 0.0;
  int leveltmp = 0;
  QString name = p["user_name"].toString();
  QString pswd = p["user_pwd"].toString();

  //定义一个数据查询的对象,执行命令的结果放在query
  QSqlQuery query;
  query.prepare("select * from tb_user where user_name = :user_name");
  query.bindValue(":user_name",name);

  bool ret = query.exec();
  if(!ret) {
    qDebug()<<query.lastError().text();

  } else {
    while(query.next()) {
      moneytmp = query.value(4).toDouble();
      leveltmp = query.value(5).toInt();
    }
  }
  instance->RemoveConnection();


  UserHandle user_handle;
  User user(name,pswd);
  Protocol pRet(Protocol::Login);

  //检查登录状态，正常情况下online=0,可以直接登录
  if(user_handle.CheckUserAndPwd(user)) {

    bool ret = user_handle.ModifyOnline(name,true);//调整在线状态
    if(ret) {
      qDebug()<<"服务器:账户登录成功";
      pRet["result"] = "LoginTrue";
      pRet["user_name"] = name;
      pRet["money"] = moneytmp;
      pRet["score"] = leveltmp;
    } else {
      qDebug()<<"服务器：账户登陆失败";
      pRet["result"] = "LoginFalse";
    }
  } else {
    pRet["result"] = "LoginFalse";
  }
  socket->write(pRet.pack());

  //登录成功，进入直播间大厅，自动刷新已存在的直播间

}

void AcountManage::QuitAcount(QTcpSocket *socket, const Protocol &p) {
  QString user_name = p["user_name"].toString();
  Protocol pRet(Protocol::QuitClient);
  UserHandle* user_handle = new UserHandle();
  bool ret = user_handle->HasUser(user_name);
  if(ret) {
    ret = user_handle->ModifyOnline(user_name,false);
    if(!ret) {
      qDebug()<<"关闭客户端失败";
      pRet["result"] = "QuitRoomFalse";
    } else {
      pRet["result"] = "QuitRoomTrue";
    }
  }
  socket->write(pRet.pack());
  //下线成功，直播间大厅自动刷新已存在的直播间

}





















