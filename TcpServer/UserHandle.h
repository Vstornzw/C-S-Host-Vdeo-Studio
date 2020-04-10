#ifndef USERHANDLE_H
#define USERHANDLE_H
#include "User.h"

class UserHandle
{
public:
    UserHandle();

    bool HasUser(const QString &UserName);
    bool InsterUder(const User &user);

    bool CheckUserAndPwd(const User& user);
    bool ModifyOnline(const QString& name, bool online);

    bool DeleteUser(QString name);

};

#endif // USERHANDLE_H
