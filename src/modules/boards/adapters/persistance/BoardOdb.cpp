#include "modules/boards/adapters/persistance/BoardOdb-odb.hxx"
#include "modules/boards/adapters/persistance/BoardOdb.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb-odb.hxx"

#include <memory>

Board BoardOdb::to_domain(const BoardOdb &odb)
{
    std::optional<std::vector<std::shared_ptr<BoardCategory>>> categories;
    if (odb.is_field_included("categories"))
    {
        categories = std::vector<std::shared_ptr<BoardCategory>>{};
        for (const auto &cat : odb.categories_value())
        {
            if (!cat.loaded())
            {
                categories = std::nullopt;
                break;
            }
            categories->push_back(std::make_shared<BoardCategory>(BoardCategoryOdb::to_domain(*cat)));
        }
    }

    return Board(
        odb.title_value(),
        odb.description_value(),
        categories,
        odb.id_value());
}

const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> BoardOdb::get_loaders_map() const
{
    std::map<std::string, std::function<std::vector<Loader>()>> map;
    map["categories"] = [this]()
    {
        return BaseOdb::to_loaders(this->categories_);
    };
    return map;
}

BoardOdb BoardOdb::to_odb(const Board &domain)
{
    return BoardOdb(
        domain.get_title(),
        domain.get_description(),
        static_cast<unsigned long>(domain.get_id()));
}
