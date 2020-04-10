#ifndef ACOUNTMANAGE_H
#define ACOUNTMANAGE_H

#include "Ihandler.h"
class AcountManage : public Ihandler
{
public:
    AcountManage();
    ~AcountManage() {}//在这里写了析构函数，在后边的cpp文件就不需要写了

    void Handle(QTcpSocket *socket, const Protocol &p);
    void RegistHandle(QTcpSocket *socket ,const Protocol &p); //注册账户
    void LoginHandle(QTcpSocket *socket ,const Protocol &p);//登录账户
    void QuitAcount(QTcpSocket *socket, const Protocol &p);//账户下线
    void DeleteAcount(QTcpSocket *socket,const Protocol &p);//注销账户
};

#endif // ACOUNTMANAGE_H
