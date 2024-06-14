/** @file */

#ifndef EARL_H
#define EARL_H

#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include <variant>

#include "scope.hpp"
#include "ast.hpp"
#include "token.hpp"

namespace earl {
    namespace value {
        enum class Type {
            Int = 0,
            Str,
            Void,
            List,
        };

        struct Obj {
            ~Obj() = default;
            int x;

            virtual Type type(void) const = 0;
        };

        struct Int : public Obj {
            Int(std::optional<int> value = {});
            void fill(int value);
            int value(void);
            Type type(void) const override;

        private:
            std::optional<int> m_value;
        };

        struct Str : public Obj {
            Str(std::optional<std::string> value = {});
            void fill(std::string value);
            std::string &value(void);
            Type type(void) const override;

        private:
            std::optional<std::string> m_value;
        };

        struct Void : public Obj {
            Void(std::optional<void *> value = {});
            void *value(void);
            Type type(void) const override;

        private:
            std::optional<void *> m_value;
        };

        struct List : public Obj {
            List(std::optional<std::vector<Obj>> value = {});
            void fill(std::vector<Obj> value);
            std::vector<Obj> &value(void);
            Type type(void) const override;

        private:
            std::optional<std::vector<Obj>> m_value;
        };

        Obj *of_str(const std::string &s);
        bool type_is_compatable(Obj *const obj1, Obj *const obj2);
    };

    namespace variable {
        struct Obj {
            Obj(Token *id, std::unique_ptr<value::Obj> value);
            ~Obj() = default;

            const std::string &id(void) const;
            bool is_global(void) const;
            value::Obj *value(void) const;
            value::Type type(void) const;

        private:
            Token *m_id;
            std::unique_ptr<value::Obj> m_value;
        };
    };

    namespace function {
        struct Obj {
            Obj(StmtDef *stmtdef);
            ~Obj() = default;

            std::string &id(void) const;

            void load_parameters(std::vector<value::Obj *>);

            void push_scope(void);
            void pop_scope(void);
            void new_scope_context(void);
            void drop_scope_context(void);

            bool has_local(const std::string &id);
            void add_local(variable::Obj *var);
            void remove_local(const std::string &id);
            variable::Obj &get_local(const std::string &id);

            size_t context_size(void);
            bool is_world(void) const;

        private:
            StmtDef *m_stmtdef;
            Scope<std::string, variable::Obj *> m_local;
        };
    };
};

#endif // EARL_H
