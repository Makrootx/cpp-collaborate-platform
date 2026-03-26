#include "shared/adapters/PasswordHasher.hpp"
#include <argon2.h>
#include <array>
#include <stdexcept>
#include <openssl/rand.h>

#include <spdlog/spdlog.h>

std::string PasswordHasher::hash(const std::string &password)
{
    constexpr uint32_t t_cost = 3;
    constexpr uint32_t m_cost = 1 << 16;
    constexpr uint32_t parallelism = 1;
    constexpr size_t salt_len = 16;
    constexpr size_t hash_len = 32;
    constexpr size_t encoded_len = 128;

    std::array<unsigned char, salt_len> salt{};
    if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1)
        throw std::runtime_error("Failed to generate salt");

    std::array<char, encoded_len> encoded{};
    int rc = argon2id_hash_encoded(
        t_cost, m_cost, parallelism,
        password.data(), password.size(),
        salt.data(), salt.size(),
        hash_len, encoded.data(), encoded.size());

    if (rc != ARGON2_OK)
        throw std::runtime_error("Argon2 hashing failed: " + std::string(argon2_error_message(rc)));

    return std::string(encoded.data());
}

bool PasswordHasher::verify(const std::string &password, const std::string &hash)
{
    const bool ok = argon2id_verify(hash.c_str(), password.data(), password.size()) == ARGON2_OK;
    spdlog::debug("Password verification result: {}", ok ? "success" : "failure");
    return ok;
}