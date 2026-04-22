#include "shared/ports/AOdbMapper.hpp"

template <typename T, typename Y>
    requires OdbMappable<T, Y> && OdbMappableQuery<T, Y>
T AOdbMapper<T, Y>::to_domain(const Y &odb)
{
    return Y::to_domain(odb);
}

template <typename T, typename Y>
    requires OdbMappable<T, Y> && OdbMappableQuery<T, Y>
Y AOdbMapper<T, Y>::to_odb(const T &domain)
{
    return Y::to_odb(domain);
}

template <typename T, typename Y>
    requires OdbMappable<T, Y> && OdbMappableQuery<T, Y>
T AOdbMapper<T, Y>::to_domain_query(const Y &odb, const std::vector<std::string> &columns)
{
    odb.populate(columns);
    return Y::to_domain(odb);
}