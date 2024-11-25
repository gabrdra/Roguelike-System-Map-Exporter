#include "Connection.h"
#include "Room.h"

Connection::Connection(std::shared_ptr<Room> _room, const std::string& _connected_passage)
    : room(_room), connected_passage(_connected_passage) {}

std::string Connection::to_string() const {
    return room ? room->name + ": " + connected_passage : "null: " + connected_passage;
}

bool Connection::equals(const Connection& other_connection) const {
    return room && other_connection.room &&
           room->name == other_connection.room->name &&
           connected_passage == other_connection.connected_passage;
}