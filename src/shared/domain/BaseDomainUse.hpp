#pragma once

#include <optional>
#include "shared/domain/BaseWrapper.hpp"

/// @brief Pure interface requiring an entity to expose a numeric identifier through get_id/set_id.
class BaseDomainIdentityUse
{
public:
    virtual ~BaseDomainIdentityUse() = default;
    virtual long int get_id() const = 0;
    virtual void set_id(long int id) = 0;
};

/// @brief Base use-case interface for a domain entity; combines identity access with domain materialization.
template <typename Domain>
class BaseDomainUse : public virtual BaseDomainIdentityUse, public virtual Wrappable<Domain>
{
public:
    virtual ~BaseDomainUse() = default;
};

/// @brief Secure variant of BaseDomainUse where every accessor wraps its return in optional to encode permission denial.
template <typename Domain>
class BaseDomainSecureUse : public virtual Wrappable<Domain>
{
public:
    virtual ~BaseDomainSecureUse() = default;
    virtual std::optional<long int> get_id() const = 0;
    virtual bool set_id(long int id) = 0;
};