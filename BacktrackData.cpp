#include "BacktrackData.h"

BacktrackData::BacktrackData(std::string _parent_room_name, Room _room, std::unordered_map<std::string, Connection> _connections)
    : parent_room_name(_parent_room_name), room(_room), connections(_connections) {}