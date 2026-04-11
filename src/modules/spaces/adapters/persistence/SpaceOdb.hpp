#pragma once
#include "modules/spaces/domain/Space.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "modules/users/adapters/persistence/UserOdb.hpp"

#include "shared/adapters/persistence/odb/BaseOdb.hpp"

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>
#include <vector>
#include <optional>
#include <string>
#include <map>
#include <functional>

#pragma db object table("spaces")
/**
 * @brief ODB persistence model for spaces and related associations.
 *
 * Supports full and query-aware mapping between Space domain objects and
 * database rows.
 */
class SpaceOdb : public BaseOdb, public IOdbMappable<Space, SpaceOdb>, public IOdbMappableQuery<Space, SpaceOdb>
{

protected:
    explicit SpaceOdb(std::string title,
                      std::optional<std::string> description = std::nullopt,
                      unsigned long id = 0)
        : BaseOdb(id), title_(std::move(title)), description_(description ? odb::nullable<std::string>(*description)
                                                                          : odb::nullable<std::string>()) {}

public:
    SpaceOdb() = default;

    SpaceOdb(const SpaceOdb &) = default;
    SpaceOdb(SpaceOdb &&) noexcept = default;
    SpaceOdb &operator=(SpaceOdb &&) noexcept = default;
    SpaceOdb &operator=(const SpaceOdb &) = default;
    ~SpaceOdb() = default;

    const std::string &get_title() const { return title_; }
    const std::vector<odb::lazy_shared_ptr<UserOdb>> &get_members() const { return members_; }
    const odb::lazy_shared_ptr<UserOdb> &get_owner() const { return owner_; }

    std::optional<std::string> get_description() const
    {
        if (description_.null())
            return std::nullopt;
        return description_.get();
    }

    static Space to_domain(const SpaceOdb &odb);
    static SpaceOdb to_odb(const Space &domain);

private:
    friend class odb::access;

#pragma db column("title") type("VARCHAR(255)")
    std::string title_;

#pragma db column("description") type("TEXT") null
    odb::nullable<std::string> description_;

#pragma db value_not_null unordered \
    table("space_members")          \
        id_column("space_id")       \
            value_column("user_id")
    std::vector<odb::lazy_shared_ptr<UserOdb>> members_;

#pragma db not_null column("owner_id")
    odb::lazy_shared_ptr<UserOdb> owner_;

    const std::map<std::string, std::function<std::vector<Loader>()>> get_loaders_map() const override;
};