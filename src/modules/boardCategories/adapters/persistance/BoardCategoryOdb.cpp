#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb-odb.hxx"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"
#include "modules/tasks/adapters/persistance/TaskOdb-odb.hxx"
#include "modules/tasks/adapters/persistance/TaskOdb.hpp"

#include <memory>

BoardCategory BoardCategoryOdb::to_domain(const BoardCategoryOdb &odb)
{
    std::optional<std::vector<Task>> tasks;
    if (odb.is_field_included("tasks"))
    {
        tasks = std::vector<Task>{};
        for (const auto &t : odb.tasks_value())
        {
            if (!t.loaded())
            {
                tasks = std::nullopt;
                break;
            }
            tasks->push_back(TaskOdb::to_domain(*t));
        }
    }

    return BoardCategory(
        odb.title_value(),
        tasks,
        std::nullopt,
        odb.id_value());
}

const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> BoardCategoryOdb::get_loaders_map() const
{
    std::map<std::string, std::function<std::vector<Loader>()>> map;
    map["tasks"] = [this]()
    {
        return BaseOdb::to_loaders(this->tasks_);
    };
    return map;
}

BoardCategoryOdb BoardCategoryOdb::to_odb(const BoardCategory &domain)
{
    return BoardCategoryOdb(
        domain.get_title(),
        {},
        static_cast<unsigned long>(domain.get_id()));
}
