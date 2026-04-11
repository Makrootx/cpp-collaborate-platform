#include <memory>
#include <concepts>
#include <optional>
#include <iostream>

template <typename T>
class Wrappable
{
public:
    virtual T to_domain() = 0;
    virtual ~Wrappable() = default;
};

template <typename UseType, typename Domain>
concept WrappableConcept = requires(UseType useType) {
    { useType.to_domain() } -> std::convertible_to<Domain>;
};

template <typename UseType, typename Domain>
    requires WrappableConcept<UseType, Domain>
class Wrapper : public virtual Wrappable<Domain>
{
protected:
    std::unique_ptr<UseType> owned_wrapper_;
    UseType *wrapper_;

public:
    explicit Wrapper(UseType &wrapper) : wrapper_(&wrapper) {}

    template <typename Derived>
        requires std::derived_from<Derived, UseType>
    explicit Wrapper(Derived &&wrapper)
        : owned_wrapper_(std::make_unique<Derived>(std::move(wrapper))),
          wrapper_(owned_wrapper_.get())
    {
    }

    Domain to_domain() override
    {
        return wrapper_->to_domain();
    }
};

class User;

class UserUse : public virtual Wrappable<User>
{
public:
    virtual int get_id() = 0;
    virtual void set_id(int id) = 0;
    virtual ~UserUse() = default;
};

class UserSecureUse : public virtual Wrappable<User>
{
public:
    virtual std::optional<int> get_id() = 0;
    virtual bool set_id(int id) = 0;
    virtual ~UserSecureUse() = default;
};

class User : public UserUse
{
protected:
    int id_;

public:
    User(int id) : id_(std::move(id)) {};

    int get_id() override { return id_; }
    void set_id(int id) override { id_ = std::move(id); }
    User to_domain() override { return *this; }
};

class AutomativeUser : public Wrapper<UserUse, User>, public UserUse
{
public:
    explicit AutomativeUser(UserUse &wrapper) : Wrapper(wrapper) {};

    int get_id() override
    {
        std::cout << "Automative use " << wrapper_->get_id() << std::endl;
        return wrapper_->get_id();
    }
    void set_id(int id) override
    {
        std::cout << "Automative use " << id << std::endl;
        wrapper_->set_id(id);
    };
};

class DefaultSecureUser : public Wrapper<UserUse, User>, public UserSecureUse
{
public:
    explicit DefaultSecureUser(UserUse &wrapper) : Wrapper(wrapper) {};

    std::optional<int> get_id() override { return wrapper_->get_id(); }
    bool set_id(int id) override
    {
        wrapper_->set_id(id);
        return true;
    };
};

class SecureUser : public Wrapper<UserSecureUse, User>, public UserSecureUse
{
public:
    explicit SecureUser(UserSecureUse &wrapper) : Wrapper(wrapper) {};
    explicit SecureUser(UserUse &wrapper) : Wrapper(DefaultSecureUser(wrapper)) {};

    std::optional<int> get_id() override
    {
        const auto id = wrapper_->get_id();
        std::cout << "Secure use " << id.value_or(-1) << std::endl;
        return id;
    }
    bool set_id(int id) override
    {
        std::cout << "Secure use " << id << std::endl;
        return wrapper_->set_id(id);
    };
};

int main()
{
    std::cout << "Hello, World!" << std::endl;
    User u(1);
    AutomativeUser a(u);
    SecureUser s(a);

    s.get_id();
    s.set_id(2);
    std::cout << "User id: " << s.to_domain().get_id() << std::endl;
    return 0;
}
