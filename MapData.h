#ifndef MAPDATA_H
#define MAPDATA_H

#include <string>
#include <unordered_map>
#include "LevelData.h"

class MapData {
public:
    std::unordered_map<std::string, std::shared_ptr<LevelData>> levels;
    std::string passages_holder_name;

    MapData(std::unordered_map<std::string, std::shared_ptr<LevelData>> _levels = {}, std::string _passages_holder_name = "Passages");
};
#endif