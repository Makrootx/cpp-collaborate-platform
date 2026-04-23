#pragma once

#include "modules/tasks/ports/ITaskService.hpp"
#include "modules/tasks/ports/ITaskRepo.hpp"
#include "modules/users/ports/IUserRepo.hpp"
#include "modules/spaces/ports/ISpaceRepo.hpp"
#include "modules/boardCategories/ports/IBoardCategoryRepo.hpp"
#include "modules/tasks/adapters/dto/TaskDto.hpp"
#include <memory>

class TaskService : public ITaskService
{
    long int space_id_ = -1;
    std::shared_ptr<ITaskRepo> repo_;
    std::shared_ptr<ISpaceRepo> space_repo_;
    std::shared_ptr<IBoardCategoryRepo> category_repo_;
    GroupPermission required_permission;

public:
    explicit TaskService(std::shared_ptr<ITaskRepo> repo_,
                         std::shared_ptr<ISpaceRepo> space_repo_,
                         std::shared_ptr<IBoardCategoryRepo> category_repo_,
                         GroupPermission required_permission = {})
        : repo_(std::move(repo_)),
          space_repo_(std::move(space_repo_)),
          category_repo_(std::move(category_repo_)),
          required_permission(required_permission) {};

    explicit TaskService(long int space_id, GroupPermission required_permission,
                         std::shared_ptr<ITaskRepo> repo_,
                         std::shared_ptr<ISpaceRepo> space_repo_,
                         std::shared_ptr<IBoardCategoryRepo> category_repo_)
        : TaskService(std::move(repo_), std::move(space_repo_), std::move(category_repo_), required_permission)
    {
        this->space_id_ = space_id;
    };

    long int create_task(const TaskCreateDto &dto) override;
    std::optional<TaskResultDto> get_task_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::optional<TaskResultSecureDto> get_task_secured_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::vector<TaskResultDto> get_all_tasks_query(const std::vector<std::string> &relations) override;
    std::vector<TaskResultSecureDto> get_all_tasks_query_secured(const std::vector<std::string> &relations) override;
    // std::vector<TaskResultDto> get_all_tasks() override;
    void update_task(const TaskUpdateDto &dto) override;
    TaskResultSecureUpdateDto update_task_secured(const TaskUpdateDto &dto) override;
    void delete_task_by_id(long int id) override;
};