#pragma once

#include "shared/helpers/json/JsonDto.hpp"
#include "modules/boardCategories/domain/BoardCategory.hpp"
#include "modules/boardCategories/domain/IBoardCategoryUse.hpp"
#include "modules/tasks/adapters/dto/TaskDto.hpp"

class BoardCategoryCreateDto : public JsonDto<BoardCategoryCreateDto>
{
public:
    std::string title;
    long int board_id;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("title", &BoardCategoryCreateDto::title),
            field("board_id", &BoardCategoryCreateDto::board_id),
        };
        return f;
    }
};

class BoardCategoryUpdateDto : public JsonDto<BoardCategoryUpdateDto>
{
public:
    long int id;
    std::optional<std::string> title;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &BoardCategoryUpdateDto::id),
            optional_field("title", &BoardCategoryUpdateDto::title),
        };
        return f;
    }
};

class BoardCategoryResultDto : public JsonDto<BoardCategoryResultDto>
{
public:
    long int id;
    std::string title;
    std::optional<long int> board_id;
    std::optional<std::vector<TaskResultDto>> tasks;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &BoardCategoryResultDto::id),
            field("title", &BoardCategoryResultDto::title),
            optional_field("board_id", &BoardCategoryResultDto::board_id),
            optional_nested_field("tasks", &BoardCategoryResultDto::tasks),
        };
        return f;
    }

    static BoardCategoryResultDto from_domain(const IBoardCategoryUse &cat)
    {
        std::optional<std::vector<TaskResultDto>> tasks_dto;
        auto tasks_opt = cat.get_tasks();
        if (tasks_opt.has_value())
        {
            tasks_dto = std::vector<TaskResultDto>{};
            for (const auto &t : *tasks_opt)
                tasks_dto->push_back(TaskResultDto::from_domain(*t));
        }
        return BoardCategoryResultDto{
            .id = cat.get_id(),
            .title = cat.get_title(),
            .board_id = std::nullopt,
            .tasks = tasks_dto};
    }
};

class BoardCategoryResultSecureDto : public JsonDto<BoardCategoryResultSecureDto>
{
public:
    std::optional<long int> id;
    std::optional<std::string> title;
    std::optional<std::optional<std::vector<TaskResultSecureDto>>> tasks;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field_secured("id", &BoardCategoryResultSecureDto::id),
            field_secured("title", &BoardCategoryResultSecureDto::title),
            optional_nested_field_secured("tasks", &BoardCategoryResultSecureDto::tasks),
        };
        return f;
    }

    BoardCategoryResultSecureDto merge_with_plain(const BoardCategoryResultDto &plain)
    {
        if (id.has_value())
            id = plain.id;
        if (title.has_value())
            title = plain.title;
        if (tasks.has_value())
        {
            if (tasks.value().has_value())
            {
                auto tasks_vec = tasks.value().value();
                for (int i = 0; i < tasks_vec.size(); i++)
                {
                    tasks_vec[i].merge_with_plain(plain.tasks.value()[i]);
                }
            }
            else
            {
                tasks = std::optional<std::optional<std::vector<TaskResultSecureDto>>>(std::nullopt);
            }
        }
        return *this;
    }

    static BoardCategoryResultSecureDto from_domain(const IBoardCategorySecureUse &cat)
    {
        auto result = BoardCategoryResultSecureDto{
            .id = cat.get_id(),
            .title = cat.get_title()};

        auto tasks_opt = cat.get_tasks();
        if (tasks_opt.has_value())
        {
            auto tasks_optional = tasks_opt.value();
            result.tasks = std::optional<std::vector<TaskResultSecureDto>>(std::nullopt);
            if (tasks_optional.has_value())
            {
                result.tasks = std::vector<TaskResultSecureDto>{};
                for (const auto &t : *tasks_optional)
                    result.tasks.value()->push_back(TaskResultSecureDto::from_domain(*t));
            }
        }
        return result;
    }
};

class BoardCategoryResultSecureUpdateDto : public JsonDto<BoardCategoryResultSecureUpdateDto>
{
public:
    bool id = false;
    bool title = false;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &BoardCategoryResultSecureUpdateDto::id),
            field("title", &BoardCategoryResultSecureUpdateDto::title),
        };
        return f;
    }
};
