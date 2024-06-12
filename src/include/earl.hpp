#ifndef EARL_H
#define EARL_H

#include <unordered_map>
#include <string>
#include <vector>
#include <any>
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
            virtual Type type(void) const = 0;
        };

        struct Int : public Obj {
            Int(int value);
            int value(void);
            Type type(void) const override;

        private:
            int m_value;
        };

        struct Str : public Obj {
            Str(std::string value);
            std::string &value(void);
            Type type(void) const override;

        private:
            std::string m_value;
        };

        struct Void : public Obj {
            Void(void *value = nullptr);
            void *value(void);
            Type type(void) const override;

        private:
            void *m_value;
        };

        struct List : public Obj {
            List(std::vector<Obj> value);
            std::vector<Obj> &value(void);
            Type type(void) const override;

        private:
            std::vector<Obj> m_value;
        };
    };

    namespace variable {
        struct Obj;
    };

    namespace function {
        struct Obj;
    };

    namespace evalres {
        struct Obj;
    };
};

#endif // EARL_H
