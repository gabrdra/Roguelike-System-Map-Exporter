#ifndef BACKTRACKDATA_H
#define BACKTRACKDATA_H

#include <string>
#include <unordered_map>
#include "Room.h"
#include "Connection.h"

class BacktrackData
{

public:
    std::string parent_room_name;
    Room room;
    std::unordered_map<std::string, Connection> connections;
    BacktrackData(std::string _parent_room_name = "", Room _room = Room(), std::unordered_map<std::string, Connection> _connections = {});
};

#endif