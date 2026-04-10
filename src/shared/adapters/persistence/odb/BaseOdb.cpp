#include "shared/adapters/persistence/odb/BaseOdb.hpp"

const bool BaseOdb::is_field_included(const std::string &field) const
{
    return std::find(included_fields_.begin(), included_fields_.end(), field) != included_fields_.end();
}

const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> BaseOdb::get_loaders_map() const
{
    static const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> map = {};
    return map;
}

void BaseOdb::populate(
    const std::vector<std::string> &columns)
{
    included_fields_.clear();

    auto nested_columns_by_root = separate_root_and_nested(columns);

    auto populate_fields = get_loaders_map();

    for (const auto &[root, nested_columns] : nested_columns_by_root)
    {
        included_fields_.push_back(root);
        const auto it = populate_fields.find(root);
        if (it == populate_fields.end())
        {
            continue;
        }

        for (const auto &loader : it->second())
        {
            loader(nested_columns);
        }
    }
}

const std::map<std::string, std::vector<std::string>> BaseOdb::separate_root_and_nested(const std::vector<std::string> &columns)
{
    std::map<std::string, std::vector<std::string>> nested_columns_by_root;
    for (const auto &column : columns)
    {
        const std::string_view value(column);
        const auto split = value.find('.');
        if (split == std::string_view::npos)
        {
            nested_columns_by_root[column];
            continue;
        }

        const auto root = std::string(value.substr(0, split));
        const auto nested = std::string(value.substr(split + 1));
        if (!nested.empty())
        {
            nested_columns_by_root[root].push_back(std::move(nested));
        }
        else
        {
            nested_columns_by_root[root];
        }
    }
    return nested_columns_by_root;
}