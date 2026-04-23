#include "modules/spaces/adapters/persistence/PgSpaceRepo.hpp"
#include "modules/spaces/adapters/persistence/SpaceOdb-odb.hxx"
#include "modules/users/adapters/persistence/UserOdb-odb.hxx"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    std::filesystem::path resolve_task_sql_path(std::string path)
    {
        const std::filesystem::path base = PROJECT_SOURCE_DIR;

        const std::filesystem::path generated =
            base / path;
        if (std::filesystem::exists(generated))
        {
            return generated;
        }

        throw std::runtime_error("SQL file not found: " + generated.string());
    }

    std::string read_file_content(const std::filesystem::path &file_path)
    {
        std::ifstream input(file_path, std::ios::in | std::ios::binary);
        if (!input)
        {
            throw std::runtime_error("Failed to open SQL file: " + file_path.string());
        }

        std::ostringstream buffer;
        buffer << input.rdbuf();
        return buffer.str();
    }
} // namespace

template <typename Y>
    requires OdbMappable<Space, Y> && OdbBase<Y> && OdbMappableQuery<Space, Y>
long int PgSpaceRepo<Y>::create(const Space &domain)
{
    const auto space_id = PgBaseRepo<Space, Y>::create(domain);
    try
    {
        const std::string schema_name = "space_" + std::to_string(space_id);
        const std::string create_schema_sql = "CREATE SCHEMA IF NOT EXISTS \"" + schema_name + "\"";
        const std::string set_search_path_sql = "SET search_path TO \"" + schema_name + "\", public";
        const std::string create_task_table_sql = read_file_content(resolve_task_sql_path("src\\modules\\tasks\\adapters\\persistance\\TaskOdb.sql"));
        const std::string create_group_table_sql = read_file_content(resolve_task_sql_path("src\\modules\\groups\\adapters\\persistance\\GroupOdb.sql"));
        const std::string create_automation_table_sql = read_file_content(resolve_task_sql_path("src\\modules\\automation\\adapters\\persistance\\AutomationDomainOdb.sql"));
        const std::string automation_insert_in_table = read_file_content(resolve_task_sql_path("src\\modules\\automation\\adapters\\persistance\\AutomationDomainOdbInsert.sql"));
        const std::string create_board_table_sql = read_file_content(resolve_task_sql_path("src\\modules\\boards\\adapters\\persistance\\BoardOdb.sql"));
        const std::string create_board_category_table_sql = read_file_content(resolve_task_sql_path("src\\modules\\boardCategories\\adapters\\persistance\\BoardCategoryOdb.sql"));

        odb::transaction tx(this->db_->begin());
        this->execute_context_query();
        this->db_->execute(create_schema_sql);
        this->db_->execute(set_search_path_sql);
        this->db_->execute(create_task_table_sql);
        this->db_->execute(create_group_table_sql);
        this->db_->execute(create_automation_table_sql);
        this->db_->execute(automation_insert_in_table);
        // BoardCategory must run before Board: Board's ALTER on board_category_boards
        // references board_categories (created by BoardCategory), so BoardCategory goes first.
        this->db_->execute(create_board_category_table_sql);
        this->db_->execute(create_board_table_sql);
        tx.commit();
    }
    catch (const std::exception &e)
    {
        PgBaseRepo<Space, Y>::delete_by_id(space_id);
        throw;
    }

    return space_id;
}

template <typename Y>
    requires OdbMappable<Space, Y> && OdbBase<Y> && OdbMappableQuery<Space, Y>
std::vector<Space> PgSpaceRepo<Y>::find_user_spaces(long int user_id)
{
    std::vector<Space> result;

    odb::transaction t(this->db_->begin());
    this->execute_context_query();

    // Query spaces where user is a member (via join table)
    // OR where user is the owner
    auto rows = this->db_->template query<Y>(
        "id IN ("
        "SELECT space_id FROM space_members WHERE user_id = " +
        odb::query<Y>::_val(static_cast<unsigned long>(user_id)) +
        ") OR owner_id = " +
        odb::query<Y>::_val(static_cast<unsigned long>(user_id)));

    for (auto it = rows.begin(); it != rows.end(); ++it)
    {
        // Load lazy relations while transaction is still open
        // it.load();
        result.push_back(this->to_domain(*it));
    }

    t.commit();
    return result;
}

template class PgSpaceRepo<SpaceOdb>;