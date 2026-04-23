#pragma once

#include "modules/tasks/domain/Task.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/odb/BaseOdb.hpp"
#include "modules/users/adapters/persistence/UserOdb.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>

#include <memory>

#pragma db object table("tasks") pointer(std::shared_ptr)
class TaskOdb : public BaseOdb, public IOdbMappable<Task, TaskOdb>, public IOdbMappableQuery<Task, TaskOdb>
{
public:
    TaskOdb() = default;

    explicit TaskOdb(std::string title,
                     std::string description,
                     boost::posix_time::ptime create_date,
                     boost::posix_time::ptime update_date,
                     std::optional<boost::posix_time::ptime> due_date = std::nullopt,
                     std::unordered_set<std::string> labels = {},
                     TaskPriority priority = TaskPriority::Medium,
                     std::optional<std::string> type = std::nullopt, unsigned long id = 0)
        : title_(std::move(title)), description_(std::move(description)),
          create_date_(create_date), update_date_(update_date),
          due_date_(due_date ? odb::nullable<boost::posix_time::ptime>(*due_date) : odb::nullable<boost::posix_time::ptime>()),
          labels_(std::move(labels)),
          priority_(priority),
          type_(type ? odb::nullable<std::string>(*type) : odb::nullable<std::string>()),
          BaseOdb(id) {}

    TaskOdb(const TaskOdb &) = default;
    TaskOdb(TaskOdb &&) noexcept = default;
    TaskOdb &operator=(TaskOdb &&) noexcept = default;
    TaskOdb &operator=(const TaskOdb &) = default;
    ~TaskOdb() = default;

    const std::string &title_value() const { return title_; }
    const std::string &description_value() const { return description_; }
    const boost::posix_time::ptime &create_date_value() const { return create_date_; }
    const boost::posix_time::ptime &update_date_value() const { return update_date_; }
    std::optional<boost::posix_time::ptime> due_date_value() const
    {
        return due_date_.null() ? std::nullopt : std::optional<boost::posix_time::ptime>(due_date_.get());
    }
    const odb::lazy_shared_ptr<UserOdb> &assignee_value() const { return assignee_; }
    const odb::lazy_shared_ptr<UserOdb> &reporter_value() const { return reporter_; }
    const std::unordered_set<std::string> &labels_value() const { return labels_; }
    TaskPriority priority_value() const { return priority_; }
    std::optional<std::string> type_value() const
    {
        return type_.null() ? std::nullopt : std::optional<std::string>(type_.get());
    }

    static Task to_domain(const TaskOdb &odb);
    static TaskOdb to_odb(const Task &domain);

protected:
    const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> get_loaders_map() const override;

private:
    friend class odb::access;

#pragma db column("title") type("VARCHAR(255)")
    std::string title_;
#pragma db column("description") type("TEXT")
    std::string description_;
#pragma db column("create_date") type("TIMESTAMP")
    boost::posix_time::ptime create_date_;
#pragma db column("update_date") type("TIMESTAMP")
    boost::posix_time::ptime update_date_;
#pragma db column("due_date") type("TIMESTAMP") null
    odb::nullable<boost::posix_time::ptime> due_date_;

#pragma db column("assignee_id") null
    odb::lazy_shared_ptr<UserOdb> assignee_;

#pragma db column("reporter_id") null
    odb::lazy_shared_ptr<UserOdb> reporter_;

#pragma db column("priority") type("INT")
    TaskPriority priority_;
#pragma db column("type") type("VARCHAR(255)") null
    odb::nullable<std::string> type_;

#pragma db value_not_null unordered \
    table("task_labels")            \
        id_column("task_id")        \
            value_column("label")
    std::unordered_set<std::string> labels_;
};