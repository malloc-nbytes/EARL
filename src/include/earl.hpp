/** @file */

// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef EARL_H
#define EARL_H

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>

#include "shared-scope.hpp"
#include "ast.hpp"
#include "token.hpp"

#define ASSERT_BINOP_COMPAT(obj0, obj1, op)                             \
    do {                                                                \
        if (!type_is_compatable(obj0, obj1)) {                          \
            Err::err_wtok(op);                                          \
            ERR_WARGS(Err::Type::Fatal,                                 \
                      "value of type `%s` is not compatible with value of type `%s`", \
                      earl::value::type_to_str(obj0->type()).c_str(), earl::value::type_to_str(obj1->type()).c_str()); \
        }                                                                       \
    } while (0)

#define ASSERT_MUTATE_COMPAT(obj0, obj1)                                \
    do {                                                                \
        if (!type_is_compatable(obj0, obj1)) {                          \
            ERR_WARGS(Err::Type::Fatal,                                 \
                      "value of type `%s` is not compatible with value of type `%s`", \
                      earl::value::type_to_str(obj0->type()).c_str(), earl::value::type_to_str(obj1->type()).c_str()); \
        }                                                                       \
    } while (0)

struct Ctx;
struct FunctionCtx;

namespace earl {
    namespace variable {struct Obj;}
    namespace function {struct Obj;}
}

/**
 * All values associated with EARL during runtime.
 */
namespace earl {
    /**
     * The values that are returned from each
     * evaluation during runtime. They represent
     * the underlying data of EARL.
     */
    namespace value {
        /// @brief The intrinsic types of EARL
        enum class Type {
            /** EARL 32bit integer type */
            Int,
            /** EARL floating integer (double) type */
            Float,
            /** EARL boolean type */
            Bool,
            /** EARL string type */
            Str,
            /** EARL char type */
            Char,
            /** EARL UNIT type (used when eval's don't return anything) */
            Void,
            /** EARL list type (holds any value including a mix of datatypes) */
            List,
            /** EARL module type. Used for member access of a module. */
            Module,
            /** EARL file type */
            File,
            /** EARL option type */
            Option,
            /** EARL This type for getting class members */
            This,
            /** EARL closure type */
            Closure,
            /** EARL OS type */
            OS,
            /** EARL break type */
            Break,
            /** EARL class type */
            Class,
            /** EARL enum type */
            Enum,
        };

        /// @brief The base abstract class that all
        /// EARL values inherit from
        struct Obj {
            virtual ~Obj() {}

            /// @brief Get the type of the value
            virtual Type type(void) const = 0;

            /// @brief Perform a binary operation on THIS
            /// value with another value
            /// @param op The binary operator
            /// @param other The object to perform the binop with
            virtual std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) = 0;

            /// @brief Get the evaluation when put into
            /// a conditional to evaluate to either true or false
            virtual bool boolean(void) = 0;

            /// @brief Modify the underlying data of THIS value
            /// with the underlying value of another object
            /// @param other The value to mutate THIS instance with
            virtual void mutate(const std::shared_ptr<Obj> &other) = 0;

            /// @brief Copy THIS instance
            virtual std::shared_ptr<Obj> copy(void) = 0;

            /// @brief Check the equality of two objects, not just by their values.
            /// @param other The object to compare
            /// @return true on equal, false otherwise
            virtual bool eq(std::shared_ptr<Obj> &other) = 0;

            /// @brief Convert the value of an object to a cxx std::string
            /// @return The stringified version of the value
            virtual std::string to_cxxstring(void) = 0;

            virtual void spec_mutate(Token *op, const std::shared_ptr<Obj> &other) = 0;

            virtual std::shared_ptr<Obj> unaryop(Token *op) = 0;
        };

        /// @brief The structure that represents EARL 32bit integers
        struct Int : public Obj {
            Int(int value = 0);

            /// @brief Fill the underlying data with some data
            /// @param value The value to use to fill
            void fill(int value);

            /// @brief Get the underlying integer value
            int value(void);

            void incr(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            int m_value;
        };

        struct Float : public Obj {
            Float(double value = 0);

            /// @brief Fill the underlying data with some data
            /// @param value The value to use to fill
            void fill(double value);

            /// @brief Get the underlying integer value
            double value(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            double m_value;
        };

        /// @brief The structure that represents EARL 32bit integers
        struct Bool : public Obj {
            Bool(bool value = false);

            /// @brief Get the underlying integer value
            bool value(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            bool m_value;
        };

        struct Char : public Obj {
            Char(std::string value = "");

            /// @brief Get the underlying string value
            char value(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            char m_value;
        };

        /// @brief The structure that represents EARL UNITs
        struct Void : public Obj {
            Void(void *value = nullptr);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            void *m_value;
        };

        struct Closure : public Obj {
            Closure(ExprClosure *expr_closure,
                    std::vector<std::pair<Token *, uint32_t>> params,
                    std::shared_ptr<Ctx> owner);

            StmtBlock *block(void);
            void load_parameters(std::vector<std::shared_ptr<earl::value::Obj>> &values, std::shared_ptr<Ctx> ctx);
            std::shared_ptr<Obj> call(std::vector<std::shared_ptr<earl::value::Obj>> &values, std::shared_ptr<Ctx> &ctx);
            size_t params_len(void) const;
            bool param_at_is_ref(size_t i) const;

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            ExprClosure *m_expr_closure;
            std::vector<std::pair<Token *, uint32_t>> m_params;
            std::shared_ptr<Ctx> m_owner;
        };

        /// @brief The structure that represents EARL lists.
        /// They can hold any value in any mix of them i.e.,
        /// list = [int, str, str, int, list[int, str]]
        struct List : public Obj {
            List(std::vector<std::shared_ptr<Obj>> value = {});

            /// @brief Fill the underlying data with some data
            /// @param value The value to use to fill
            void fill(std::vector<std::shared_ptr<Obj>> &value);

            /// @brief Get the underlying list value
            std::vector<std::shared_ptr<Obj>> &value(void);

            /// @brief Get the `nth` element from the list
            /// @note This is called from the intrinsic `nth` member function
            /// @param idx The object that contains the index
            /// @note `idx` MUST BE an integer value
            std::shared_ptr<Obj> nth(std::shared_ptr<Obj> &idx);

            /// @brief Reverse a list
            void rev(void);

            /// @brief Append a list of values to a list
            /// @param values The values to append
            void append(std::vector<std::shared_ptr<Obj>> values);

            void append(std::shared_ptr<Obj> value);

            /// @brief Remove an element in the list at a specific index
            /// @param idx The index of the element to remove
            void pop(std::shared_ptr<Obj> &idx);

            std::shared_ptr<List> filter(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx);

            void foreach(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx);

            std::shared_ptr<Obj> back(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            std::vector<std::shared_ptr<Obj>> m_value;
        };

        /// @brief The structure that represents EARL strings
        struct Str : public Obj {
            Str(std::string value = "");

            std::string value(void); // NOTE: needs optimization
            std::vector<std::shared_ptr<Char>> &value_raw(void);
            std::shared_ptr<Char> nth(std::shared_ptr<Obj> &idx);
            std::shared_ptr<List> split(std::shared_ptr<Obj> &delim);
            std::shared_ptr<Str> substr(std::shared_ptr<Obj> &idx1, std::shared_ptr<Obj> &idx2);
            void pop(std::shared_ptr<Obj> &idx);
            std::shared_ptr<Obj> back(void);
            void rev(void);
            void append(std::vector<std::shared_ptr<Obj>> &values);
            void append(std::shared_ptr<Obj> &c);
            std::shared_ptr<Str> filter(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx);
            void foreach(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx);
            void trim(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            std::vector<std::shared_ptr<Char>> m_value;
        };

        struct Module : public Obj {
            Module(std::shared_ptr<Ctx> ctx);

            /// @brief Get the module context
            std::shared_ptr<Ctx> &value(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            std::shared_ptr<Ctx> m_value;
        };

        struct Class : public Obj {
            Class(StmtClass *stmtclass, std::shared_ptr<Ctx> ctx);
            Class(const Class &other);

            const std::string &id(void) const;

            void load_class_members(std::vector<std::shared_ptr<Obj>> &args);

            void add_method(std::shared_ptr<function::Obj> func);
            void add_member(std::shared_ptr<variable::Obj> var);
            void add_member_assignee(Token *assignee);
            bool is_pub(void) const;

            [[deprecated]]
            std::shared_ptr<function::Obj> get_method(const std::string &id);
            [[deprecated]]
            std::shared_ptr<earl::variable::Obj> get_member(const std::string &id);
            [[deprecated]]
            std::vector<std::shared_ptr<earl::variable::Obj>> &get_members(void);

            std::shared_ptr<Ctx> &ctx(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            StmtClass *m_stmtclass;
            std::shared_ptr<Ctx> m_ctx;

            std::vector<std::shared_ptr<variable::Obj>> m_members;
            std::vector<std::shared_ptr<function::Obj>> m_methods;
            std::vector<Token *> m_member_assignees;
        };

        struct Enum : public Obj {
            Enum(StmtEnum *stmt,
                 std::unordered_map<std::string, std::shared_ptr<variable::Obj>> elems,
                 uint32_t attrs);

            const std::string &id(void) const;
            std::shared_ptr<variable::Obj> get_entry(const std::string &id);
            bool has_entry(const std::string &id) const;
            bool is_pub(void) const;
            std::unordered_map<std::string, std::shared_ptr<variable::Obj>> &
            extract(void);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            StmtEnum *m_stmt;
            std::unordered_map<std::string, std::shared_ptr<variable::Obj>> m_elems;
            Token *m_id;
            uint32_t m_attrs;
        };

        struct File : public Obj {
            File(std::shared_ptr<Str> fp, std::shared_ptr<Str> mode, std::fstream stream);

            void set_open(void);
            void set_closed(void);

            void dump(void);
            void close(void);
            std::shared_ptr<Str> read(void);
            void write(std::shared_ptr<Obj> value);
            void writelines(std::shared_ptr<List> &value);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            std::shared_ptr<Str> m_fp;
            std::shared_ptr<Str> m_mode;
            std::fstream m_stream;
            bool m_open;
        };

        struct Option : public Obj {
            Option(std::shared_ptr<Obj> value = nullptr);

            std::shared_ptr<Obj> &value(void);
            bool is_some(void) const;
            bool is_none(void) const;
            void set_value(std::shared_ptr<Obj> other);

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;

        private:
            std::shared_ptr<Obj> m_value;
        };

        struct Break : public Obj {
            Break() = default;

            /*** OVERRIDES ***/
            Type type(void) const                                              override;
            std::shared_ptr<Obj> binop(Token *op, std::shared_ptr<Obj> &other) override;
            bool boolean(void)                                                 override;
            void mutate(const std::shared_ptr<Obj> &other)                     override;
            std::shared_ptr<Obj> copy(void)                                    override;
            bool eq(std::shared_ptr<Obj> &other)                               override;
            std::string to_cxxstring(void)                                     override;
            void spec_mutate(Token *op, const std::shared_ptr<Obj> &other)     override;
            std::shared_ptr<Obj> unaryop(Token *op)                            override;
        };

        std::string type_to_str(earl::value::Type ty);
        const char *type_to_cstr(earl::value::Type ty);

        /// @brief Get an empty EARL value from a type
        /// @param s The string type to parse
        [[deprecated]] [[nodiscard]]
        Obj *of_str(const std::string &s);

        [[nodiscard]]
        bool type_is_compatable(Type ty1, Type ty2);

        /// @brief Check if two EARL values are type compatable
        /// @param obj1 The first object
        /// @param obj2 The second object
        [[nodiscard]]
        bool type_is_compatable(const Obj *const obj1, const Obj *const obj2);
    };

    /**
     * How variables are created in EARL during runtime
     */
    namespace variable {

        /// @brief The structure to represent EARL variables
        struct Obj {
            Obj(Token *id, std::shared_ptr<value::Obj> value, uint32_t attrs = 0);
            ~Obj() = default;

            /// @brief Get the identifier of this variable
            const std::string &id(void) const;
            /// @brief Check if a variable is in global scope
            [[deprecated]]
            bool is_global(void) const;
            /// @brief Get the actual value of this variable
            std::shared_ptr<value::Obj> value(void) const;
            /// @brief Get the type of this variable
            value::Type type(void) const;
            bool is_ref(void) const;
            bool is_pub(void) const;
            std::shared_ptr<Obj> copy(void);

        private:
            Token *m_id;
            std::shared_ptr<value::Obj> m_value;
            uint32_t m_attrs;
        };
    };

    /**
     * How functions are created in EARL during runtime
     */
    namespace function {

        /// @brief The structure to represent EARL functions
        struct Obj {
            Obj(StmtDef *stmtdef, std::vector<std::pair<Token *, uint32_t>> params);
            ~Obj() = default;

            const std::string &id(void) const;
            size_t params_len(void) const;
            StmtBlock *block(void) const;
            void load_parameters(std::vector<std::shared_ptr<earl::value::Obj>> &values, std::shared_ptr<FunctionCtx> &new_ctx);
            bool is_world(void) const;
            bool is_pub(void) const;
            Obj *copy(void);
            bool param_at_is_ref(size_t i) const;
            uint32_t attrs(void) const;

        private:
            StmtDef *m_stmtdef;
            std::vector<std::pair<Token *, uint32_t>> m_params;
        };
    };
};

#endif // EARL_H
