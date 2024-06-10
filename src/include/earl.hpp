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

namespace EARL {
    namespace Primitive {
        enum class Type {
            Int = 0,
            Str,
            Void,
            List,
        };
    };

    namespace Runtime {
        namespace Value {
            struct Obj;

            using Int = int;
            using Str = std::string;
            using Void = std::nullptr_t;
            using List = std::vector<Obj>;
            using RuntimeValue = std::variant<Int, Str, Void, List>;

            struct Obj {
                Obj() = default;
                ~Obj() = default;
                Obj(Int val);
                Obj(Str val);
                Obj(Void val);
                Obj(List val);

                RuntimeValue get() const;

            private:
                RuntimeValue m_value;
                Primitive::Type m_type;
            };
        };

        namespace Variable {
            struct Obj {
                Token *m_id;
                Value::Obj m_value;

                Obj(Token *id, Value::Obj value);
                ~Obj() = default;
            };
        };

        namespace Function {
            struct Obj {
                Token *m_id;
                Token *m_rettype;
                std::vector<Variable::Obj *> m_args;
                StmtBlock *m_block;
                uint32_t m_attrs;
                std::vector<Scope<std::string, Variable::Obj *>> m_local;

                Obj(Token *id, Token *rettype, std::vector<Variable::Obj *> args, StmtBlock *block, uint32_t attrs);
                ~Obj() = default;

                void push_scope(void);
                void pop_scope(void);
                void new_scope_context(void);
                void drop_scope_context(void);

                bool has_local(const std::string &id) const;
                Variable::Obj *get_local(const std::string &id) const;
                void add_local(Variable::Obj *var);
                size_t context_size(void) const;
                void remove_local(const std::string &id);
                void remove_local(const Variable::Obj &var);

                bool is_world(void);
            };
        };
    };
};

#endif // EARL_H
