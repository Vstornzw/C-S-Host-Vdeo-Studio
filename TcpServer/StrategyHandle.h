#ifndef STRATEGYHANDLE_H
#define STRATEGYHANDLE_H

#include <QObject>
#include "protocol.h"
#include <QTcpSocket>
#include "Ihandler.h"


class StrategyHandle
{
public:
    StrategyHandle();
    void Handle(QTcpSocket *socket, const Protocol &p);
};

#endif // STRATEGYHANDLE_H
