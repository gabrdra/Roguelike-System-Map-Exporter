#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <forward_list>
#include "Room.h"
#include "Connection.h"

class LevelData {
public:
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms;
    std::shared_ptr<Room> starter_room;
    std::forward_list<std::forward_list<int>> possibilities;
    std::forward_list<std::pair<Connection, Connection>> connection_pairs;


    LevelData();
    LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room);
    
};
#endif