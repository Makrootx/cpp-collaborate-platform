#pragma once
#include <vector>

/**
 * @brief Extension contract for query-aware domain/ODB conversion.
 *
 * Used when callers request partial hydration and conversion rules depend
 * on selected columns.
 */
template <typename Domain, typename Odb>
class IOdbMappableQuery
{
public:
    static Domain to_domain_query(Odb &odb, const std::vector<std::string> &columns);
    static Odb to_odb_query(const Domain &domain, const std::vector<std::string> &columns);
};