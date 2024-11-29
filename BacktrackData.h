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
    std::shared_ptr<Room> room;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Connection>>> passages_attempts;
    BacktrackData(std::string _parent_room_name = "", std::shared_ptr<Room> _room, std::unordered_map<std::string, std::vector<std::shared_ptr<Connection>>> passages_attempts);
};

#endif