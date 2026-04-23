#pragma once

#include "shared/domain/AutomativeWrapper.hpp"
#include "shared/domain/SecureWrapper.hpp"
#include "modules/boardCategories/domain/BoardCategory.hpp"
#include "modules/tasks/domain/TaskWrappers.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"

class AutomativeBoardCategory : public AutomativeWrapper<IBoardCategoryUse>, public IBoardCategoryUse
{
public:
    using AutomativeWrapper<IBoardCategoryUse>::AutomativeWrapper;
    using BoardCatP = EntitySchemas::EntityAccessor::BoardCategorySchema;
    using BoardCatAtr = BoardCatP::Attributes;
    using BoardCatRel = BoardCatP::Relations;

    std::string automation_entity() const override { return EntitySchemas::EntityAccessor::BOARD_CATEGORIES; }

    std::unordered_map<std::string, FieldAccessor> field_accessors() override
    {
        return {
            {BoardCatAtr::ID, FieldAccessor{.get = [this]()
                                            { return wrapper_->get_id(); },
                                            .set = [this](std::optional<LiteralValue> value)
                                            { wrapper_->set_id(static_cast<long int>(std::get<int>(value.value()))); }}},
            {BoardCatAtr::TITLE, FieldAccessor{.get = [this]()
                                               { return wrapper_->get_title(); },
                                               .set = [this](std::optional<LiteralValue> value)
                                               { wrapper_->set_title(std::get<std::string>(value.value())); }}},
        };
    }

    long int get_id() const override { return perform(&IBoardCategoryUse::get_id, read_trigger(BoardCatAtr::ID)); }
    void set_id(long int id) override { perform(&IBoardCategoryUse::set_id, update_trigger(BoardCatAtr::ID), id); }

    std::string get_title() const override { return perform(&IBoardCategoryUse::get_title, read_trigger(BoardCatAtr::TITLE)); }
    void set_title(std::string v) override { perform(&IBoardCategoryUse::set_title, update_trigger(BoardCatAtr::TITLE), std::move(v)); }

    std::optional<std::vector<std::shared_ptr<ITaskUse>>> get_tasks() const override
    {
        auto tasks = perform(&IBoardCategoryUse::get_tasks, read_trigger(BoardCatRel::TASKS));
        if (!tasks.has_value())
            return std::nullopt;
        std::vector<std::shared_ptr<ITaskUse>> result;
        result.reserve(tasks->size());
        for (const auto &t : *tasks)
            result.push_back(std::make_shared<AutomativeTask>(t, space_id_));
        return result;
    }
    void set_tasks(std::optional<std::vector<Task>> v) override { perform(&IBoardCategoryUse::set_tasks, update_trigger(BoardCatRel::TASKS), std::move(v)); }
};

class SecureBoardCategory : public SecureWrapper<IBoardCategoryUse>, public IBoardCategorySecureUse
{
public:
    using BoardCatP = EntitySchemas::EntityAccessor::BoardCategorySchema;
    using BoardCatAtr = BoardCatP::Attributes;
    using BoardCatRel = BoardCatP::Relations;
    using SecureWrapper<IBoardCategoryUse>::SecureWrapper;

    std::string get_domain_permission_name() const override { return EntitySchemas::EntityAccessor::BOARD_CATEGORIES; }

    std::optional<long int> get_id() const override { return secure_get(&IBoardCategoryUse::get_id, PermissionFactor::get_read_factor(BoardCatAtr::ID)); }
    bool set_id(long int id) override { return secure_set(&IBoardCategoryUse::set_id, PermissionFactor::get_update_factor(BoardCatAtr::ID), id); }

    std::optional<std::string> get_title() const override { return secure_get(&IBoardCategoryUse::get_title, PermissionFactor::get_read_factor(BoardCatAtr::TITLE)); }
    bool set_title(std::string v) override { return secure_set(&IBoardCategoryUse::set_title, PermissionFactor::get_update_factor(BoardCatAtr::TITLE), std::move(v)); }

    std::optional<std::optional<std::vector<std::shared_ptr<ITaskSecureUse>>>> get_tasks() const override
    {
        auto tasks = secure_get(&IBoardCategoryUse::get_tasks, PermissionFactor::get_read_factor(BoardCatRel::TASKS));
        if (!tasks.has_value())
            return std::nullopt;
        auto &inner = *tasks;
        if (!inner.has_value())
            return std::make_optional<std::optional<std::vector<std::shared_ptr<ITaskSecureUse>>>>(std::nullopt);
        std::vector<std::shared_ptr<ITaskSecureUse>> result;
        result.reserve(inner->size());
        for (const auto &t : *inner)
            result.push_back(std::make_shared<SecureTask>(t, security_context_));
        return std::optional<std::optional<std::vector<std::shared_ptr<ITaskSecureUse>>>>(std::move(result));
    }
    bool set_tasks(std::optional<std::vector<Task>> v) override { return secure_set(&IBoardCategoryUse::set_tasks, PermissionFactor::get_update_factor(BoardCatRel::TASKS), std::move(v)); }
};
