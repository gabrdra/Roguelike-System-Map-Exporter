#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <memory>

class Room;

class Connection {
public:
    std::shared_ptr<Room> room;
    std::string connected_passage;

    Connection(std::shared_ptr<Room> _room, const std::string& _connected_passage);
    std::string to_string() const;
    bool equals(const Connection& other_connection) const;
};

#endif