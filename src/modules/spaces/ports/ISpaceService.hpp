#pragma once

#include "modules/spaces/domain/Space.hpp"
#include "modules/spaces/ports/ISpaceRepo.hpp"
#include "modules/spaces/adapters/dto/SpaceDto.hpp"

/** @brief Application service contract for space-related workflows. */
class ISpaceService
{
public:
    /** @brief Creates a new space and returns persisted identifier. */
    virtual long int create_space(const SpaceCreateDto &dto) = 0;
    /** @brief Retrieves full space representation by id. */
    virtual std::optional<SpaceResultDto> get_space_by_id(long int id) = 0;
    /** @brief Retrieves space by id with selective field hydration. */
    virtual std::optional<SpaceResultDto> get_space_by_id_query(long int id, const std::vector<std::string> &columns) = 0;
    /** @brief Lists spaces accessible to specified user. */
    virtual std::vector<SpaceResultDto> get_user_spaces(long int user_id) = 0;
    /** @brief Updates editable space metadata. */
    virtual void update_space(const SpaceUpdateDto &dto) = 0;
    /** @brief Deletes space and associated ownership relations by id. */
    virtual void delete_space_by_id(long int id) = 0;
    /** @brief Adds multiple users to target space membership. */
    virtual void add_space_members(long int space_id, std::vector<long int> user_ids) = 0;
    /** @brief Removes multiple users from target space membership. */
    virtual void remove_space_members(long int space_id, std::vector<long int> user_ids) = 0;
};