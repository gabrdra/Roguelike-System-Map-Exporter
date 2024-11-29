#include "BacktrackData.h"

BacktrackData::BacktrackData(std::string _parent_room_name, std::shared_ptr<Room> _room, std::unordered_map<std::string, std::vector<std::shared_ptr<Connection>>> _passages_attempts)
    : parent_room_name(_parent_room_name), room(_room), passages_attempts(_passages_attempts) {}