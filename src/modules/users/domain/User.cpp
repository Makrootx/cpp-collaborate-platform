#include "modules/users/domain/User.hpp"
#include "shared/adapters/PasswordHasher.hpp"

std::string User::hash_password(const std::string &password)
{
    return PasswordHasher::hash(password);
};

bool User::verify_password(const std::string &password) const
{
    return PasswordHasher::verify(password, this->password);
}
