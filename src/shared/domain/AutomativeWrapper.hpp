#pragma once

#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "shared/domain/BaseWrapper.hpp"
#include "shared/adapters/automation_core/AutomationRuntimeStructs.hpp"
#include "shared/adapters/automation_core/AutomationService.hpp"

struct DefaultPolicy
{
    template <typename Fn>
    static void before(Fn)
    {
    }

    template <typename Fn>
    static void after(Fn, DomainAction)
    {
    }
};

template <typename UseType, typename Domain = domain_t<UseType>, typename Policy = DefaultPolicy>
class AutomativeWrapper : public BaseWrapper<UseType, Domain>, public IAutomatable
{
    using Base = BaseWrapper<UseType, Domain>;

public:
    explicit AutomativeWrapper(UseType &w, long int space_id)
        : Base(w), space_id_(space_id) {}
    explicit AutomativeWrapper(std::shared_ptr<UseType> w, long int space_id, GroupPermission = {})
        : Base(std::move(w)), space_id_(space_id) {}

    template <typename Derived>
        requires std::derived_from<Derived, UseType> && (!std::is_abstract_v<Derived>)
    explicit AutomativeWrapper(Derived &&w, long int space_id)
        : Base(std::forward<Derived>(w)), space_id_(space_id)
    {
    }

    void flush_domain_actions();

protected:
    long int space_id_;
    mutable std::vector<DomainAction> domain_actions_ = {};
    mutable std::unordered_map<std::string, std::optional<LiteralValue>> before_snapshot_;

    std::unordered_map<std::string, std::optional<LiteralValue>> before_domain() const override
    {
        return before_snapshot_;
    }

    DomainAction read_trigger(std::string attribute) const
    {
        return DomainAction{.event = AutomationEvent::READ, .attribute = std::move(attribute)};
    }

    DomainAction update_trigger(std::string attribute) const
    {
        return DomainAction{.event = AutomationEvent::UPDATE, .attribute = std::move(attribute)};
    }

    void log_domain_action(AutomationEvent event, std::string attribute) const
    {
        domain_actions_.push_back({.event = event, .attribute = std::move(attribute)});
    }

    template <typename ReturnType, typename... Args>
    ReturnType perform(ReturnType (UseType::*method)(Args...) const, DomainAction action, Args... args) const;

    template <typename ReturnType, typename... Args>
    ReturnType perform(ReturnType (UseType::*method)(Args...), DomainAction action, Args... args);
};

#include "AutomativeWrapper.tpp"