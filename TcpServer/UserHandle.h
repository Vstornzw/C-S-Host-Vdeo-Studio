#ifndef USERHANDLE_H
#define USERHANDLE_H
#include "User.h"

class UserHandle
{
public:
    UserHandle();

    bool HasUser(const QString &UserName);
    bool InsterUder(const User &user);

};

#endif // USERHANDLE_H
