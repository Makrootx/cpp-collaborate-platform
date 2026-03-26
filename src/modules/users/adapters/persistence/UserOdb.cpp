#include "modules/users/adapters/persistence/UserOdb.hpp"

User UserOdb::to_domain(const UserOdb &odb)
{
    return User::from_persisted(odb.email_, odb.password_, odb.id_);
}

UserOdb UserOdb::to_odb(const User &domain)
{
    return UserOdb(domain.get_email(), domain.get_password(), static_cast<unsigned long>(domain.get_id()));
}