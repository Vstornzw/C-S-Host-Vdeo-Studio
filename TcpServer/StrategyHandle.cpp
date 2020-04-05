﻿#include "StrategyHandle.h"
#include "AcountManage.h"


StrategyHandle::StrategyHandle()
{

}

void StrategyHandle::Handle(QTcpSocket *socket, const Protocol &p) {

  Ihandler *i_handle = nullptr;
  switch (p.GetType()) {
  case Protocol::none:
    break;
  case Protocol::Regitst:
      i_handle = new AcountManage();//账户注册
  default:
    break;
  }

  //使用策略
  if(i_handle != nullptr) {
    i_handle->Handle(socket,p);
    delete i_handle;
  }
}
