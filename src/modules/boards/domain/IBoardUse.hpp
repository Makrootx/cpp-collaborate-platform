#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>
#include "shared/domain/BaseWrapper.hpp"

class Board;
class BoardCategory;
class IBoardCategoryUse;
class IBoardCategorySecureUse;

class IBoardUse : public virtual BaseDomainUse<Board>
{
public:
    virtual ~IBoardUse() = default;

    virtual std::string get_title() const = 0;
    virtual void set_title(std::string v) = 0;

    virtual std::string get_description() const = 0;
    virtual void set_description(std::string v) = 0;

    virtual std::optional<std::vector<std::shared_ptr<IBoardCategoryUse>>> get_categories() const = 0;
    virtual void set_categories(std::optional<std::vector<std::shared_ptr<BoardCategory>>> v) = 0;
};

class IBoardSecureUse : public virtual BaseDomainSecureUse<Board>
{
public:
    virtual ~IBoardSecureUse() = default;

    virtual std::optional<long int> get_id() const = 0;
    virtual bool set_id(long int id) = 0;

    virtual std::optional<std::string> get_title() const = 0;
    virtual bool set_title(std::string v) = 0;

    virtual std::optional<std::string> get_description() const = 0;
    virtual bool set_description(std::string v) = 0;

    virtual std::optional<std::optional<std::vector<std::shared_ptr<IBoardCategorySecureUse>>>> get_categories() const = 0;
    virtual bool set_categories(std::optional<std::vector<std::shared_ptr<BoardCategory>>> v) = 0;
};
