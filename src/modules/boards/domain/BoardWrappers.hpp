#pragma once

#include "shared/domain/AutomativeWrapper.hpp"
#include "shared/domain/SecureWrapper.hpp"
#include "modules/boards/domain/Board.hpp"
#include "modules/boardCategories/domain/BoardCategoryWrappers.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"

class AutomativeBoard : public AutomativeWrapper<IBoardUse>, public IBoardUse
{
public:
    using AutomativeWrapper<IBoardUse>::AutomativeWrapper;
    using BoardP = EntitySchemas::EntityAccessor::BoardSchema;
    using BoardAtr = BoardP::Attributes;
    using BoardRel = BoardP::Relations;

    std::string automation_entity() const override { return EntitySchemas::EntityAccessor::BOARDS; }

    std::unordered_map<std::string, FieldAccessor> field_accessors() override
    {
        return {
            {BoardAtr::ID, FieldAccessor{.get = [this]()
                                         { return wrapper_->get_id(); },
                                         .set = [this](std::optional<LiteralValue> value)
                                         { wrapper_->set_id(static_cast<long int>(std::get<int>(value.value()))); }}},
            {BoardAtr::TITLE, FieldAccessor{.get = [this]()
                                            { return wrapper_->get_title(); },
                                            .set = [this](std::optional<LiteralValue> value)
                                            { wrapper_->set_title(std::get<std::string>(value.value())); }}},
            {BoardAtr::DESCRIPTION, FieldAccessor{.get = [this]()
                                                  { return wrapper_->get_description(); },
                                                  .set = [this](std::optional<LiteralValue> value)
                                                  { wrapper_->set_description(std::get<std::string>(value.value())); }}},
        };
    }

    long int get_id() const override { return perform(&IBoardUse::get_id, read_trigger(BoardAtr::ID)); }
    void set_id(long int id) override { perform(&IBoardUse::set_id, update_trigger(BoardAtr::ID), id); }

    std::string get_title() const override { return perform(&IBoardUse::get_title, read_trigger(BoardAtr::TITLE)); }
    void set_title(std::string v) override { perform(&IBoardUse::set_title, update_trigger(BoardAtr::TITLE), std::move(v)); }

    std::string get_description() const override { return perform(&IBoardUse::get_description, read_trigger(BoardAtr::DESCRIPTION)); }
    void set_description(std::string v) override { perform(&IBoardUse::set_description, update_trigger(BoardAtr::DESCRIPTION), std::move(v)); }

    std::optional<std::vector<std::shared_ptr<IBoardCategoryUse>>> get_categories() const override
    {
        auto cats = perform(&IBoardUse::get_categories, read_trigger(BoardRel::CATEGORIES));
        if (!cats.has_value())
            return std::nullopt;
        std::vector<std::shared_ptr<IBoardCategoryUse>> result;
        result.reserve(cats->size());
        for (const auto &cat : *cats)
            result.push_back(std::make_shared<AutomativeBoardCategory>(cat, space_id_));
        return result;
    }
    void set_categories(std::optional<std::vector<std::shared_ptr<BoardCategory>>> v) override { perform(&IBoardUse::set_categories, update_trigger(BoardRel::CATEGORIES), std::move(v)); }
};

class SecureBoard : public SecureWrapper<IBoardUse>, public IBoardSecureUse
{
public:
    using BoardP = EntitySchemas::EntityAccessor::BoardSchema;
    using BoardAtr = BoardP::Attributes;
    using BoardRel = BoardP::Relations;
    using SecureWrapper<IBoardUse>::SecureWrapper;

    std::string get_domain_permission_name() const override { return EntitySchemas::EntityAccessor::BOARDS; }

    std::optional<long int> get_id() const override { return secure_get(&IBoardUse::get_id, PermissionFactor::get_read_factor(BoardAtr::ID)); }
    bool set_id(long int id) override { return secure_set(&IBoardUse::set_id, PermissionFactor::get_update_factor(BoardAtr::ID), id); }

    std::optional<std::string> get_title() const override { return secure_get(&IBoardUse::get_title, PermissionFactor::get_read_factor(BoardAtr::TITLE)); }
    bool set_title(std::string v) override { return secure_set(&IBoardUse::set_title, PermissionFactor::get_update_factor(BoardAtr::TITLE), std::move(v)); }

    std::optional<std::string> get_description() const override { return secure_get(&IBoardUse::get_description, PermissionFactor::get_read_factor(BoardAtr::DESCRIPTION)); }
    bool set_description(std::string v) override { return secure_set(&IBoardUse::set_description, PermissionFactor::get_update_factor(BoardAtr::DESCRIPTION), std::move(v)); }

    std::optional<std::optional<std::vector<std::shared_ptr<IBoardCategorySecureUse>>>> get_categories() const override
    {
        auto cats = secure_get(&IBoardUse::get_categories, PermissionFactor::get_read_factor(BoardRel::CATEGORIES));
        if (!cats.has_value())
            return std::nullopt;
        auto &inner = *cats;
        if (!inner.has_value())
            return std::make_optional<std::optional<std::vector<std::shared_ptr<IBoardCategorySecureUse>>>>(std::nullopt);
        std::vector<std::shared_ptr<IBoardCategorySecureUse>> result;
        result.reserve(inner->size());
        for (const auto &cat : *inner)
            result.push_back(std::make_shared<SecureBoardCategory>(cat, security_context_));
        return std::optional<std::optional<std::vector<std::shared_ptr<IBoardCategorySecureUse>>>>(std::move(result));
    }
    bool set_categories(std::optional<std::vector<std::shared_ptr<BoardCategory>>> v) override { return secure_set(&IBoardUse::set_categories, PermissionFactor::get_update_factor(BoardRel::CATEGORIES), std::move(v)); }
};
