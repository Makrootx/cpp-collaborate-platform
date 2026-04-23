#pragma once

#include "modules/tasks/domain/ITaskUse.hpp"
#include "modules/tasks/domain/Task.hpp"
#include "modules/tasks/adapters/dto/TaskDto.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"

class ITaskService
{
public:
    virtual ~ITaskService() = default;

    virtual long int create_task(const TaskCreateDto &task) = 0;
    virtual std::optional<TaskResultDto> get_task_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::optional<TaskResultSecureDto> get_task_secured_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::vector<TaskResultDto> get_all_tasks_query(const std::vector<std::string> &relations) = 0;
    virtual std::vector<TaskResultSecureDto> get_all_tasks_query_secured(const std::vector<std::string> &relations) = 0;
    virtual void update_task(const TaskUpdateDto &task) = 0;
    virtual TaskResultSecureUpdateDto update_task_secured(const TaskUpdateDto &dto) = 0;
    virtual void delete_task_by_id(long int id) = 0;
};
