#pragma once

#include "shared/helpers/json/JsonDto.hpp"
#include "modules/tasks/domain/Task.hpp"
#include "modules/users/adapters/dto/UserDto.hpp"

class TaskCreateDto : public JsonDto<TaskCreateDto>
{
public:
    long int category_id;
    std::string title;
    std::string description;
    std::optional<std::tm> due_date;
    std::optional<long int> assignee_id;
    std::optional<long int> reporter_id;
    std::optional<std::vector<std::string>> labels;
    TaskPriority priority;
    std::optional<std::string> type;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("category_id", &TaskCreateDto::category_id),
            field("title", &TaskCreateDto::title),
            field("description", &TaskCreateDto::description),
            optional_field("due_date", &TaskCreateDto::due_date),
            optional_field("assignee_id", &TaskCreateDto::assignee_id),
            optional_field("reporter_id", &TaskCreateDto::reporter_id),
            optional_field("labels", &TaskCreateDto::labels),
            field("priority", &TaskCreateDto::priority),
            optional_field("type", &TaskCreateDto::type),
        };
        return f;
    };
};

class TaskUpdateDto : public JsonDto<TaskUpdateDto>
{
public:
    long int id;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::tm> due_date;
    std::optional<long int> assignee_id;
    std::optional<long int> reporter_id;
    std::optional<std::vector<std::string>> labels;
    std::optional<TaskPriority> priority;
    std::optional<std::string> type;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &TaskUpdateDto::id),
            optional_field("title", &TaskUpdateDto::title),
            optional_field("description", &TaskUpdateDto::description),
            optional_field("due_date", &TaskUpdateDto::due_date),
            optional_field("assignee_id", &TaskUpdateDto::assignee_id),
            optional_field("reporter_id", &TaskUpdateDto::reporter_id),
            optional_field("labels", &TaskUpdateDto::labels),
            optional_field("priority", &TaskUpdateDto::priority),
            optional_field("type", &TaskUpdateDto::type),
        };
        return f;
    };
};

class TaskResultDto : public JsonDto<TaskResultDto>
{
public:
    long int id;
    std::string title;
    std::string description;
    std::tm created_date;
    std::tm updated_date;
    std::optional<std::tm> due_date;
    std::optional<UserResultDto> assignee;
    std::optional<UserResultDto> reporter;
    std::vector<std::string> labels;
    TaskPriority priority;
    std::optional<std::string> type;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &TaskResultDto::id),
            field("title", &TaskResultDto::title),
            field("description", &TaskResultDto::description),
            field("created_date", &TaskResultDto::created_date),
            field("updated_date", &TaskResultDto::updated_date),
            optional_field("due_date", &TaskResultDto::due_date),
            optional_nested_field("assignee", &TaskResultDto::assignee),
            optional_nested_field("reporter", &TaskResultDto::reporter),
            field("labels", &TaskResultDto::labels),
            field("priority", &TaskResultDto::priority),
            optional_field("type", &TaskResultDto::type),
        };
        return f;
    };

    static TaskResultDto from_domain(const ITaskUse &task)
    {
        auto task_result = TaskResultDto{
            .id = task.get_id(),
            .title = task.getTitle(),
            .description = task.getDescription(),
            .created_date = task.getCreatedDate(),
            .updated_date = task.getUpdatedDate(),
            .due_date = task.getDueDate(),
            .labels = task.getLabels(),
            .priority = task.getPriority(),
            .type = task.getType()};
        auto assignee_opt = task.getAssignee();
        if (assignee_opt.has_value() && assignee_opt.value() != nullptr)
        {
            auto user_ptr = assignee_opt.value();
            if (user_ptr)
            {
                task_result.assignee = UserResultDto::from_domain(user_ptr->to_domain());
            }
        }
        auto reporter_opt = task.getReporter();

        return task_result;
    }
};

class TaskResultSecureDto : public JsonDto<TaskResultSecureDto>
{
public:
    std::optional<long int> id;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::tm> created_date;
    std::optional<std::tm> updated_date;
    std::optional<std::optional<std::tm>> due_date;
    std::optional<std::optional<UserResultSecureDto>> assignee;
    std::optional<std::optional<UserResultSecureDto>> reporter;
    std::optional<std::vector<std::string>> labels;
    std::optional<TaskPriority> priority;
    std::optional<std::optional<std::string>> type;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field_secured("id", &TaskResultSecureDto::id),
            field_secured("title", &TaskResultSecureDto::title),
            field_secured("description", &TaskResultSecureDto::description),
            field_secured("created_date", &TaskResultSecureDto::created_date),
            field_secured("updated_date", &TaskResultSecureDto::updated_date),
            optional_field_secured("due_date", &TaskResultSecureDto::due_date),
            optional_nested_field_secured("assignee", &TaskResultSecureDto::assignee),
            optional_nested_field_secured("reporter", &TaskResultSecureDto::reporter),
            field_secured("labels", &TaskResultSecureDto::labels),
            field_secured("priority", &TaskResultSecureDto::priority),
            optional_field_secured("type", &TaskResultSecureDto::type),
        };
        return f;
    };

    TaskResultSecureDto merge_with_plain(const TaskResultDto &plain)
    {
        if (id.has_value())
            id = plain.id;
        if (title.has_value())
            title = plain.title;
        if (description.has_value())
            description = plain.description;
        if (created_date.has_value())
            created_date = plain.created_date;
        if (updated_date.has_value())
            updated_date = plain.updated_date;
        if (due_date.has_value())
            due_date = plain.due_date;
        if (assignee.has_value())
            assignee.value().has_value() ? assignee.value().value().merge_with_plain(plain.assignee.value()) : std::optional<std::optional<UserResultSecureDto>>(std::nullopt);
        if (reporter.has_value())
            reporter.value().has_value() ? reporter.value().value().merge_with_plain(plain.reporter.value()) : std::optional<std::optional<UserResultSecureDto>>(std::nullopt);
        if (labels.has_value())
            labels = plain.labels;
        if (priority.has_value())
            priority = plain.priority;
        if (type.has_value())
            type = plain.type;
        return *this;
    }

    static TaskResultSecureDto from_domain(const ITaskSecureUse &task)
    {
        auto task_result = TaskResultSecureDto{
            .id = task.get_id(),
            .title = task.getTitle(),
            .description = task.getDescription(),
            .created_date = task.getCreatedDate(),
            .updated_date = task.getUpdatedDate(),
            .due_date = task.getDueDate(),
            .assignee = std::nullopt,
            .reporter = std::nullopt,
            .labels = task.getLabels(),
            .priority = task.getPriority(),
            .type = task.getType()};
        auto assignee_opt = task.getAssignee();
        if (assignee_opt.has_value())
        {
            auto user_optional = assignee_opt.value();
            task_result.assignee = std::optional<UserResultSecureDto>(std::nullopt);
            if (user_optional.has_value())
            {
                auto user_ptr = user_optional.value();
                task_result.assignee = UserResultSecureDto::from_domain(*user_ptr);
            }
        }
        auto reporter_opt = task.getReporter();
        if (reporter_opt.has_value())
        {
            auto user_optional = reporter_opt.value();
            task_result.reporter = std::optional<UserResultSecureDto>(std::nullopt);
            if (user_optional.has_value())
            {
                auto user_ptr = user_optional.value();
                task_result.reporter = UserResultSecureDto::from_domain(*user_ptr);
            }
        }

        return task_result;
    }
};

class TaskResultSecureUpdateDto : public JsonDto<TaskResultSecureUpdateDto>
{
public:
    bool id;
    bool title;
    bool description;
    bool created_date;
    bool updated_date;
    bool due_date;
    bool assignee;
    bool reporter;
    bool labels;
    bool priority;
    bool type;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &TaskResultSecureUpdateDto::id),
            field("title", &TaskResultSecureUpdateDto::title),
            field("description", &TaskResultSecureUpdateDto::description),
            field("created_date", &TaskResultSecureUpdateDto::created_date),
            field("updated_date", &TaskResultSecureUpdateDto::updated_date),
            field("due_date", &TaskResultSecureUpdateDto::due_date),
            field("assignee", &TaskResultSecureUpdateDto::assignee),
            field("reporter", &TaskResultSecureUpdateDto::reporter),
            field("labels", &TaskResultSecureUpdateDto::labels),
            field("priority", &TaskResultSecureUpdateDto::priority),
            field("type", &TaskResultSecureUpdateDto::type),
        };
        return f;
    };
};