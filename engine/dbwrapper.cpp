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
        try {
            validate_db_path(*dest_db_path);
            db_pimpl->db_path = *dest_db_path;
        } catch (fs::filesystem_error &fs_error) {
            std::cerr << fs_error.what() << "\n";
            std::cerr << "The DatabaseSession instance will remain uninitialized. ";
            std::cerr << "Please invoke create_dummy method to assign a path first.\n";
        }
    }
}

// Destructor
sqliteops::DatabaseSession::~DatabaseSession() {
    // Disconnect the database
    disconnect();
}

// Validate the database path.
// This assumes that the path IS NOT EMPTY.
void sqliteops::DatabaseSession::validate_db_path(const fs::path& dest_db_path) {
    // TODO: Add checks for extension (strictly .db)
    fs::path abs_dest_db_path = "~";
    // See if it is absolute, if not, fix it
    if (dest_db_path.is_relative()) {
        const auto root_path = fs::weakly_canonical("./");
        abs_dest_db_path = fs::weakly_canonical(root_path / dest_db_path);
    } else if (dest_db_path.is_absolute()) {
        abs_dest_db_path = dest_db_path;
    }

    // Validation
    if (!fs::exists(abs_dest_db_path)) {
        // file does not exist or has been moved elsewhere
        throw fs::filesystem_error(
            "The database does not exist or has been permanently moved.",
            dest_db_path,
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }
    if (fs::is_directory(dest_db_path)) {
        // the path is actually a directory
        throw fs::filesystem_error(
            "The path you've specified is actually a directory.",
            dest_db_path,
            std::make_error_code(std::errc::is_a_directory)
        );
    }
}

// Create an empty database at designated path.
// Return 0 if **operation successfully terminated without errors**.
// Do note that the return value is a strict 8-bit unsigned integer.
uint16_t sqliteops::DatabaseSession::create_dummy(const fs::path &dest_db_path) const {
    // Empty object, assign the value
    if (db_pimpl->db_path.empty()) {
        try {
            validate_db_path(dest_db_path);
            db_pimpl->db_path = dest_db_path;
        } catch (fs::filesystem_error &fs_error) {
            std::cerr << fs_error.what() << "\n";
            if (fs_error.code() == std::errc::no_such_file_or_directory)
                return SQLITE_CANTOPEN;
            if (fs_error.code() == std::errc::is_a_directory)
                return SQLITE_CANTOPEN_ISDIR;
        }
    }

    sqlite3* db_file = nullptr;
    // Create the database if not exist, using sqlite3_open
    const uint8_t return_code = sqlite3_open(
        dest_db_path.c_str(),
        &db_file
    );

    // Mostly the path is not correct (in case there are any other errors)
    if (return_code != SQLITE_OK) {
        std::cerr << "Unable to open or create the database " << dest_db_path
            << ". Error message: "
            << (db_file ? sqlite3_errmsg(db_file) : "Unknown Error")
            << "\n";
    } else {
        db_pimpl->db_inst = db_file;
    }
    sqlite3_close(db_file);
    return return_code;
}
