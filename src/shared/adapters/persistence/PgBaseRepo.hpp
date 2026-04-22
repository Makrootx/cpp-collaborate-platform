#pragma once
#include "shared/ports/IBaseRepo.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <concepts>
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include "shared/ports/AOdbMapper.hpp"

#include <concepts>

template <typename Odb>
concept OdbBase = requires(Odb odb) {
    { odb.id_value() } -> std::convertible_to<unsigned long>;
};

/**
 * @brief PostgreSQL ODB-backed base repository implementation.
 *
 * Provides shared CRUD infrastructure and dispatches domain/ODB conversion
 * through AOdbMapper.
 */
template <typename Domain, typename Odb>
    requires OdbMappable<Domain, Odb> && OdbBase<Odb> && OdbMappableQuery<Domain, Odb>
class PgBaseRepo : public virtual IBaseRepo<Domain>
{
protected:
    std::string context_ = "public";

    void execute_context_query()
    {
        std::string context_query = "SET search_path TO " + context_ + ", public";
        db_->execute(context_query);
    }

    void execute_context_query(std::string context)
    {
        std::string context_query = "SET search_path TO " + context + ", public";
        db_->execute(context_query);
    }

    std::shared_ptr<odb::database> db_;

    Domain to_domain(const Odb &odb)
    {
        return AOdbMapper<Domain, Odb>::to_domain(odb);
    };
    Odb to_odb(const Domain &domain)
    {
        return AOdbMapper<Domain, Odb>::to_odb(domain);
    };

    /**
     * @brief Convert ODB object to domain object with optional partial hydration.
     */
    Domain to_domain_query(const Odb &odb, const std::vector<std::string> &columns)
    {
        return AOdbMapper<Domain, Odb>::to_domain_query(odb, columns);
    };

public:
    explicit PgBaseRepo(std::shared_ptr<odb::database> db)
        : db_(std::move(db)) {}

    explicit PgBaseRepo(std::shared_ptr<odb::database> db, std::string schema_context)
        : db_(std::move(db)), context_(std::move(schema_context)) {}

    PgBaseRepo(const PgBaseRepo &) = default;
    PgBaseRepo(PgBaseRepo &&) noexcept = default;
    PgBaseRepo &operator=(PgBaseRepo &&) noexcept = default;
    PgBaseRepo &operator=(const PgBaseRepo &) = default;
    virtual ~PgBaseRepo() = default;

    long int create(const Domain &domain) override;
    std::optional<Domain> find_by_id(long int id) override;
    std::optional<Domain> find_by_id_query(long int id, const std::vector<std::string> &columns) override;
    std::vector<Domain> get_all() override;
    std::vector<Domain> get_all_query(const std::vector<std::string> &columns) override;
    void update(const Domain &domain) override;
    void delete_by_id(long int id) override;
};

#include "shared/adapters/persistence/PgBaseRepo.tpp"