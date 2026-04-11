#pragma once
#include "modules/spaces/ports/ISpaceService.hpp"
#include "modules/spaces/ports/ISpaceRepo.hpp"
#include "modules/users/ports/IUserRepo.hpp"
#include <memory>

/**
 * @brief Space use-case implementation with membership and ownership logic.
 */
class SpaceService : public ISpaceService
{
    std::shared_ptr<ISpaceRepo> repo_;
    std::shared_ptr<IUserRepo> user_repo_;

public:
    explicit SpaceService(std::shared_ptr<ISpaceRepo> repo_, std::shared_ptr<IUserRepo> user_repo_) : repo_(std::move(repo_)), user_repo_(std::move(user_repo_)) {
                                                                                                      };

    /** @brief Creates a space and persists initial ownership state. */
    long int create_space(const SpaceCreateDto &dto) override;
    /** @brief Retrieves complete space DTO by identifier. */
    std::optional<SpaceResultDto> get_space_by_id(long int id) override;
    /** @brief Retrieves space DTO with selected fields only. */
    std::optional<SpaceResultDto> get_space_by_id_query(long int id, const std::vector<std::string> &columns) override;
    /** @brief Lists spaces visible to a user. */
    std::vector<SpaceResultDto> get_user_spaces(long int user_id) override;
    /** @brief Updates mutable space properties. */
    void update_space(const SpaceUpdateDto &dto) override;
    /** @brief Deletes space by identifier. */
    void delete_space_by_id(long int id) override;
    /** @brief Adds users as members of target space. */
    void add_space_members(long int space_id, std::vector<long int> user_ids) override;
    /** @brief Removes users from target space membership. */
    void remove_space_members(long int space_id, std::vector<long int> user_ids) override;
};