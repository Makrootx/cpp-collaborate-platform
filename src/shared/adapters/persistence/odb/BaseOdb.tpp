#include "shared/adapters/persistence/odb/BaseOdb.hpp"

template <typename LazyPtr>
    requires Loadable<LazyPtr>
const std::function<void(const std::vector<std::string> &)> BaseOdb::get_loader_callback(LazyPtr *ptr_ref)
{
    return [ptr_ref](const std::vector<std::string> &nested_columns)
    {
        auto loaded = ptr_ref->load();
        if (!loaded || nested_columns.empty())
        {
            return;
        }

        if constexpr (std::derived_from<std::remove_cvref_t<decltype(*loaded)>, BaseOdb>)
        {
            loaded->populate(nested_columns);
        }
    };
}

template <typename LazyPtr>
    requires Loadable<LazyPtr>
std::vector<BaseOdb::Loader> BaseOdb::to_loaders(const std::vector<LazyPtr> &lazy_ptrs)
{
    std::vector<BaseOdb::Loader> loaders;
    loaders.reserve(lazy_ptrs.size());
    for (const auto &ptr : lazy_ptrs)
    {
        const auto *ptr_ref = &ptr;
        loaders.push_back(get_loader_callback(ptr_ref));
    }
    return loaders;
}

template <typename LazyPtr>
    requires Loadable<LazyPtr>
std::vector<BaseOdb::Loader> BaseOdb::to_loaders(const LazyPtr &lazy_ptr)
{
    const auto *ptr_ref = &lazy_ptr;
    return {get_loader_callback(ptr_ref)};
}