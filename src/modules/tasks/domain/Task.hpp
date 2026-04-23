#pragma once

#include "shared/domain/BaseDomain.hpp"
#include "modules/users/domain/User.hpp"
#include "modules/tasks/domain/TaskPriority.hpp"
#include "modules/tasks/domain/ITaskUse.hpp"
#include <string>
#include <ctime>
#include <optional>
#include <vector>
#include <unordered_set>

class Task : public BaseDomain, virtual public ITaskUse
{

public:
    explicit Task(std::string title,
                  std::string description,
                  std::tm create_date,
                  std::tm update_date,
                  std::optional<std::tm> due_date = std::nullopt,
                  std::optional<std::shared_ptr<User>> assignee = std::nullopt,
                  std::optional<std::shared_ptr<User>> reporter = std::nullopt,
                  std::unordered_set<std::string> labels = {},
                  TaskPriority priority = TaskPriority::Medium,
                  std::optional<std::string> type = std::nullopt, unsigned long id = 0)
        : title(std::move(title)), description(std::move(description)),
          create_date(create_date), update_date(update_date),
          due_date(std::move(due_date)), assignee(std::move(assignee)),
          reporter(std::move(reporter)), labels(std::move(labels)),
          priority(priority), type(std::move(type)), BaseDomain(id) {}

    Task(const Task &) = default;
    Task(Task &&) noexcept = default;
    Task &operator=(Task &&) noexcept = default;
    Task &operator=(const Task &) = default;
    ~Task() = default;

    std::string getTitle() const override { return title; };
    void setTitle(std::string title) override { this->title = std::move(title); };

    std::string getDescription() const override { return description; };
    void setDescription(std::string description) override { this->description = std::move(description); };

    std::tm getCreatedDate() const override { return create_date; };

    std::tm getUpdatedDate() const override { return update_date; };
    void setUpdatedDate(std::tm date) override { this->update_date = std::move(date); };

    std::optional<std::tm> getDueDate() const override { return due_date; };
    void setDueDate(std::optional<std::tm> date) override { this->due_date = std::move(date); };

    std::optional<std::shared_ptr<IUserUse>> getAssignee() const override
    {
        if (!assignee.has_value())
        {
            return std::nullopt;
        }
        // auto user_ptr =
        return std::optional<std::shared_ptr<IUserUse>>{assignee.value()};
    };
    void setAssignee(std::shared_ptr<User> user) override
    {
        assignee = std::move(user);
    };

    std::optional<std::shared_ptr<IUserUse>> getReporter() const override
    {
        if (!reporter.has_value())
        {
            return std::nullopt;
        }
        // auto user_ptr = std::make_shared<User>(reporter.value());
        return std::optional<std::shared_ptr<IUserUse>>{reporter.value()};
    };
    void setReporter(std::shared_ptr<User> user) override
    {
        reporter = std::move(user);
    };

    std::vector<std::string> getLabels() const override { return std::vector<std::string>(labels.begin(), labels.end()); };
    void addLabels(std::vector<std::string> labels) override
    {
        for (const auto &label : labels)
        {
            this->labels.insert(label);
        }
    };
    void removeLabels(std::vector<std::string> labels) override
    {
        for (const auto &label : labels)
        {
            this->labels.erase(label);
        }
    };

    TaskPriority getPriority() const override { return priority; };
    void setPriority(TaskPriority priority) override { this->priority = std::move(priority); };

    std::string getType() const override { return type.value_or(""); };
    void setType(std::string type) override { this->type = std::move(type); };

    Task to_domain() override { return *this; }

protected:
    std::string title;
    std::string description;

    std::tm create_date;
    std::tm update_date;
    std::optional<std::tm> due_date; /*can be null; non-connected to db*/

    std::optional<std::shared_ptr<User>> assignee; /*can be null; can be not loaded from db*/
    std::optional<std::shared_ptr<User>> reporter; /*can be null; can be not loaded from db*/
    std::unordered_set<std::string> labels;        /*SEPARATE TABLE IMPLEMENTATION*/
    TaskPriority priority;                         /*IDEALLY SEPARATE TABLE ENUM*/
    std::optional<std::string> type;               /*SEPARATE TABLE IMPLEMENTATION*/
};