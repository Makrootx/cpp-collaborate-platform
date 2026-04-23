#pragma once

#include <array>
#include <string_view>
#include <unordered_map>

namespace EntitySchemas
{
    template <typename T>
    concept HasEntitySchema = requires {
        T::ENTITY_ACCESSOR;
        T::Attributes::ALL;
        T::Relations::ALL;
    };

    /// @brief Type-erased runtime representation of an entity schema for attribute/relation lookups.
    struct RuntimeSchema
    {
        const char *name;
        bool (*has_attr)(std::string_view);
        bool (*has_rel)(std::string_view);
    };

    /// @brief Builds a RuntimeSchema from a compile-time schema type by erasing attribute/relation lookup into function pointers.
    template <typename Schema>
        requires HasEntitySchema<Schema>
    inline RuntimeSchema make_runtime_schema()
    {
        return {
            Schema::ENTITY_ACCESSOR,
            [](std::string_view attr)
            {
                for (const char *a : Schema::Attributes::ALL)
                {
                    if (a == attr)
                        return true;
                }
                return false;
            },
            [](std::string_view rel)
            {
                for (const char *r : Schema::Relations::ALL)
                {
                    if (r == rel)
                        return true;
                }
                return false;
            }};
    }

    /// @brief Central registry of entity accessor keys and their per-entity compile-time attribute/relation schemas.
    struct EntityAccessor
    {
        static constexpr const char *TASKS = "tasks";
        static constexpr const char *USERS = "users";
        static constexpr const char *BOARDS = "boards";
        static constexpr const char *BOARD_CATEGORIES = "board_categories";

        static constexpr const char *ALL_ENTITIES[] = {TASKS, USERS, BOARDS, BOARD_CATEGORIES};

        struct TaskSchema
        {
            static constexpr const char *ENTITY_ACCESSOR = TASKS;
            struct Attributes
            {
                static constexpr const char *ID = "id";
                static constexpr const char *TITLE = "title";
                static constexpr const char *DESCRIPTION = "description";
                static constexpr const char *DUE_DATE = "due_date";
                static constexpr const char *CREATE_DATE = "create_date";
                static constexpr const char *UPDATE_DATE = "update_date";
                static constexpr const char *LABELS = "labels";
                static constexpr const char *PRIORITY = "priority";
                static constexpr const char *TYPE = "type";

                static constexpr const char *ALL[] = {
                    ID,
                    TITLE,
                    DESCRIPTION,
                    DUE_DATE,
                    CREATE_DATE,
                    UPDATE_DATE,
                    LABELS,
                    PRIORITY,
                    TYPE,
                };
            };

            struct Relations
            {
                static constexpr const char *ASSIGNEE = "assignee";
                static constexpr const char *REPORTER = "reporter";
                static constexpr const char *CONNECTED_CATEGORY = "connected_category";

                static constexpr const char *ALL[] = {
                    ASSIGNEE,
                    REPORTER,
                    CONNECTED_CATEGORY,
                };
            };
        };

        struct UserSchema
        {
            static constexpr const char *ENTITY_ACCESSOR = USERS;
            struct Attributes
            {
                static constexpr const char *EMAIL = "email";
                static constexpr const char *ID = "id";

                static constexpr const char *ALL[] = {
                    EMAIL,
                    ID,
                };
            };

            struct Relations
            {
                static constexpr std::array<const char *, 0> ALL{};
            };
        };

        struct BoardSchema
        {
            static constexpr const char *ENTITY_ACCESSOR = BOARDS;
            struct Attributes
            {
                static constexpr const char *ID = "id";
                static constexpr const char *TITLE = "title";
                static constexpr const char *DESCRIPTION = "description";

                static constexpr const char *ALL[] = {
                    ID,
                    TITLE,
                    DESCRIPTION,
                };
            };

            struct Relations
            {
                static constexpr const char *CATEGORIES = "categories";

                static constexpr const char *ALL[] = {
                    CATEGORIES,
                };
            };
        };

        struct BoardCategorySchema
        {
            static constexpr const char *ENTITY_ACCESSOR = BOARD_CATEGORIES;
            struct Attributes
            {
                static constexpr const char *ID = "id";
                static constexpr const char *TITLE = "title";

                static constexpr const char *ALL[] = {
                    ID,
                    TITLE,
                };
            };

            struct Relations
            {
                static constexpr const char *TASKS = "tasks";
                static constexpr const char *CONNECTED_BOARD = "connected_board";

                static constexpr const char *ALL[] = {
                    TASKS,
                    CONNECTED_BOARD,
                };
            };
        };

        /// @brief Returns the lazily-initialized map from entity accessor key to its RuntimeSchema.
        static const std::unordered_map<std::string_view, RuntimeSchema> &get_schema_map()
        {
            static const std::unordered_map<std::string_view, RuntimeSchema> SCHEMA_MAP = {
                {TASKS, make_runtime_schema<TaskSchema>()},
                {USERS, make_runtime_schema<UserSchema>()},
                {BOARDS, make_runtime_schema<BoardSchema>()},
                {BOARD_CATEGORIES, make_runtime_schema<BoardCategorySchema>()}};
            return SCHEMA_MAP;
        }
    };
}
