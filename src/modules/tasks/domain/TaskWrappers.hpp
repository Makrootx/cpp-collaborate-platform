#pragma once

#include "shared/domain/AutomativeWrapper.hpp"
#include "shared/domain/SecureWrapper.hpp"
#include "modules/tasks/domain/Task.hpp"
#include "modules/users/domain/UserWrappers.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"
#include "shared/helpers/TmStringUtils.hpp"

class AutomativeTask : public AutomativeWrapper<ITaskUse>, public ITaskUse
{
public:
    using AutomativeWrapper<ITaskUse>::AutomativeWrapper;
    using TaskP = EntitySchemas::EntityAccessor::TaskSchema;
    using TaskAtr = TaskP::Attributes;
    using TaskRel = TaskP::Relations;

    std::string automation_entity() const override { return EntitySchemas::EntityAccessor::TASKS; }

    std::unordered_map<std::string, FieldAccessor> field_accessors() override
    {
        return {
            {TaskAtr::ID, FieldAccessor{.get = [this]()
                                        { return wrapper_->get_id(); },
                                        .set = [this](std::optional<LiteralValue> value)
                                        { wrapper_->set_id(static_cast<long int>(std::get<int>(value.value()))); }}},
            {TaskAtr::TITLE, FieldAccessor{.get = [this]()
                                           { return wrapper_->getTitle(); },
                                           .set = [this](std::optional<LiteralValue> value)
                                           { wrapper_->setTitle(std::get<std::string>(value.value())); }}},
            {TaskAtr::DESCRIPTION, FieldAccessor{.get = [this]()
                                                 { return wrapper_->getDescription(); },
                                                 .set = [this](std::optional<LiteralValue> value)
                                                 { wrapper_->setDescription(std::get<std::string>(value.value())); }}},
            {TaskAtr::DUE_DATE, FieldAccessor{.get = [this]()
                                              { 
                                                auto due_date = wrapper_->getDueDate();
                                                return due_date.has_value() ? std::optional<std::string>{TmStringUtils::to_string(due_date.value())} : std::nullopt; },
                                              .set = [this](std::optional<LiteralValue> value)
                                              {
                                                if (value.has_value())
                                                {
                                                    wrapper_->setDueDate(std::make_optional(TmStringUtils::from_string(std::get<std::string>(value.value()))));
                                                }
                                                else
                                                {
                                                    wrapper_->setDueDate(std::nullopt);
                                                } }}},
            {TaskAtr::PRIORITY, FieldAccessor{.get = [this]()
                                              { return wrapper_->getPriority(); },
                                              .set = [this](std::optional<LiteralValue> value)
                                              { wrapper_->setPriority(TaskPriorityUtils::from_string(std::get<std::string>(value.value()))); }}},
            {TaskAtr::TYPE, FieldAccessor{.get = [this]()
                                          { return wrapper_->getType(); },
                                          .set = [this](std::optional<LiteralValue> value)
                                          { wrapper_->setType(std::get<std::string>(value.value())); }}},
        };
    }

    long int get_id() const override { return perform(&ITaskUse::get_id, read_trigger(TaskAtr::ID)); }
    void set_id(long int id) override { perform(&ITaskUse::set_id, update_trigger(TaskAtr::ID), id); };

    std::string getTitle() const override { return perform(&ITaskUse::getTitle, read_trigger(TaskAtr::TITLE)); }
    void setTitle(std::string title) override { perform(&ITaskUse::setTitle, update_trigger(TaskAtr::TITLE), std::move(title)); }

    std::string getDescription() const override { return perform(&ITaskUse::getDescription, read_trigger(TaskAtr::DESCRIPTION)); }
    void setDescription(std::string description) override { perform(&ITaskUse::setDescription, update_trigger(TaskAtr::DESCRIPTION), std::move(description)); }

    std::tm getCreatedDate() const override { return perform(&ITaskUse::getCreatedDate, read_trigger(TaskAtr::CREATE_DATE)); }
    std::tm getUpdatedDate() const override { return perform(&ITaskUse::getUpdatedDate, read_trigger(TaskAtr::UPDATE_DATE)); }
    void setUpdatedDate(std::tm date) override { perform(&ITaskUse::setUpdatedDate, update_trigger(TaskAtr::UPDATE_DATE), std::move(date)); }
    std::optional<std::tm> getDueDate() const override { return perform(&ITaskUse::getDueDate, read_trigger(TaskAtr::DUE_DATE)); }
    void setDueDate(std::optional<std::tm> date) override { perform(&ITaskUse::setDueDate, update_trigger(TaskAtr::DUE_DATE), std::move(date)); }

    std::optional<std::shared_ptr<IUserUse>> getAssignee() const override
    {

        auto assignee = perform(&ITaskUse::getAssignee, read_trigger(TaskRel::ASSIGNEE));
        if (assignee.has_value() && assignee.value())
        {
            return std::make_shared<AutomativeUser>(std::move(assignee.value()), space_id_);
        }
        return assignee;
    }
    void setAssignee(std::shared_ptr<User> user) override { perform(&ITaskUse::setAssignee, update_trigger(TaskRel::ASSIGNEE), std::move(user)); }

    std::optional<std::shared_ptr<IUserUse>> getReporter() const override
    {
        auto reporter = perform(&ITaskUse::getReporter, read_trigger(TaskRel::REPORTER));
        if (reporter.has_value() && reporter.value())
        {
            return std::make_shared<AutomativeUser>(std::move(reporter.value()), space_id_);
        }
        return reporter;
    }
    void setReporter(std::shared_ptr<User> user) override { perform(&ITaskUse::setReporter, update_trigger(TaskRel::REPORTER), std::move(user)); }

    std::vector<std::string> getLabels() const override { return perform(&ITaskUse::getLabels, read_trigger(TaskAtr::LABELS)); }
    void addLabels(std::vector<std::string> labels) override { perform(&ITaskUse::addLabels, update_trigger(TaskAtr::LABELS), std::move(labels)); }
    void removeLabels(std::vector<std::string> labels) override { perform(&ITaskUse::removeLabels, update_trigger(TaskAtr::LABELS), std::move(labels)); }

    TaskPriority getPriority() const override { return perform(&ITaskUse::getPriority, read_trigger(TaskAtr::PRIORITY)); }
    void setPriority(TaskPriority priority) override { perform(&ITaskUse::setPriority, update_trigger(TaskAtr::PRIORITY), priority); }

    std::string getType() const override { return perform(&ITaskUse::getType, read_trigger(TaskAtr::TYPE)); }
    void setType(std::string type) override { perform(&ITaskUse::setType, update_trigger(TaskAtr::TYPE), std::move(type)); }
};

class SecureTask : public SecureWrapper<ITaskUse>, public ITaskSecureUse
{
public:
    using TaskP = EntitySchemas::EntityAccessor::TaskSchema;
    using TaskAtr = TaskP::Attributes;
    using TaskRel = TaskP::Relations;
    using SecureWrapper<ITaskUse>::SecureWrapper;

    std::string get_domain_permission_name() const override { return EntitySchemas::EntityAccessor::TASKS; }

    std::optional<long int> get_id() const override { return secure_get(&ITaskUse::get_id, PermissionFactor::get_read_factor(TaskAtr::ID)); }
    bool set_id(long int id) override { return secure_set(&ITaskUse::set_id, PermissionFactor::get_update_factor(TaskAtr::ID), id); };

    std::optional<std::string> getTitle() const override { return secure_get(&ITaskUse::getTitle, PermissionFactor::get_read_factor(TaskAtr::TITLE)); }
    bool setTitle(std::string title) override { return secure_set(&ITaskUse::setTitle, PermissionFactor::get_update_factor(TaskAtr::TITLE), std::move(title)); }

    std::optional<std::string> getDescription() const override { return secure_get(&ITaskUse::getDescription, PermissionFactor::get_read_factor(TaskAtr::DESCRIPTION)); }
    bool setDescription(std::string description) override { return secure_set(&ITaskUse::setDescription, PermissionFactor::get_update_factor(TaskAtr::DESCRIPTION), std::move(description)); }

    std::optional<std::tm> getCreatedDate() const override { return secure_get(&ITaskUse::getCreatedDate, PermissionFactor::get_read_factor(TaskAtr::CREATE_DATE)); }
    std::optional<std::tm> getUpdatedDate() const override { return secure_get(&ITaskUse::getUpdatedDate, PermissionFactor::get_read_factor(TaskAtr::UPDATE_DATE)); }
    bool setUpdatedDate(std::tm date) override { return secure_set(&ITaskUse::setUpdatedDate, PermissionFactor::get_update_factor(TaskAtr::UPDATE_DATE), std::move(date)); }
    std::optional<std::optional<std::tm>> getDueDate() const override { return secure_get(&ITaskUse::getDueDate, PermissionFactor::get_read_factor(TaskAtr::DUE_DATE)); }
    bool setDueDate(std::optional<std::tm> date) override { return secure_set(&ITaskUse::setDueDate, PermissionFactor::get_update_factor(TaskAtr::DUE_DATE), std::move(date)); }

    std::optional<std::optional<std::shared_ptr<IUserSecureUse>>> getAssignee() const override
    {
        auto assignee = secure_get(&ITaskUse::getAssignee, PermissionFactor::get_read_factor(TaskRel::ASSIGNEE));
        if (!assignee.has_value())
        {
            return std::nullopt;
        }
        if (!assignee.value().has_value() || !assignee.value().value())
        {
            return std::make_optional<std::optional<std::shared_ptr<IUserSecureUse>>>(std::nullopt);
        }
        return std::optional<std::optional<std::shared_ptr<IUserSecureUse>>>(
            std::make_optional<std::shared_ptr<IUserSecureUse>>(
                std::make_shared<SecureUser>(std::move(assignee.value().value()), security_context_)));
    }
    bool setAssignee(std::shared_ptr<User> user) override { return secure_set(&ITaskUse::setAssignee, PermissionFactor::get_update_factor(TaskRel::ASSIGNEE), std::move(user)); }

    std::optional<std::optional<std::shared_ptr<IUserSecureUse>>> getReporter() const override
    {
        auto reporter = secure_get(&ITaskUse::getReporter, PermissionFactor::get_read_factor(TaskRel::REPORTER));
        if (!reporter.has_value())
        {
            return std::nullopt;
        }
        if (!reporter.value().has_value() || !reporter.value().value())
        {
            return std::make_optional<std::optional<std::shared_ptr<IUserSecureUse>>>(std::nullopt);
        }
        return std::optional<std::optional<std::shared_ptr<IUserSecureUse>>>(
            std::make_optional<std::shared_ptr<IUserSecureUse>>(
                std::make_shared<SecureUser>(std::move(reporter.value().value()), security_context_)));
    }
    bool setReporter(std::shared_ptr<User> user) override { return secure_set(&ITaskUse::setReporter, PermissionFactor::get_update_factor(TaskRel::REPORTER), std::move(user)); }

    std::optional<std::vector<std::string>> getLabels() const override { return secure_get(&ITaskUse::getLabels, PermissionFactor::get_read_factor(TaskAtr::LABELS)); }
    bool addLabels(std::vector<std::string> labels) override { return secure_set(&ITaskUse::addLabels, PermissionFactor::get_update_factor(TaskAtr::LABELS), std::move(labels)); }
    bool removeLabels(std::vector<std::string> labels) override { return secure_set(&ITaskUse::removeLabels, PermissionFactor::get_update_factor(TaskAtr::LABELS), std::move(labels)); }

    std::optional<TaskPriority> getPriority() const override { return secure_get(&ITaskUse::getPriority, PermissionFactor::get_read_factor(TaskAtr::PRIORITY)); }
    bool setPriority(TaskPriority priority) override { return secure_set(&ITaskUse::setPriority, PermissionFactor::get_update_factor(TaskAtr::PRIORITY), priority); }

    std::optional<std::string> getType() const override { return secure_get(&ITaskUse::getType, PermissionFactor::get_read_factor(TaskAtr::TYPE)); }
    bool setType(std::string type) override { return secure_set(&ITaskUse::setType, PermissionFactor::get_update_factor(TaskAtr::TYPE), std::move(type)); }
};
