#pragma once

#include "modules/automation/domain/AutomationDomain.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/odb/BaseOdb.hpp"

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>

#include <memory>
#include <spdlog/spdlog.h>

#pragma db object table("automations") pointer(std::shared_ptr)
class AutomationDomainOdb : public BaseOdb, public IOdbMappable<AutomationDomain, AutomationDomainOdb>, public IOdbMappableQuery<AutomationDomain, AutomationDomainOdb>
{
public:
    AutomationDomainOdb() = default;

    explicit AutomationDomainOdb(AutomationConfig config, unsigned long id = 0)
        : config_(std::move(config.to_json().dump())), BaseOdb(id) {}

    AutomationDomainOdb(const AutomationDomainOdb &) = default;
    AutomationDomainOdb(AutomationDomainOdb &&) noexcept = default;
    AutomationDomainOdb &operator=(AutomationDomainOdb &&) noexcept = default;
    AutomationDomainOdb &operator=(const AutomationDomainOdb &) = default;
    ~AutomationDomainOdb() = default;

    const AutomationConfig config_value() const
    {
        auto json = crow::json::load(config_);
        if (!json)
        {
            spdlog::error("Error parsing JSON in config_ field of AutomationDomainOdb with id {}", id_value());
            return AutomationConfig{};
        }

        spdlog::debug("Raw config JSON for id {}: {}", id_value(), config_);

        try
        {
            // Get validation errors to understand why from_json might fail
            auto [valid, errors] = AutomationConfig::validate(json);
            if (!valid)
            {
                spdlog::error("AutomationConfig JSON validation failed for id {}", id_value());
                for (const auto &err : errors)
                {
                    spdlog::error("  - Validation error: {}", err);
                }
                return AutomationConfig{};
            }

            auto config_opt = AutomationConfig::from_json(json);
            if (!config_opt.has_value())
            {
                spdlog::error("Error parsing JSON in config_ field of AutomationDomainOdb with id {}: from_json returned empty optional after validation passed", id_value());
                return AutomationConfig{};
            }
            return config_opt.value();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Error parsing JSON in config_ field of AutomationDomainOdb with id {}: Exception during deserialization: {}", id_value(), e.what());
            return AutomationConfig{};
        }
    }
    static AutomationDomain to_domain(const AutomationDomainOdb &odb);
    static AutomationDomainOdb to_odb(const AutomationDomain &domain);

private:
    friend class odb::access;

#pragma db type("TEXT")
    std::string config_;
};
