#include "modules/users/adapters/persistence/UserOdb-odb.hxx"
#include "modules/tasks/adapters/persistance/TaskOdb-odb.hxx"
#include "modules/tasks/adapters/persistance/TaskOdb.hpp"

#include <optional>

namespace
{
    std::tm ptime_to_tm(const boost::posix_time::ptime &value)
    {
        return boost::posix_time::to_tm(value);
    }

    boost::posix_time::ptime tm_to_ptime(std::tm value)
    {
        return boost::posix_time::ptime_from_tm(value);
    }
}

Task TaskOdb::to_domain(const TaskOdb &odb)
{
    std::optional<std::shared_ptr<User>> assignee;
    if (odb.is_field_included("assignee") && odb.assignee_)
    {
        assignee = std::make_shared<User>(UserOdb::to_domain(*odb.assignee_));
    }

    std::optional<std::shared_ptr<User>> reporter;
    if (odb.is_field_included("reporter") && odb.reporter_)
    {
        reporter = std::make_shared<User>(UserOdb::to_domain(*odb.reporter_));
    }

    return Task(
        odb.title_value(),
        odb.description_value(),
        ptime_to_tm(odb.create_date_value()),
        ptime_to_tm(odb.update_date_value()),
        odb.due_date_value().has_value() ? std::optional<std::tm>(ptime_to_tm(odb.due_date_value().value())) : std::nullopt,
        assignee,
        reporter,
        odb.labels_value(),
        odb.priority_value(),
        odb.type_value());
}

const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> TaskOdb::get_loaders_map() const
{
    std::map<std::string, std::function<std::vector<Loader>()>> map;
    map["assignee"] = [this]()
    {
        return BaseOdb::to_loaders(this->assignee_);
    };
    map["reporter"] = [this]()
    {
        return BaseOdb::to_loaders(this->reporter_);
    };
    return map;
}

TaskOdb TaskOdb::to_odb(const Task &domain)
{
    TaskOdb odb(
        domain.getTitle(),
        domain.getDescription(),
        tm_to_ptime(domain.getCreatedDate()),
        tm_to_ptime(domain.getUpdatedDate()),
        domain.getDueDate().has_value() ? std::optional<boost::posix_time::ptime>(tm_to_ptime(domain.getDueDate().value())) : std::nullopt,
        std::unordered_set<std::string>(domain.getLabels().begin(), domain.getLabels().end()),
        domain.getPriority(),
        domain.getType(),
        domain.get_id());

    if (domain.getAssignee().has_value())
    {
        odb.assignee_ = std::make_shared<UserOdb>(UserOdb::to_odb(*std::dynamic_pointer_cast<User>(domain.getAssignee().value())));
    }

    if (domain.getReporter().has_value())
    {
        odb.reporter_ = std::make_shared<UserOdb>(UserOdb::to_odb(*std::dynamic_pointer_cast<User>(domain.getReporter().value())));
    }

    return odb;
}