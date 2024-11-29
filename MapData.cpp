#include "MapData.h"

MapData::MapData(std::unordered_map<std::string, std::shared_ptr<LevelData>> _levels, std::string _passages_holder_name)
:levels(_levels), passages_holder_name(_passages_holder_name){}
