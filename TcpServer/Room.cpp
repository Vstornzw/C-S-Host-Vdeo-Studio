#include "Room.h"

Room::Room()
{

}

Room::Room(const QString &hostname, const QString &name, QTcpSocket *socket)
    :host_name(hostname),host_person(name),socket(socket) {

}

Room::Room(const QString &hostname, const QString &name, QTcpSocket *socket, QString address, int camera_port, int audio_port)
    :host_name(hostname),host_person(name),socket(socket),address(address),camera_port(camera_port),audio_port(audio_port){

}
