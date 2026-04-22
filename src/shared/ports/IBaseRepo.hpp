#pragma once
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Generic repository contract for CRUD over domain entities.
 *
 * Concrete persistence adapters implement this interface for storage backends.
 */
template <typename T>
class IBaseRepo
{
public:
    virtual ~IBaseRepo() = default;

    virtual long int create(const T &domain) = 0;
    virtual std::optional<T> find_by_id(long int id) = 0;
    virtual std::optional<T> find_by_id_query(long int id, const std::vector<std::string> &columns) = 0;
    virtual std::vector<T> get_all() = 0;
    virtual std::vector<T> get_all_query(const std::vector<std::string> &columns) = 0;
    virtual void update(const T &domain) = 0;
    virtual void delete_by_id(long int id) = 0;
};