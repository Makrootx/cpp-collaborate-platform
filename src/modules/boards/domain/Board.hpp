#pragma once

#include "shared/domain/BaseDomain.hpp"
#include "modules/boards/domain/IBoardUse.hpp"
#include "modules/boardCategories/domain/IBoardCategoryUse.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class BoardCategory; // forward declare — full type in boardCategories module

class Board : public BaseDomain, virtual public IBoardUse
{
public:
    explicit Board(std::string title,
                   std::string description,
                   std::optional<std::vector<std::shared_ptr<BoardCategory>>> categories = std::nullopt,
                   unsigned long id = 0)
        : title(std::move(title)), description(std::move(description)),
          categories(std::move(categories)), BaseDomain(id) {}

    Board(const Board &) = default;
    Board(Board &&) noexcept = default;
    Board &operator=(Board &&) noexcept = default;
    Board &operator=(const Board &) = default;
    ~Board() = default;

    std::string get_title() const override { return title; }
    void set_title(std::string v) override { title = std::move(v); }

    std::string get_description() const override { return description; }
    void set_description(std::string v) override { description = std::move(v); }

    const std::optional<std::vector<std::shared_ptr<BoardCategory>>> &get_categories_raw() const { return categories; }
    void set_categories(std::optional<std::vector<std::shared_ptr<BoardCategory>>> v) override { categories = std::move(v); }

    std::optional<std::vector<std::shared_ptr<IBoardCategoryUse>>> get_categories() const override
    {
        if (!categories.has_value())
            return std::nullopt;
        std::vector<std::shared_ptr<IBoardCategoryUse>> result;
        result.reserve(categories->size());
        for (const auto &cat : *categories)
            result.push_back(cat);
        return result;
    }

    Board to_domain() override { return *this; }

protected:
    std::string title;
    std::string description;
    std::optional<std::vector<std::shared_ptr<BoardCategory>>> categories;
};