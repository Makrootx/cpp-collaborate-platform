#pragma once
#include "shared/domain/BaseDomain.hpp"
#include <optional>
#include <string>
#include <vector>
#include "modules/users/domain/User.hpp"
#include <spdlog/spdlog.h>

// class SpaceOdb;

/**
 * @brief Space aggregate containing owner, optional members, and metadata.
 */
class Space : public BaseDomain
{
protected:
    std::string title;
    std::optional<std::string> description;
    std::optional<std::vector<User>> members;
    User owner;

public:
    Space() = default;

    explicit Space(std::string title,
                   User owner,
                   std::optional<std::string> description = std::nullopt,
                   std::optional<std::vector<User>> members = {}, unsigned long id = 0)
        : title(std::move(title)), description(std::move(description)), owner(std::move(owner)), members(std::move(members)), BaseDomain(id) {}

    Space(const Space &) = default;
    Space(Space &&) noexcept = default;
    Space &operator=(Space &&) noexcept = default;
    Space &operator=(const Space &) = default;
    ~Space() = default;

    void set_title(std::string title) { this->title = std::move(title); };
    void set_description(std::optional<std::string> description) { this->description = std::move(description); };
    void set_owner(const User &new_owner) { owner = std::move(new_owner); }

    const std::string &get_title() const { return title; }
    const User &get_owner() const { return owner; }
    const std::optional<std::string> &get_description() const { return description; }
    const std::optional<std::vector<User>> &get_members() const { return members; }

    /** @brief Finds member by id when members are available in current aggregate state. */
    std::optional<User> get_user_by_id(long int user_id) const;
    /** @brief Checks whether user is part of members collection. */
    bool is_user_member(long int user_id) const;
    /** @brief Grants access when user is owner or a registered member. */
    bool user_has_access(long int user_id) const;
    /** @brief Adds user to members collection if not already present. */
    void add_member(const User &user);
    /** @brief Removes user from members collection by identifier. */
    void remove_member(long int user_id);
};
