#ifndef SOCKETHANDLE_H
#define SOCKETHANDLE_H

#include <QObject>
#include <QTcpSocket>
#include <QVector>
#include "protocol.h"

class SocketHandle : public QObject
{
  Q_OBJECT
public:
  explicit SocketHandle(QObject *parent = 0);
  explicit SocketHandle(QTcpSocket *client_socket, QObject *parent = 0);

  QTcpSocket* GetSocket() {
    return this->client_socket;
  }

public slots:
  void onReadSlot();
private:
  QTcpSocket *client_socket;
};

#endif // SOCKETHANDLE_H
