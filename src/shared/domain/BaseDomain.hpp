#pragma once

/** @brief Common base for domain entities with numeric identifier. */
class BaseDomain
{
protected:
    long int id = 0;

public:
    long int get_id() const { return id; }
    void set_id(long int new_id) { id = new_id; }

    BaseDomain() = default;
    explicit BaseDomain(long int id) : id(id) {}
};