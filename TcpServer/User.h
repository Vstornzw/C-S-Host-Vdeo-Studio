#ifndef USER_H
#define USER_H
#include <QString>
#include <string>

//====1 在确定账户是否注册时候会用上====//
class User {
public:
  User() {}
  User(const QString &name, const QString &pswd)
      :user_name(name),user_pswd(pswd) {}

  int GetUserId() const {
    return this->user_id;
  }

  const QString& GetUserName() const {
    return this->user_name;
  }

  const QString& GetUserPswd() const {
    return this->user_pswd;
  }

  int GetOnline() const {
    return this->online;
  }

private:
  int user_id;
  QString user_name;
  QString user_pswd;
  int online;
  double money;
  int score;
};

#endif // USER_H
