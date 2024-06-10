#ifndef EARL_H
#define EARL_H

#include <string>
#include <vector>
#include <any>
#include <variant>

namespace EARL {
    namespace Type {

        struct T;

        using Int = int;
        using Str = std::string;
        using Void = std::nullptr_t;
        using List = std::vector<T>;
        using Value = std::variant<Int, Str, Void, List, T*>;

        struct T {
            T();
            ~T() = default;
            T(Int value);
            T(Str value);
            T(Void value);
            T(List value);
            T(T *value);

            Value get() const;

        private:
            Value m_value;
        };
    };

    namespace Variable {
    };

    namespace Function {
    };
};

#endif // EARL_H
