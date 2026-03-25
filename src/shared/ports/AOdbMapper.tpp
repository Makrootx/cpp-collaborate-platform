#include "shared/ports/AOdbMapper.hpp"

template <typename T, typename Y>
    requires OdbMappable<T, Y>
T AOdbMapper<T, Y>::to_domain(const Y &odb)
{
    return Y::to_domain(odb);
}

template <typename T, typename Y>
    requires OdbMappable<T, Y>
Y AOdbMapper<T, Y>::to_odb(const T &domain)
{
    return Y::to_odb(domain);
}

template <typename T, typename Y>
    requires OdbMappable<T, Y>
T AOdbMapper<T, Y>::to_domain_query(const Y &odb, const std::vector<std::string> &columns)
{
    static_assert(OdbMappableQuery<T, Y>, "Y must provide to_domain_query/to_odb_query for AOdbMapper query methods");
    return Y::to_domain_query(odb, columns);
}

template <typename T, typename Y>
    requires OdbMappable<T, Y>
Y AOdbMapper<T, Y>::to_odb_query(const T &domain, const std::vector<std::string> &columns)
{
    static_assert(OdbMappableQuery<T, Y>, "Y must provide to_domain_query/to_odb_query for AOdbMapper query methods");
    return Y::to_odb_query(domain, columns);
}