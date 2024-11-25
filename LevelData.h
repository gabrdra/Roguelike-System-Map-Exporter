#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <map>
#include <string>
#include <memory>
#include <vector>
#include "Room.h"
#include "Connection.h"

class LevelData {
public:
    std::map<std::string, std::shared_ptr<Room>> rooms;
    std::shared_ptr<Room> starter_room;

    LevelData();
    LevelData(std::map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room);
    
};
#endif