// GMF
module;
#include <iostream>
#include <optional>
#include <filesystem>
#include <memory>
#include <string_view>
#include <cstdint> // In case some compilers say "uint8_t was not declared"

export module dbwrapper;

namespace sqliteops {
    namespace fs = std::filesystem;

    export void test() {
        std::cout << "This is a test function.\n";
    }

    export class DatabaseSession {
    public:
        // Constructor and destructor
        explicit DatabaseSession(const std::optional<fs::path>& dest_db_path);
        ~DatabaseSession();

        // Disable copying while allow moving
        // Disable copying
        DatabaseSession(const DatabaseSession&) = delete;
        DatabaseSession& operator=(const DatabaseSession&) = delete;

        // Explicitly enable moving
        DatabaseSession(DatabaseSession&&) = default;
        DatabaseSession& operator=(DatabaseSession&&) = default;

        // Create empty or preconfigured database
        [[nodiscard]] uint16_t create_dummy(const fs::path& dest_db_path) const;
        [[nodiscard]] uint16_t create_db(const fs::path& dest_db_path, const fs::path& schema_path);

        // Connect and disconnect
        bool connect(uint8_t sqlite_open_flag);
        bool disconnect();

        // Execute SQL statements (including insert, update, lookup and delete)
        uint8_t execute_sql(std::string_view sql_statement);
        uint8_t execute_sql_schema(const fs::path& schema_path);

        // Add more methods here...
    private:
        // Apply Pimpl (Pointer to Implementation) idiom to hide attrs
        struct DBImpl;
        std::unique_ptr<DBImpl> db_pimpl;

        // Validate file
        static void validate_db_path(const fs::path& dest_db_path);
    };
}
