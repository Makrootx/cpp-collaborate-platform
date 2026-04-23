#pragma once

#include <string>
#include <ctime>
#include <optional>
#include <vector>
#include <tuple>
#include "modules/users/domain/User.hpp"
#include "modules/tasks/domain/TaskPriority.hpp"

#include "shared/domain/BaseWrapper.hpp"

class Task;

class ITaskUse : public virtual BaseDomainUse<Task>
{
public:
    virtual ~ITaskUse() = default;

    virtual std::string getTitle() const = 0;
    virtual void setTitle(std::string title) = 0;

    virtual std::string getDescription() const = 0;
    virtual void setDescription(std::string description) = 0;

    virtual std::tm getCreatedDate() const = 0;

    virtual std::tm getUpdatedDate() const = 0;
    virtual void setUpdatedDate(std::tm date) = 0;

    virtual std::optional<std::tm> getDueDate() const = 0;
    virtual void setDueDate(std::optional<std::tm> date) = 0;

    virtual std::optional<std::shared_ptr<IUserUse>> getAssignee() const = 0;
    virtual void setAssignee(std::shared_ptr<User> user) = 0;

    virtual std::optional<std::shared_ptr<IUserUse>> getReporter() const = 0;
    virtual void setReporter(std::shared_ptr<User> user) = 0;

    virtual std::vector<std::string> getLabels() const = 0;
    virtual void addLabels(std::vector<std::string> labels) = 0;
    virtual void removeLabels(std::vector<std::string> labels) = 0;

    virtual TaskPriority getPriority() const = 0;
    virtual void setPriority(TaskPriority priority) = 0;

    virtual std::string getType() const = 0;
    virtual void setType(std::string type) = 0;
};

class ITaskSecureUse : public virtual BaseDomainSecureUse<Task>
{
public:
    virtual ~ITaskSecureUse() = default;

    virtual std::optional<std::string> getTitle() const = 0;
    virtual bool setTitle(std::string title) = 0;

    virtual std::optional<std::string> getDescription() const = 0;
    virtual bool setDescription(std::string description) = 0;

    virtual std::optional<std::tm> getCreatedDate() const = 0;

    virtual std::optional<std::tm> getUpdatedDate() const = 0;
    virtual bool setUpdatedDate(std::tm date) = 0;

    virtual std::optional<std::optional<std::tm>> getDueDate() const = 0;
    virtual bool setDueDate(std::optional<std::tm> date) = 0;

    virtual std::optional<std::optional<std::shared_ptr<IUserSecureUse>>> getAssignee() const = 0;
    virtual bool setAssignee(std::shared_ptr<User> user) = 0;

    virtual std::optional<std::optional<std::shared_ptr<IUserSecureUse>>> getReporter() const = 0;
    virtual bool setReporter(std::shared_ptr<User> user) = 0;

    virtual std::optional<std::vector<std::string>> getLabels() const = 0;
    virtual bool addLabels(std::vector<std::string> labels) = 0;
    virtual bool removeLabels(std::vector<std::string> labels) = 0;

    virtual std::optional<TaskPriority> getPriority() const = 0;
    virtual bool setPriority(TaskPriority priority) = 0;

    virtual std::optional<std::string> getType() const = 0;
    virtual bool setType(std::string type) = 0;
};