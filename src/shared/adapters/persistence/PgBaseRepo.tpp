#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include <iostream>
#include <typeinfo>
#include <spdlog/spdlog.h>

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
long int PgBaseRepo<Domain, Odb>::create(const Domain &domain)
{
    Odb odb = to_odb(domain);
    odb::transaction t(db_->begin());
    execute_context_query();
    db_->persist(odb);
    t.commit();
    return odb.id_value();
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
std::optional<Domain> PgBaseRepo<Domain, Odb>::find_by_id(long int id)
{
    try
    {
        odb::session s;
        odb::transaction t(db_->begin());
        execute_context_query();
        using id_type = typename odb::object_traits<Odb>::id_type;
        std::optional<Domain> result;

        if (auto loaded = db_->template find<Odb>(static_cast<id_type>(id)); loaded)
        {
            result = to_domain(*loaded);
        }

        t.commit();
        return result;
    }
    catch (const std::exception &)
    {
        return std::nullopt;
    }
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
std::optional<Domain> PgBaseRepo<Domain, Odb>::find_by_id_query(long int id, const std::vector<std::string> &columns)
{
    try
    {
        odb::session s;
        odb::transaction t(db_->begin());
        execute_context_query();
        using id_type = typename odb::object_traits<Odb>::id_type;
        std::optional<Domain> result;

        if (auto loaded = db_->template find<Odb>(static_cast<id_type>(id)); loaded)
        {
            spdlog::debug("Found object with id {}: {}", id, typeid(*loaded).name());
            result = to_domain_query(*loaded, columns);
        }

        t.commit();
        return result;
    }
    catch (const std::exception &)
    {
        return std::nullopt;
    }
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
std::vector<Domain> PgBaseRepo<Domain, Odb>::get_all()
{
    odb::transaction t(db_->begin());
    execute_context_query();
    std::vector<Domain> result;
    auto r = db_->query<Odb>();
    for (const auto &odb : r)
    {
        result.push_back(to_domain(odb));
    }
    t.commit();
    return result;
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
std::vector<Domain> PgBaseRepo<Domain, Odb>::get_all_query(const std::vector<std::string> &columns)
{
    odb::transaction t(db_->begin());
    execute_context_query();
    std::vector<Domain> result;
    auto r = db_->query<Odb>();
    for (const auto &odb : r)
    {
        result.push_back(to_domain_query(odb, columns));
    }
    t.commit();
    return result;
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
void PgBaseRepo<Domain, Odb>::update(const Domain &domain)
{
    Odb odb = to_odb(domain);
    odb::transaction t(db_->begin());
    execute_context_query();
    db_->update(odb);
    t.commit();
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
void PgBaseRepo<Domain, Odb>::delete_by_id(long int id)
{
    odb::transaction t(db_->begin());
    execute_context_query();
    db_->erase<Odb>(id);
    t.commit();
}
