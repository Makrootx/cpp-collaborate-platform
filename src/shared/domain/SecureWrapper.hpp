#pragma once

#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>

#include "shared/adapters/permission_core/PermissionUtils.hpp"
#include "shared/domain/BaseWrapper.hpp"

struct SecurePolicy
{
    template <typename Fn>
    static bool before(Fn, PermissionFactor perm_factor, GroupPermission context, std::string entity);

    template <typename Fn>
    static void after(Fn);
};

template <typename UseType, typename Domain = domain_t<UseType>, typename Policy = SecurePolicy>
class SecureWrapper : public BaseWrapper<UseType, Domain>
{
    using Base = BaseWrapper<UseType, Domain>;

protected:
    GroupPermission security_context_;

    virtual std::string get_domain_permission_name() const = 0;

public:
    explicit SecureWrapper(UseType &w, GroupPermission context = {}) : Base(w), security_context_(std::move(context)) {}
    explicit SecureWrapper(std::shared_ptr<UseType> w, GroupPermission context = {}) : Base(std::move(w)), security_context_(std::move(context)) {}

    template <typename Derived>
        requires std::derived_from<Derived, UseType> && (!std::is_abstract_v<Derived>)
    explicit SecureWrapper(Derived &&w, GroupPermission context = {})
        : Base(std::forward<Derived>(w)), security_context_(std::move(context))
    {
    }

protected:
    template <typename ReturnType, typename... Args>
    std::optional<std::remove_cvref_t<ReturnType>> secure_get(ReturnType (UseType::*method)(Args...) const, PermissionFactor perm_factor, Args... args) const;

    template <typename ReturnType, typename... Args>
    bool secure_set(ReturnType (UseType::*method)(Args...), PermissionFactor perm_factor, Args... args);
};

#include "SecureWrapper.tpp"