#pragma once

#include <optional>
#include "shared/domain/BaseWrapper.hpp"

class BaseDomainIdentityUse
{
public:
    virtual ~BaseDomainIdentityUse() = default;
    virtual long int get_id() const = 0;
    virtual void set_id(long int id) = 0;
};

template <typename Domain>
class BaseDomainUse : public virtual BaseDomainIdentityUse, public virtual Wrappable<Domain>
{
public:
    virtual ~BaseDomainUse() = default;
};

template <typename Domain>
class BaseDomainSecureUse : public virtual Wrappable<Domain>
{
public:
    virtual ~BaseDomainSecureUse() = default;
    virtual std::optional<long int> get_id() const = 0;
    virtual bool set_id(long int id) = 0;
};