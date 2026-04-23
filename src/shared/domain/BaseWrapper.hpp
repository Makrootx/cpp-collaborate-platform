#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

/// @brief Alias that resolves the domain type returned by T::to_domain().
template <typename T>
using domain_t = decltype(std::declval<T>().to_domain());

/// @brief Interface for types that can materialize themselves as their associated domain object.
template <typename T>
class Wrappable
{
public:
    virtual T to_domain() = 0;
    virtual ~Wrappable() = default;
};

/// @brief Satisfied when UseType exposes a to_domain() member convertible to Domain.
template <typename UseType, typename Domain>
concept WrappableConcept = requires(UseType u) {
    { u.to_domain() } -> std::convertible_to<Domain>;
};

/// @brief Ownership-agnostic wrapper base that holds a UseType by reference, shared_ptr, or unique_ptr and forwards method calls.
template <typename UseType, typename Domain = domain_t<UseType>>
    requires WrappableConcept<UseType, Domain>
class BaseWrapper : public virtual Wrappable<Domain>
{
protected:
    std::shared_ptr<UseType> shared_wrapper_;
    std::unique_ptr<UseType> owned_wrapper_;
    UseType *wrapper_;

public:
    explicit BaseWrapper(UseType &w) : wrapper_(&w) {}
    explicit BaseWrapper(const UseType &w) : wrapper_(const_cast<UseType *>(&w)) {}

    explicit BaseWrapper(std::shared_ptr<UseType> w)
        : shared_wrapper_(std::move(w)), wrapper_(shared_wrapper_.get()) {}

    template <typename Derived>
        requires std::derived_from<Derived, UseType> && (!std::is_abstract_v<Derived>)
    explicit BaseWrapper(Derived &&wrapper)
        : owned_wrapper_(std::make_unique<Derived>(std::move(wrapper))),
          wrapper_(owned_wrapper_.get())
    {
    }

    Domain to_domain() override { return wrapper_->to_domain(); }

    /// @brief Invokes a non-const member function pointer on the wrapped object.
    template <typename Ret, typename... Args>
    Ret perform(Ret (UseType::*fn)(Args...), Args &&...args)
    {
        return (wrapper_->*fn)(std::forward<Args>(args)...);
    }

    /// @brief Invokes a const member function pointer on the wrapped object.
    template <typename Ret, typename... Args>
    Ret perform(Ret (UseType::*fn)(Args...) const, Args &&...args) const
    {
        return (wrapper_->*fn)(std::forward<Args>(args)...);
    }
};