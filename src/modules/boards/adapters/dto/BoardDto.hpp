#pragma once

#include "shared/helpers/json/JsonDto.hpp"
#include "modules/boards/domain/Board.hpp"
#include "modules/boards/domain/IBoardUse.hpp"
#include "modules/boardCategories/adapters/dto/BoardCategoryDto.hpp"

class BoardCreateDto : public JsonDto<BoardCreateDto>
{
public:
    std::string title;
    std::string description;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("title", &BoardCreateDto::title),
            field("description", &BoardCreateDto::description),
        };
        return f;
    }
};

class BoardUpdateDto : public JsonDto<BoardUpdateDto>
{
public:
    long int id;
    std::optional<std::string> title;
    std::optional<std::string> description;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &BoardUpdateDto::id),
            optional_field("title", &BoardUpdateDto::title),
            optional_field("description", &BoardUpdateDto::description),
        };
        return f;
    }
};

class BoardResultDto : public JsonDto<BoardResultDto>
{
public:
    long int id;
    std::string title;
    std::string description;
    std::optional<std::vector<BoardCategoryResultDto>> categories;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &BoardResultDto::id),
            field("title", &BoardResultDto::title),
            field("description", &BoardResultDto::description),
            optional_nested_field("categories", &BoardResultDto::categories),
        };
        return f;
    }

    static BoardResultDto from_domain(const IBoardUse &board)
    {
        std::optional<std::vector<BoardCategoryResultDto>> cats;
        auto cats_opt = board.get_categories();
        if (cats_opt.has_value())
        {
            cats = std::vector<BoardCategoryResultDto>{};
            for (const auto &cat_ptr : *cats_opt)
                cats->push_back(BoardCategoryResultDto::from_domain(*cat_ptr));
        }
        return BoardResultDto{
            .id = board.get_id(),
            .title = board.get_title(),
            .description = board.get_description(),
            .categories = cats};
    }
};

class BoardResultSecureDto : public JsonDto<BoardResultSecureDto>
{
public:
    std::optional<long int> id;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::optional<std::vector<BoardCategoryResultSecureDto>>> categories;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field_secured("id", &BoardResultSecureDto::id),
            field_secured("title", &BoardResultSecureDto::title),
            field_secured("description", &BoardResultSecureDto::description),
            optional_nested_field_secured("categories", &BoardResultSecureDto::categories),
        };
        return f;
    }

    BoardResultSecureDto merge_with_plain(const BoardResultDto &plain)
    {
        if (id.has_value())
            id = plain.id;
        if (title.has_value())
            title = plain.title;
        if (description.has_value())
            description = plain.description;
        if (categories.has_value())
        {
            if (categories.value().has_value())
            {
                auto tasks_vec = categories.value().value();
                for (int i = 0; i < tasks_vec.size(); i++)
                {
                    tasks_vec[i].merge_with_plain(plain.categories.value()[i]);
                }
            }
            else
            {
                categories = std::optional<std::optional<std::vector<BoardCategoryResultSecureDto>>>(std::nullopt);
            }
        }
        return *this;
    }

    static BoardResultSecureDto from_domain(const IBoardSecureUse &board)
    {
        auto result = BoardResultSecureDto{
            .id = board.get_id(),
            .title = board.get_title(),
            .description = board.get_description()};
        auto cat_opt = board.get_categories();
        if (cat_opt.has_value())
        {
            auto cat_optional = cat_opt.value();
            result.categories = std::optional<std::vector<BoardCategoryResultSecureDto>>(std::nullopt);
            if (cat_optional.has_value())
            {
                result.categories = std::vector<BoardCategoryResultSecureDto>{};
                for (const auto &cat : *cat_optional)
                    result.categories.value()->push_back(BoardCategoryResultSecureDto::from_domain(*cat));
            }
        }
        return result;
    }
};

class BoardResultSecureUpdateDto : public JsonDto<BoardResultSecureUpdateDto>
{
public:
    bool id = false;
    bool title = false;
    bool description = false;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &BoardResultSecureUpdateDto::id),
            field("title", &BoardResultSecureUpdateDto::title),
            field("description", &BoardResultSecureUpdateDto::description),
        };
        return f;
    }
};
