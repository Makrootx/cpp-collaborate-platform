#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include <iostream>
#include <typeinfo>

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb>
long int PgBaseRepo<Domain, Odb>::create(const Domain &domain)
{
    Odb odb = to_odb(domain);
    odb::transaction t(db_->begin());
    db_->persist(odb);
    t.commit();
    return odb.id_value();
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb>
std::optional<Domain> PgBaseRepo<Domain, Odb>::find_by_id(long int id)
{
    try
    {
        odb::session s;
        odb::transaction t(db_->begin());
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
    requires OdbMappable<Domain, Odb> && OdbBase<Odb>
std::optional<Domain> PgBaseRepo<Domain, Odb>::find_by_id_query(long int id, const std::vector<std::string> &columns)
{
    try
    {
        odb::session s;
        odb::transaction t(db_->begin());
        using id_type = typename odb::object_traits<Odb>::id_type;
        std::optional<Domain> result;

        if (auto loaded = db_->template find<Odb>(static_cast<id_type>(id)); loaded)
        {
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
    requires OdbMappable<Domain, Odb> && OdbBase<Odb>
std::vector<Domain> PgBaseRepo<Domain, Odb>::get_all()
{
    odb::transaction t(db_->begin());
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
    requires OdbMappable<Domain, Odb> && OdbBase<Odb>
void PgBaseRepo<Domain, Odb>::update(const Domain &domain)
{
    Odb odb = to_odb(domain);
    odb::transaction t(db_->begin());
    db_->update(odb);
    t.commit();
}

template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb>
void PgBaseRepo<Domain, Odb>::delete_by_id(long int id)
{
    odb::transaction t(db_->begin());
    db_->erase<Odb>(id);
    t.commit();
}
