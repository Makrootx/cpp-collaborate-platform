#pragma once
#include <concepts>
#include <string>
#include <vector>
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"

/** @brief True when Y provides full-object static conversion for T. */
template <typename T, typename Y>
concept OdbMappable = requires(T t, Y y) {
    { Y::to_domain(y) } -> std::convertible_to<T>;
    { Y::to_odb(t) } -> std::convertible_to<Y>;
};

/** @brief True when Y provides query-aware static conversion for T. */
template <typename T, typename Y>
concept OdbMappableQuery = requires(T t, Y y) {
    { Y::to_domain_query(y, std::vector<std::string>{}) } -> std::convertible_to<T>;
    { Y::to_odb_query(t, std::vector<std::string>{}) } -> std::convertible_to<Y>;
};

/**
 * @brief Stateless facade over static mapping functions on ODB types.
 *
 * Keeps persistence layer code generic and isolated from concrete adapter
 * implementations.
 */
template <typename T, typename Y>
    requires OdbMappable<T, Y>
class AOdbMapper
{
public:
    static T to_domain(const Y &odb);
    static Y to_odb(const T &domain);

    static T to_domain_query(const Y &odb, const std::vector<std::string> &columns);
    static Y to_odb_query(const T &domain, const std::vector<std::string> &columns);
};

#include "shared/ports/AOdbMapper.tpp"