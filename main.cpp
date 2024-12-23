#include "FileHandler.h"
#include <fstream>
#include "MapValidation.h"
#include "inipp.h"
#include <iostream>
#include <string>
#include <chrono>

int main()
{
    MapValidation mapValidation;
    std::string plugin_data_path = "";
    std::string exported_data_path = "";
    int max_instances = INT_MAX;
    inipp::Ini<char> ini;
    std::ifstream is("RogueSysConfig.ini");
    ini.parse(is);
    ini.interpolate();

    if (ini.errors.size() > 0) {
        std::cerr << "Error parsing config file" << std::endl;
        return 1;
    }

    inipp::get_value(ini.sections["File locations"], "PluginFile", plugin_data_path);
    inipp::get_value(ini.sections["File locations"], "ExportFile", exported_data_path);
    if (ini.sections["Test configuration"].find("MaxInstances") != ini.sections["Test configuration"].end()){
        inipp::get_value(ini.sections["Test configuration"], "MaxInstances", max_instances);
        if(max_instances < 1){
            std::cerr << "MaxInstances must be a positive integer" << std::endl;
            return 1;
        }
    }
    FileHandler fileHandler;
    MapData map_data = fileHandler.load_map_data(plugin_data_path);
    if (map_data.levels.empty()) {
        return 1;
    }
    std::cout << "Map data loaded" << std::endl;

    // auto start = std::chrono::high_resolution_clock::now(); // Start time

    MapData validated_map = mapValidation.validate_map(map_data, max_instances);

    // auto end = std::chrono::high_resolution_clock::now(); // End time
    // std::chrono::duration<double> duration = end - start;
    // std::cout << "Map validation took " << duration.count() << " seconds" << std::endl;

    bool save_result = fileHandler.save_validated_map_data(exported_data_path, validated_map);
    if (!save_result){
        return 1;
    }
    std::cout << "Exported data saved" << std::endl;
    //wait for user to press any key before exiting
    std::cin.get();
    return 0;
}


//print map
// void print_map_data(MapData map_data) {
//     for (const auto& [level_name, level] : map_data.levels) {
//         std::cout << "Level: " << level_name << std::endl;
//         for (const auto& [room_name, room] : level->rooms) {
//             std::cout << "Room: " << room_name << std::endl;
//             for (const auto& [passage_name, connections] : room->passages) {
//                 std::cout << "Passage: " << passage_name << std::endl;
//                 for (const auto& conn : std::get<std::vector<std::shared_ptr<Connection>>>(connections)) {
//                     std::cout << "    " << conn->to_string() << std::endl;
//                 }
//             }
//         }
//     }
// }
// void print_validated_map_data(MapData map_data) {
//     for (const auto& [level_name, level] : map_data.levels) {
//         std::cout << "Level: " << level_name << std::endl;
//         for (const auto& [room_name, room] : level->rooms) {
//             std::cout << "Room: " << room_name << std::endl;
//             for (const auto& [passage_name, connections] : room->passages) {
//                 std::cout << "Passage: " << passage_name << std::endl;
//                 for (const auto& conn : std::get<std::vector<std::shared_ptr<Connection>>>(connections)) {
//                     std::cout << "    " << conn->to_string() << std::endl;
//                 }
//             }
//         }
//         std::cout<<"ConnPairs: "<<std::endl;
//         //print connection pairs
//         for (const auto& conn_pair : level->connection_pairs) {
//             std::cout << "Connection pair: " << conn_pair.first->to_string() << " - " << conn_pair.second->to_string() << std::endl;
//         }
//         std::cout<<"Possibilities: "<<std::endl;
//         //print possibilities
//         for (const auto& possibility : level->possibilities) {
//             std::cout << "Possibility: ";
//             for (const auto& index : possibility) {
//                 std::cout << index << " ";
//             }
//             std::cout << std::endl;
//         }
//     }
// }