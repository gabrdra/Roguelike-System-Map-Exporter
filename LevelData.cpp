#include "LevelData.h"
#include "Room.h"
#include "Connection.h"

LevelData::LevelData() {}
LevelData::LevelData(std::map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room)
    : rooms(rooms), starter_room(starter_room) {}
