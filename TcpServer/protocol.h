#ifndef protocol_H
#define protocol_H
#include <QJsonObject>
#include <QJsonDocument>


class Protocol {

public:

  enum Type {
    none = 0,  //empyty

    Regitst,
    Login,
    QuitClient,
    DeleteUser,

    CreateRoom,
    RoomList,
    UserLset,  // 直播间列表
    JoinRoom,
    Chat,
    Gift,
    CloseRoom,
    Charge,
    RoomListPer  //大厅账户刷新
  };

  Protocol(Type type = none) : type(type) {}//构造函数
  void SetType(Type type) {this->type = type;}
  Type GetType() const {return this->type;}

  //支持中括号访问和添加
  QJsonValue operator [](const QString &key) const { return m_body[key];}
  QJsonValueRef operator [](const QString &key) { return m_body[key];}

  //打包
  QByteArray pack() {

    //总共数据包的大小就是 head + 实际数据
    QByteArray body = QJsonDocument(m_body).toJson();

    QByteArray head(8,0);//以0初始化8个字节空间
    //前四个字节为世界数据的大小
    *(int*)(head.data()) = body.size();
    //后四个字节是数据包类型
    *(Type*)(head.data() + 4) = this->type;
    return head+body;
  }

  //拆包
  int unpack(const QByteArray &byte) {

    if(byte.isEmpty()){
      return 0;
    }

    //实际数据长度
    int len = *(int*)(byte.data());
    if(len <= 0) {return 0;}
    //取出类型
    this->type = *(Type*)(byte.data() + 4);//类型强转为Type
    //取出实际数据
    QByteArray body = byte.mid(8, len);
    //保存到oboject中
    if(body.size() < 0) {
        return 0;
    }

    this->m_body = QJsonDocument::fromJson(body).object();
    return 8+ len;
  }

private:
  Type type;//数据包类型
  //数据主体
  QJsonObject m_body;//实际数据
};

#endif // protocol_H
