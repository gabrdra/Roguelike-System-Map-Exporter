#include "LevelData.h"
#include "Room.h"
#include "Connection.h"
#include <list>

LevelData::LevelData() {}
LevelData::LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room)
    : rooms(rooms), starter_room(starter_room) {}
LevelData::LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room, std::list<std::list<int>> possibilities, std::list<std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>> connection_pairs)
    : rooms(rooms), starter_room(starter_room), possibilities(possibilities), connection_pairs(connection_pairs) {}
