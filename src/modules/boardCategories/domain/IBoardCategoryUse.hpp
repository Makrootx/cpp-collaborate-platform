#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>
#include "shared/domain/BaseWrapper.hpp"
#include "modules/tasks/domain/Task.hpp"

class BoardCategory;
class ITaskUse;
class ITaskSecureUse;

class IBoardCategoryUse : public virtual BaseDomainUse<BoardCategory>
{
public:
    virtual ~IBoardCategoryUse() = default;

    virtual std::string get_title() const = 0;
    virtual void set_title(std::string v) = 0;

    virtual std::optional<std::vector<std::shared_ptr<ITaskUse>>> get_tasks() const = 0;
    virtual void set_tasks(std::optional<std::vector<Task>> v) = 0;
};

class IBoardCategorySecureUse : public virtual BaseDomainSecureUse<BoardCategory>
{
public:
    virtual ~IBoardCategorySecureUse() = default;

    virtual std::optional<long int> get_id() const = 0;
    virtual bool set_id(long int id) = 0;

    virtual std::optional<std::string> get_title() const = 0;
    virtual bool set_title(std::string v) = 0;

    virtual std::optional<std::optional<std::vector<std::shared_ptr<ITaskSecureUse>>>> get_tasks() const = 0;
    virtual bool set_tasks(std::optional<std::vector<Task>> v) = 0;
};
