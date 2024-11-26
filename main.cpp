#include "main.h"
#include "FileHandler.h"

using json = nlohmann::json;

int main()
{
    Main main;
    std::string plugin_data_path = "save_data.json";
    FileHandler fileHandler;
    MapData map_data = fileHandler.load_map_data(plugin_data_path);
    main.print_map_data(map_data);
    return 0;
}


//print map
void Main::print_map_data(MapData map_data) {
    for (const auto& [level_name, level] : map_data.levels) {
        std::cout << "Level: " << level_name << std::endl;
        for (const auto& [room_name, room] : level.rooms) {
            std::cout << "Room: " << room_name << std::endl;
            for (const auto& [passage_name, connections] : room->passages) {
                std::cout << "Passage: " << passage_name << std::endl;
                for (const auto& conn : connections) {
                    std::cout << "    " << conn->to_string() << std::endl;
                }
            }
        }
    }
}