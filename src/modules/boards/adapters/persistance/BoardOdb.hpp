#pragma once

#include "modules/boards/domain/Board.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/odb/BaseOdb.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>

#include <memory>
#include <string>
#include <vector>

#pragma db object table("boards") pointer(std::shared_ptr)
class BoardOdb : public BaseOdb,
                 public IOdbMappable<Board, BoardOdb>,
                 public IOdbMappableQuery<Board, BoardOdb>
{
public:
    BoardOdb() = default;

    explicit BoardOdb(std::string title,
                      std::string description,
                      unsigned long id = 0)
        : title_(std::move(title)), description_(std::move(description)),
          BaseOdb(id) {}

    BoardOdb(const BoardOdb &) = default;
    BoardOdb(BoardOdb &&) noexcept = default;
    BoardOdb &operator=(BoardOdb &&) noexcept = default;
    BoardOdb &operator=(const BoardOdb &) = default;
    ~BoardOdb() = default;

    const std::string &title_value() const { return title_; }
    const std::string &description_value() const { return description_; }
    const std::vector<odb::lazy_shared_ptr<BoardCategoryOdb>> &categories_value() const { return categories_; }
    void add_category_ptr(odb::lazy_shared_ptr<BoardCategoryOdb> ptr) { categories_.push_back(std::move(ptr)); }

    static Board to_domain(const BoardOdb &odb);
    static BoardOdb to_odb(const Board &domain);

protected:
    const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> get_loaders_map() const override;

private:
    friend class odb::access;

#pragma db column("title") type("VARCHAR(255)")
    std::string title_;

#pragma db column("description") type("TEXT")
    std::string description_;

#pragma db value_not_null unordered \
    table("board_category_boards")  \
        id_column("board_id")       \
            value_column("board_category_id")
    std::vector<odb::lazy_shared_ptr<BoardCategoryOdb>> categories_;
};