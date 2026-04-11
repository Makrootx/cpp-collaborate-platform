#include "modules/spaces/domain/Space.hpp"

#include <algorithm>

void Space::add_member(const User &user)
{
    if (user_has_access(user.get_id()))
    {
        spdlog::debug("Space::add_member: User '{}' already has access to space '{}'", user.get_id(), id);
        return;
    }
    if (!members.has_value())
    {
        members = std::vector<User>{};
    }
    members->push_back(user);
};

void Space::remove_member(long int user_id)
{
    if (!members.has_value())
    {
        spdlog::debug("Space::remove_member: No members in space '{}'", id);
        throw std::runtime_error("No members in space");
    }

    auto &list = members.value();
    const auto it = std::remove_if(list.begin(), list.end(), [user_id](const User &member)
                                   { return member.get_id() == user_id; });

    if (it == list.end())
    {
        throw std::runtime_error("User is not a member of the space");
    }

    list.erase(it, list.end());
};

std::optional<User> Space::get_user_by_id(long int user_id) const
{
    if (!members.has_value())
    {
        spdlog::debug("Space::is_user_member: No members in space '{}'", id);
        throw std::runtime_error("No members in space");
    }

    for (const auto &member : *members)
    {
        if (member.get_id() == user_id)
        {
            return member;
        }
    }

    return std::nullopt;
};

bool Space::is_user_member(long int user_id) const
{
    if (!members.has_value())
    {
        spdlog::debug("Space::is_user_member: No members in space '{}'", id);
        throw std::runtime_error("No members in space");
    }

    for (const auto &member : *members)
    {
        if (member.get_id() == user_id)
        {
            return true;
        }
    }

    return false;
};

bool Space::user_has_access(long int user_id) const
{
    if (owner.get_id() == user_id)
    {
        return true;
    }

    return is_user_member(user_id);
}