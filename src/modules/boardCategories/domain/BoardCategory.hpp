#pragma once

#include "shared/domain/BaseDomain.hpp"
#include "modules/boardCategories/domain/IBoardCategoryUse.hpp"
#include "modules/tasks/domain/Task.hpp"
#include "modules/tasks/domain/ITaskUse.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Board; // forward declare — full type in boards module

class BoardCategory : public BaseDomain, virtual public IBoardCategoryUse
{
public:
    explicit BoardCategory(std::string title,
                           std::optional<std::vector<Task>> tasks = std::nullopt,
                           std::optional<std::shared_ptr<Board>> connected_board = std::nullopt,
                           unsigned long id = 0)
        : title(std::move(title)), tasks(std::move(tasks)),
          connected_board(std::move(connected_board)), BaseDomain(id) {}

    BoardCategory(const BoardCategory &) = default;
    BoardCategory(BoardCategory &&) noexcept = default;
    BoardCategory &operator=(BoardCategory &&) noexcept = default;
    BoardCategory &operator=(const BoardCategory &) = default;
    ~BoardCategory() = default;

    std::string get_title() const override { return title; }
    void set_title(std::string v) override { title = std::move(v); }

    const std::optional<std::vector<Task>> &get_tasks_raw() const { return tasks; }
    void set_tasks(std::optional<std::vector<Task>> v) override { tasks = std::move(v); }

    std::optional<std::vector<std::shared_ptr<ITaskUse>>> get_tasks() const override
    {
        if (!tasks.has_value())
            return std::nullopt;
        std::vector<std::shared_ptr<ITaskUse>> result;
        result.reserve(tasks->size());
        for (const auto &t : *tasks)
            result.push_back(std::make_shared<Task>(t));
        return result;
    }

    const std::optional<std::shared_ptr<Board>> &get_connected_board() const { return connected_board; }
    void set_connected_board(std::optional<std::shared_ptr<Board>> v) { connected_board = std::move(v); }

    BoardCategory to_domain() override { return *this; }

protected:
    std::string title;
    std::optional<std::vector<Task>> tasks;
    std::optional<std::shared_ptr<Board>> connected_board;
};
