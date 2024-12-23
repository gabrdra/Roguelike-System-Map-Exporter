#include "Room.h"

Room::Room(const std::string& _name, const std::string& _scene_uid, bool _required, int _max_passes)
    : name(_name), scene_uid(_scene_uid), required(_required), max_passes(_max_passes) {}

Room::Room(const std::string& _name, const std::string& _scene_uid, bool _required, int _max_passes, std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> _passages)
    : name(_name), scene_uid(_scene_uid), required(_required), max_passes(_max_passes), passages(_passages) {}
Room::Room(const std::string& _name, const std::string& _scene_uid, std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> passages)
    : name(_name), scene_uid(_scene_uid), passages(passages) {}