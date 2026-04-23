#include "shared/domain/AutomativeWrapper.hpp"

template <typename UseType, typename Domain, typename Policy>
void AutomativeWrapper<UseType, Domain, Policy>::flush_domain_actions()
{
    if (!domain_actions_.empty())
    {
        AutomationService::instance().execute_automations(
            space_id_,
            DomainAtomAutomation{.entity = automation_entity(), .actions = domain_actions_, .field_full_accessors = full_field_accessors()});
        domain_actions_.clear();
    }
}

template <typename UseType, typename Domain, typename Policy>
template <typename ReturnType, typename... Args>
ReturnType AutomativeWrapper<UseType, Domain, Policy>::perform(ReturnType (UseType::*method)(Args...) const, DomainAction action, Args... args) const
{
    if (before_snapshot_.empty())
        before_snapshot_ = AutomationService::snapshot(*const_cast<AutomativeWrapper *>(this));
    Policy::before(method);
    decltype(auto) result = (this->wrapper_->*method)(std::forward<Args>(args)...);
    Policy::after(method, action);
    log_domain_action(action.event, action.attribute);
    return result;
}

template <typename UseType, typename Domain, typename Policy>
template <typename ReturnType, typename... Args>
ReturnType AutomativeWrapper<UseType, Domain, Policy>::perform(ReturnType (UseType::*method)(Args...), DomainAction action, Args... args)
{
    if (before_snapshot_.empty())
        before_snapshot_ = AutomationService::snapshot(*this);
    Policy::before(method);
    if constexpr (std::is_void_v<ReturnType>)
    {
        (this->wrapper_->*method)(std::forward<Args>(args)...);
        Policy::after(method, action);
        log_domain_action(action.event, action.attribute);
        return;
    }
    else
    {
        decltype(auto) result = (this->wrapper_->*method)(std::forward<Args>(args)...);
        Policy::after(method, action);
        log_domain_action(action.event, action.attribute);
        return result;
    }
}