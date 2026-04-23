#pragma once
#include "shared/domain/BaseDomainUse.hpp"

/** @brief Common base for domain entities with numeric identifier. */
class BaseDomain : public virtual BaseDomainIdentityUse
{
protected:
    long int id = 0;

public:
    long int get_id() const override { return id; }
    void set_id(long int new_id) override { id = new_id; }

    BaseDomain() = default;
    explicit BaseDomain(long int id) : id(id) {}
};