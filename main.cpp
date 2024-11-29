#include "main.h"
#include "FileHandler.h"
#include "MapValidation.h"
using json = nlohmann::json;

int main()
{
    Main main;
    MapValidation mapValidation;
    std::string plugin_data_path = "save_data.json";
    FileHandler fileHandler;
    MapData map_data = fileHandler.load_map_data(plugin_data_path);
    std::cout << "Map data loaded" << std::endl;
    main.print_map_data(map_data);
    std::cout << std::endl << "Map data after validation" << std::endl;
    MapData validated_map = mapValidation.validate_map(map_data);
    main.print_map_data(validated_map);
    return 0;
}


//print map
void Main::print_map_data(MapData map_data) {
    for (const auto& [level_name, level] : map_data.levels) {
        std::cout << "Level: " << level_name << std::endl;
        for (const auto& [room_name, room] : level->rooms) {
            std::cout << "Room: " << room_name << std::endl;
            for (const auto& [passage_name, connections] : room->passages) {
                std::cout << "Passage: " << passage_name << std::endl;
                for (const auto& conn : std::get<std::vector<std::shared_ptr<Connection>>>(connections)) {
                    std::cout << "    " << conn->to_string() << std::endl;
                }
            }
        }
    }
}