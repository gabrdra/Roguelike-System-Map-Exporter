#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <string>
#include "MapData.h"
class FileHandler
{
    public:
    MapData load_map_data(const std::string& file_path);
    bool save_validated_map_data(const std::string& file_path, const MapData& map_data);

};
#endif