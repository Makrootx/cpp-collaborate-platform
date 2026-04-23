#include "modules/tasks/adapters/TaskService.hpp"
#include "modules/tasks/domain/TaskWrappers.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"
#include <spdlog/spdlog.h>

long int TaskService::create_task(const TaskCreateDto &dto)
{
    auto category_opt = category_repo_->find_by_id(dto.category_id);
    if (!category_opt.has_value())
    {
        spdlog::error("Failed to create task: category with id {} not found", dto.category_id);
        throw std::runtime_error("Parent category not found");
    }
    auto space_opt = space_repo_->find_by_id_query(space_id_, {"members"});
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to create task: space with id {} not found", space_id_);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();
    auto assignee_user = std::optional<std::shared_ptr<User>>{std::nullopt};
    if (dto.assignee_id.has_value())
    {
        if (!space.user_has_access(dto.assignee_id.value()))
        {
            spdlog::error("Failed to create task: assignee user with id {} does not have access to space with id {}", dto.assignee_id.value(), space_id_);
            throw std::runtime_error("Assignee user does not have access to space");
        }
        auto assignee_user_opt = space.get_user_by_id(dto.assignee_id.value());
        assignee_user = std::make_shared<User>(assignee_user_opt.value());
    }
    auto reporter_user = std::optional<std::shared_ptr<User>>{std::nullopt};
    if (dto.reporter_id.has_value())
    {
        if (!space.user_has_access(dto.reporter_id.value()))
        {
            spdlog::error("Failed to create task: reporter user with id {} does not have access to space with id {}", dto.reporter_id.value(), space_id_);
            throw std::runtime_error("Reporter user does not have access to space");
        }
        auto reporter_user_opt = space.get_user_by_id(dto.reporter_id.value());
        reporter_user = std::make_shared<User>(reporter_user_opt.value());
    }
    std::tm now_tm;
    std::time_t now = std::time(nullptr);
    localtime_s(&now_tm, &now);
    auto task = Task(
        dto.title,
        dto.description,
        now_tm,
        now_tm,
        dto.due_date,
        assignee_user,
        reporter_user,
        dto.labels.has_value() ? std::unordered_set<std::string>(dto.labels.value().begin(), dto.labels.value().end()) : std::unordered_set<std::string>{},
        dto.priority,
        dto.type);

    auto task_id = repo_->create(task);
    category_repo_->link_task(dto.category_id, task_id);
    return task_id;
}

std::optional<TaskResultDto> TaskService::get_task_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto task_opt = repo_->find_by_id_query(id, relations);
    if (!task_opt.has_value())
    {
        return std::nullopt;
    }
    auto task = task_opt.value();
    return TaskResultDto::from_domain(task);
}

std::optional<TaskResultSecureDto> TaskService::get_task_secured_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto task_opt = repo_->find_by_id_query(id, relations);
    if (!task_opt.has_value())
    {
        return std::nullopt;
    }
    auto task = task_opt.value();
    auto task_automative = AutomativeTask{task, space_id_};
    auto task_secured = SecureTask{task_automative, required_permission};
    auto result = TaskResultSecureDto::from_domain(task_secured);
    task_automative.flush_domain_actions();

    return result.merge_with_plain(TaskResultDto::from_domain(task));
}

std::vector<TaskResultDto> TaskService::get_all_tasks_query(const std::vector<std::string> &relations)
{
    auto tasks = repo_->get_all_query(relations);
    std::vector<TaskResultDto> task_results;
    for (const auto &task : tasks)
    {
        task_results.push_back(TaskResultDto::from_domain(task));
    }
    return task_results;
}

std::vector<TaskResultSecureDto> TaskService::get_all_tasks_query_secured(const std::vector<std::string> &relations)
{
    auto tasks = repo_->get_all_query(relations);

    std::vector<TaskResultSecureDto> task_results;
    for (auto &task : tasks)
    {
        auto task_automative = AutomativeTask{task, space_id_};
        auto task_secured = SecureTask{task_automative, required_permission};
        auto result = TaskResultSecureDto::from_domain(task_secured);
        task_automative.flush_domain_actions();

        task_results.push_back(result.merge_with_plain(TaskResultDto::from_domain(task)));
    }
    return task_results;
}

TaskResultSecureUpdateDto TaskService::update_task_secured(const TaskUpdateDto &dto)
{
    TaskResultSecureUpdateDto secure_update_dto;
    auto task_opt = repo_->find_by_id(dto.id);
    if (!task_opt.has_value())
    {
        spdlog::error("Failed to update task: task with id {} not found", dto.id);
        throw std::runtime_error("Task not found");
    }
    auto task_plain = task_opt.value();
    auto task = SecureTask{AutomativeTask{task_plain, space_id_}, required_permission};

    auto space_opt = space_repo_->find_by_id_query(space_id_, {"members"});
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to create task: space with id {} not found", space_id_);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();

    if (dto.title.has_value())
    {
        secure_update_dto.title = task.setTitle(dto.title.value());
    }
    if (dto.description.has_value())
    {
        secure_update_dto.description = task.setDescription(dto.description.value());
    }
    if (dto.due_date.has_value())
    {
        secure_update_dto.due_date = task.setDueDate(dto.due_date.value());
    }
    auto assignee_user = std::optional<std::shared_ptr<User>>{std::nullopt};
    if (dto.assignee_id.has_value())
    {
        if (!space.user_has_access(dto.assignee_id.value()))
        {
            spdlog::error("Failed to update task: assignee user with id {} does not have access to space with id {}", dto.assignee_id.value(), space_id_);
            throw std::runtime_error("Assignee user does not have access to space");
        }
        auto assignee_user_opt = space.get_user_by_id(dto.assignee_id.value());
        assignee_user = std::make_shared<User>(assignee_user_opt.value());
        secure_update_dto.assignee = task.setAssignee(*assignee_user);
    }
    auto reporter_user = std::optional<std::shared_ptr<User>>{std::nullopt};
    if (dto.reporter_id.has_value())
    {
        if (!space.user_has_access(dto.reporter_id.value()))
        {
            spdlog::error("Failed to update task: assignee user with id {} does not have access to space with id {}", dto.reporter_id.value(), space_id_);
            throw std::runtime_error("Assignee user does not have access to space");
        }
        auto reporter_user_opt = space.get_user_by_id(dto.reporter_id.value());
        reporter_user = std::make_shared<User>(reporter_user_opt.value());
        secure_update_dto.reporter = task.setReporter(*reporter_user);
    }
    if (dto.labels.has_value())
    {
        secure_update_dto.labels = task.addLabels(std::vector<std::string>(dto.labels.value().begin(), dto.labels.value().end()));
    }
    if (dto.priority.has_value())
    {
        secure_update_dto.priority = task.setPriority(dto.priority.value());
    }
    if (dto.type.has_value())
    {
        secure_update_dto.type = task.setType(dto.type.value());
    }
    return secure_update_dto;
}

void TaskService::update_task(const TaskUpdateDto &dto)
{
    auto task_opt = repo_->find_by_id(dto.id);
    if (!task_opt.has_value())
    {
        spdlog::error("Failed to update task: task with id {} not found", dto.id);
        throw std::runtime_error("Task not found");
    }
    auto task = task_opt.value();

    auto space_opt = space_repo_->find_by_id_query(space_id_, {"members"});
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to create task: space with id {} not found", space_id_);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();

    if (dto.title.has_value())
    {
        task.setTitle(dto.title.value());
    }
    if (dto.description.has_value())
    {
        task.setDescription(dto.description.value());
    }
    if (dto.due_date.has_value())
    {
        task.setDueDate(dto.due_date.value());
    }
    auto assignee_user = std::optional<std::shared_ptr<User>>{std::nullopt};
    if (dto.assignee_id.has_value())
    {
        if (!space.user_has_access(dto.assignee_id.value()))
        {
            spdlog::error("Failed to update task: assignee user with id {} does not have access to space with id {}", dto.assignee_id.value(), space_id_);
            throw std::runtime_error("Assignee user does not have access to space");
        }
        auto assignee_user_opt = space.get_user_by_id(dto.assignee_id.value());
        assignee_user = std::make_shared<User>(assignee_user_opt.value());
        task.setAssignee(*assignee_user);
    }
    auto reporter_user = std::optional<std::shared_ptr<User>>{std::nullopt};
    if (dto.reporter_id.has_value())
    {
        if (!space.user_has_access(dto.reporter_id.value()))
        {
            spdlog::error("Failed to update task: assignee user with id {} does not have access to space with id {}", dto.reporter_id.value(), space_id_);
            throw std::runtime_error("Assignee user does not have access to space");
        }
        auto reporter_user_opt = space.get_user_by_id(dto.reporter_id.value());
        reporter_user = std::make_shared<User>(reporter_user_opt.value());
        task.setReporter(*reporter_user);
    }
    if (dto.labels.has_value())
    {
        task.addLabels(std::vector<std::string>(dto.labels.value().begin(), dto.labels.value().end()));
    }
    if (dto.priority.has_value())
    {
        task.setPriority(dto.priority.value());
    }
    if (dto.type.has_value())
    {
        task.setType(dto.type.value());
    }
}

void TaskService::delete_task_by_id(long int id)
{
    auto task_opt = repo_->find_by_id(id);
    if (!task_opt.has_value())
    {
        spdlog::error("Failed to delete task: task with id {} not found", id);
        throw std::runtime_error("Task not found");
    }
    repo_->delete_by_id(id);
}
