#ifndef EARL_H
#define EARL_H

#include <unordered_set>
#include <string>
#include <vector>
#include <any>
#include <variant>

#include "scope.hpp"
#include "ast.hpp"
#include "token.hpp"

namespace earl {
    namespace primitive {
        enum class Type {
            Int = 0,
            Str,
            Void,
            List,
        };
    };

    namespace runtime {
        namespace value {
            struct Obj;

            using Int = int;
            using Str = std::string;
            using Unit = std::nullptr_t;
            using List = std::vector<Obj>;
            using RuntimeValue = std::variant<Int, Str, Unit, List>;

            struct Obj {
                Obj() = default;
                ~Obj() = default;
                Obj(Int val);
                Obj(Str val);
                Obj(Unit val);
                Obj(List val);

                RuntimeValue get() const;
                primitive::Type type(void) const;

            private:
                RuntimeValue m_value;
                primitive::Type m_type;
            };
        };

        namespace variable {
            struct Obj {
                Obj(Token *id, value::Obj value);
                ~Obj() = default;

                primitive::Type type(void) const;
                const std::string &id(void) const;

            private:
                Token *m_id;
                value::Obj m_value;
            };
        };

        namespace function {
            struct Obj {
                Obj(Token *id, Token *rettype, std::vector<variable::Obj *> args, StmtBlock *block, uint32_t attrs);
                ~Obj() = default;

                void push_scope(void);
                void pop_scope(void);
                void new_scope_context(void);
                void drop_scope_context(void);

                variable::Obj &get_local(const std::string &id);
                bool           has_local(const std::string &id);
                void           add_local(variable::Obj *var);
                void           remove_local(const std::string &id);
                void           remove_local(const variable::Obj *var);

                size_t context_size(void) const;
                bool is_world(void) const;

                const std::string &id(void) const;

            private:
                Token *m_id;
                Token *m_rettype;
                std::vector<variable::Obj *> m_args;
                StmtBlock *m_block;
                uint32_t m_attrs;
                std::vector<Scope<std::string, variable::Obj *>> m_local;
            };
        };

        namespace evalres {
            struct Obj {
                Obj(value::Obj value);
                ~Obj() = default;

                value::RuntimeValue value(void);
                primitive::Type type(void);

            private:
                value::Obj m_value;
            };
        };
    };
};

#endif // EARL_H
