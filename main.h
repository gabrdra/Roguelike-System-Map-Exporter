#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string>
#include <fstream>
#include "include\single_include\nlohmann\json.hpp"

#include "LevelData.h"
#include "Room.h"
#include "Connection.h"
#include "MapData.h"
class Main{
public:
    void generate_level_possibilities();
    bool load_plugin_data(const std::string& path);
private:
    MapData map_data;
    std::string current_level_name;
};

#endif