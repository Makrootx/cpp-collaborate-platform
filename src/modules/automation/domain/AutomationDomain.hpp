#pragma once

#include "shared/domain/BaseDomain.hpp"
#include "shared/adapters/automation_core/AutomationStructs.hpp"

class AutomationDomain : public BaseDomain
{
protected:
    AutomationConfig config;

public:
    AutomationDomain() = default;

    explicit AutomationDomain(AutomationConfig config, unsigned long id = 0)
        : config(std::move(config)), BaseDomain(id) {}

    AutomationDomain(const AutomationDomain &) = default;
    AutomationDomain(AutomationDomain &&) noexcept = default;
    AutomationDomain &operator=(AutomationDomain &&) noexcept = default;
    AutomationDomain &operator=(const AutomationDomain &) = default;
    ~AutomationDomain() = default;

    const AutomationConfig &get_config() const { return config; };
    void set_config(AutomationConfig config) { this->config = std::move(config); }
};