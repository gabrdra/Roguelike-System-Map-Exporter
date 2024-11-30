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
    void print_map_data(MapData map_data);
    void print_validated_map_data(MapData map_data);
};

#endif