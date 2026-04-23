#pragma once

#include "modules/boardCategories/domain/BoardCategory.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/odb/BaseOdb.hpp"

#include "modules/tasks/adapters/persistance/TaskOdb.hpp"

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>

#include <memory>
#include <string>
#include <vector>

#pragma db object table("board_categories") pointer(std::shared_ptr)
class BoardCategoryOdb : public BaseOdb,
                         public IOdbMappable<BoardCategory, BoardCategoryOdb>,
                         public IOdbMappableQuery<BoardCategory, BoardCategoryOdb>
{
public:
    BoardCategoryOdb() = default;

    explicit BoardCategoryOdb(std::string title,
                              std::vector<odb::lazy_shared_ptr<TaskOdb>> tasks,
                              unsigned long id = 0)
        : title_(std::move(title)), tasks_(std::move(tasks)), BaseOdb(id) {}

    BoardCategoryOdb(const BoardCategoryOdb &) = default;
    BoardCategoryOdb(BoardCategoryOdb &&) noexcept = default;
    BoardCategoryOdb &operator=(BoardCategoryOdb &&) noexcept = default;
    BoardCategoryOdb &operator=(const BoardCategoryOdb &) = default;
    ~BoardCategoryOdb() = default;

    const std::string &title_value() const { return title_; }
    const std::vector<odb::lazy_shared_ptr<TaskOdb>> &tasks_value() const { return tasks_; }
    void add_task_ptr(odb::lazy_shared_ptr<TaskOdb> ptr) { tasks_.push_back(std::move(ptr)); }

    static BoardCategory to_domain(const BoardCategoryOdb &odb);
    static BoardCategoryOdb to_odb(const BoardCategory &domain);

protected:
    const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> get_loaders_map() const override;

private:
    friend class odb::access;

#pragma db column("title") type("VARCHAR(255)")
    std::string title_;

#pragma db value_not_null unordered    \
    table("board_category_tasks")      \
        id_column("board_category_id") \
            value_column("task_id")
    std::vector<odb::lazy_shared_ptr<TaskOdb>> tasks_;
};