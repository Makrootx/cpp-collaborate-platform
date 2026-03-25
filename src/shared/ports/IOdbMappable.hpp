#pragma once

/**
 * @brief Compile-time mapping contract between domain and ODB models.
 *
 * Concrete ODB classes provide static conversion functions used by repository
 * and mapper layers for bidirectional transformation.
 */
template <typename Domain, typename Odb>
class IOdbMappable
{
public:
    static Domain to_domain(const Odb &odb);
    static Odb to_odb(const Domain &domain);
};