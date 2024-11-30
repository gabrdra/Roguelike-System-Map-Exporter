#include "BacktrackData.h"
#include <unordered_set>

BacktrackData::BacktrackData(std::string _parent_room_name, std::shared_ptr<Room> _room, std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Connection>>> _passages_attempts)
    : parent_room_name(_parent_room_name), room(_room), passages_attempts(_passages_attempts) {}
BacktrackData::BacktrackData(std::string _parent_room_name, std::shared_ptr<Room> _room)
    : parent_room_name(_parent_room_name), room(_room) {}