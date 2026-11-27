//
// Created by nakamurasama072 on 11/23/26.
//
module;
#include <filesystem>
#include <iostream>
#include <sqlite3.h>
#include <memory>

module dbwrapper;

// Pimpl: private implementation
struct sqliteops::DatabaseSession::DBImpl {
    sqlite3* db_inst = nullptr; // SQLite database instance
    fs::path db_path{}; // Path to the database instance
};

// Constructor.
// Raises `filesystem_error` when the db file does not exist or is a directory.
sqliteops::DatabaseSession::DatabaseSession(
    const std::optional<fs::path> &dest_db_path
    ) : db_pimpl(std::make_unique<DBImpl>()) {
    // no values (default), do nothing; otherwise validate and assign
    if (dest_db_path.has_value()) {
        // Validate the path first
        if (!fs::exists(*dest_db_path)) {
            // file does not exist or has been moved elsewhere
            throw fs::filesystem_error(
                "The database does not exist or has been permanently moved.",
                *dest_db_path,
                std::make_error_code(std::errc::no_such_file_or_directory)
            );
        }
        if (fs::is_directory(*dest_db_path)) {
            // the path is actually a directory
            throw fs::filesystem_error(
                "The path you've specified is actually a directory.",
                *dest_db_path,
                std::make_error_code(std::errc::is_a_directory)
            );
        }
        db_pimpl->db_path = *dest_db_path;
    }
}

// Destructor
sqliteops::DatabaseSession::~DatabaseSession() {
    // Disconnect the database
    disconnect();
}
