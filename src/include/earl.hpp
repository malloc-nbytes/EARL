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
#include <ctime>

#include "ast.hpp"
#include "token.hpp"

/// \brief Make sure that both obj0 and obj1 are compatible with
/// all binary opterations.
#define ASSERT_BINOP_COMPAT(obj0, obj1, op)                             \
    do {                                                                \
        if (!type_is_compatable(obj0, obj1)) {                          \
            Err::err_wtok(op);                                          \
            std::string __Msg = "value of type `"+earl::value::type_to_str(obj0->type()) \
                +"` is not compatible with value of type `"+earl::value::type_to_str(obj1->type())+"`"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

/// \brief Make sure that obj0 and obj1 are the same type
/// for binary operations.
#define ASSERT_BINOP_EXACT(obj0, obj1, op)                              \
    do {                                                                \
        if (obj0->type() != obj1->type()) {                             \
            Err::err_wtok(op);                                          \
            std::string __Msg = "value of type `"+earl::value::type_to_str(obj0->type()) \
                +"` is not exactly compatible with value of type `"+earl::value::type_to_str(obj1->type())+"`"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

/// \brief Make sure that obj0 and obj1 are compatable for mutation.
#define ASSERT_MUTATE_COMPAT(obj0, obj1, stmt)                          \
    do {                                                                \
        if (!type_is_compatable(obj0, obj1)) {                          \
            Err::err_wstmt(stmt);                                       \
            std::string __Msg = "value of type `"+earl::value::type_to_str(obj0->type()) \
                +"` is not compatible with value of type `"+earl::value::type_to_str(obj1->type())+"`"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

/// \brief If obj is const, fail.
#define ASSERT_CONSTNESS(obj, stmt)                                     \
    do {                                                                \
        if (obj->is_const()) {                                          \
            Err::err_wstmt(stmt);                                       \
            const std::string __Msg = "cannot mutate value with attribute @const"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
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
            Int=0,

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
            This, // 10

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

            /** EARL tuple type */
            Tuple,

            /** EARL slice type */
            Slice,

            /** EARL dictionary types */
            DictInt,
            DictStr,
            DictFloat, // 20
            DictChar,

            /** EARL type keyword type */
            TypeKW,

            /** EARL date type */
            Time,

            /** EARL continue keyword (no longer used) */
            Continue,

            /** EARL return keyword (no longer used) */
            Return,

            /** EARL function reference type */
            FunctionRef,

            /** EARL class reference type */
            ClassRef,
        };

        struct Obj;
        struct Char;

        using ListIterator      = std::vector<std::shared_ptr<Obj>>::iterator;
        using StrIterator       = std::vector<std::shared_ptr<Char>>::iterator;
        using DictIntIterator   = std::unordered_map<int, std::shared_ptr<Obj>>::iterator;
        using DictCharIterator  = std::unordered_map<char, std::shared_ptr<Obj>>::iterator;
        using DictFloatIterator = std::unordered_map<double, std::shared_ptr<Obj>>::iterator;
        using DictStrIterator   = std::unordered_map<std::string, std::shared_ptr<Obj>>::iterator;
        using Iterator          = std::variant<ListIterator, StrIterator, DictIntIterator, DictCharIterator, DictFloatIterator, DictStrIterator>;

        /// @brief The base abstract class that all
        ///        EARL value objects inherit from
        struct Obj {
            virtual ~Obj() {}

            /// @brief Borrow the owner of the value as a ptr.
            ///        This function is used in the intrinsic function
            ///        observe()
            /// @return A pointer to the owner.
            virtual earl::variable::Obj *borrow_owner(void);

            /// @brief Get the doc comments from this value's owner
            /// @return string of the doc comment(s)
            virtual std::string get_info_from_owner(void);

            /// @brief Set the owner of this value
            /// @param owner The owner to set
            virtual void set_owner(earl::variable::Obj *owner);

            /// @brief Set this value as constant
            virtual void set_const(void);

            /// @brief Unset this value as constant
            virtual void unset_const(void);

            /// @brief Check if this value is constant
            /// @return True if constant, false if otherwise
            virtual bool is_const(void) const;

            /// @brief Get the type of the value
            /// @return The type of this value
            virtual Type type(void) const = 0;

            /// @brief Perform a binary operation on THIS
            ///        value with another value
            /// @param op The binary operator
            /// @param other The object to perform the binop with
            /// @return A value object of the result of the binary operation
            virtual std::shared_ptr<Obj> binop(Token *op, Obj *other);

            /// @brief Get the evaluation when put into
            ///        a conditional to evaluate to either true or false
            virtual bool boolean(void);

            /// @brief Modify the underlying data of THIS value
            /// with the underlying value of another object
            /// @param other The value to mutate THIS instance with
            /// @param stmt The statement (used for error reporting)
            virtual void mutate(Obj *other, StmtMut *stmt);

            /// @brief Copy THIS instance
            /// @return A value object of the copy
            virtual std::shared_ptr<Obj> copy(void);

            /// @brief Check the equality of two objects, not just by their values.
            /// @param other The object to compare
            /// @return true on equal, false otherwise
            virtual bool eq(Obj *other);

            /// @brief Convert the value of an object to a cxx std::string
            /// @return The stringified version of the value
            virtual std::string to_cxxstring(void);

            /// @brief Handles special mutation operators such as +=, -=, /= etc.
            /// @param op The binary operator
            /// @param other The right-hand-side of the operator
            /// @param stmt The statement that is being evaluated, used for error reporing
            virtual void spec_mutate(Token *op, Obj *other, StmtMut *stmt);

            /// @brief Handles a unary operator (!, `~, etc.)
            /// @param op The unary operator
            /// @return The result of the unary operator on the this value
            virtual std::shared_ptr<Obj> unaryop(Token *op);

            /// @brief Check if this value is iterable
            /// @return True if it is, false if otherwise
            virtual bool is_iterable(void) const;

            /// @brief Retrieve the iterator of this value
            /// @return This value's iterator
            virtual Iterator iter_begin(void);

            /// @brief Retrieve the end iterator of this value
            /// @return This value's end iterator
            virtual Iterator iter_end(void);

            /// @brief Advance the iterator
            /// @param it The iterator to advance
            virtual void iter_next(Iterator &it);

            /// @brief Add two value objects together
            /// @param op The binary operator
            /// @param other The other value object to add
            virtual std::shared_ptr<Obj> add(Token *op, Obj *other);

            /// @brief Subtract two value objects together
            /// @param op The binary operator
            /// @param other The other value object to subtract
            virtual std::shared_ptr<Obj> sub(Token *op, Obj *other);

            /// @brief Multiplay two value objects together
            /// @param op The binary operator
            /// @param other The other value object to multiply
            virtual std::shared_ptr<Obj> multiply(Token *op, Obj *other);

            /// @brief Divide two value objects together
            /// @param op The binary operator
            /// @param other The other value object to divide
            virtual std::shared_ptr<Obj> divide(Token *op, Obj *other);

            /// @brief Mod two value objects together
            /// @param op The binary operator
            /// @param other The other value object to mod
            virtual std::shared_ptr<Obj> modulo(Token *op, Obj *other);

            /// @brief Power two value objects together
            /// @param op The binary operator
            /// @param other The other value object that is the power
            virtual std::shared_ptr<Obj> power(Token *op, Obj *other);

            /// @brief Find greater than or equal to equality of two value objects
            /// @param op The binary operator
            /// @param other The other value object to compare
            virtual std::shared_ptr<Obj> gtequality(Token *op, Obj *other);

            /// @brief Find the equality of two value objects
            /// @param op The binary operator
            /// @param other The other value object to compare
            virtual std::shared_ptr<Obj> equality(Token *op, Obj *other);

            /// @brief Perform bitwise operations on two value objects
            /// @param op The binary operator
            /// @param other The other value object to use
            virtual std::shared_ptr<Obj> bitwise(Token *op, Obj *other);

            /// @brief Bitshift a value object with another
            /// @param op The binary operator
            /// @param other The other value object to use
            virtual std::shared_ptr<Obj> bitshift(Token *op, Obj *other);

        protected:
            /// @brief The value object is constant
            bool m_const = false;

            /// @brief The value object is iterable
            bool m_iterable = false;

            /// @brief The owner of this value object
            earl::variable::Obj *m_var_owner = nullptr;
        };

        /// @brief The function reference value object. It is created
        ///        by using a function identifier without parens.
        struct FunctionRef : public Obj {
            FunctionRef(std::shared_ptr<earl::function::Obj> fun);

            /// @brief Get a copy of the function
            /// @return The copy of m_fun
            std::shared_ptr<earl::function::Obj> value(void);

            /// @brief Get the embedded doc comments in the function
            /// @return A string of all doc comments
            const std::vector<std::string> &get_info(void);

            // Implements
            Type type(void) const                                                         override;
            std::string to_cxxstring(void)                                                override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;

        private:
            /// @brief The actual EARL function
            std::shared_ptr<earl::function::Obj> m_fun;
        };

        /// @brief The class reference value object. It is created
        ///        by using a class identifier without parens.
        struct ClassRef : public Obj {
            ClassRef(StmtClass *stmt);

            /// @brief Get the embedded doc comments in the class
            /// @return A string of all doc comments
            const std::vector<std::string> &get_info(void);

            /// @brief Get the class statement
            /// @return The statement m_stmt
            StmtClass *get_stmt(void);

            // Implements
            Type type(void) const                                                         override;
            std::string to_cxxstring(void)                                                override;
            std::shared_ptr<Obj> copy(void)                                               override;

        private:
            StmtClass *m_stmt;
        };

        struct TypeKW : public Obj {
            TypeKW(Type ty);

            Type ty(void) const;

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            Type m_ty;
        };

        /// @brief The structure that represents EARL 32bit integers
        struct Int : public Obj {
            Int(int value = 0);

            /// @brief Fill the underlying data with some data
            /// @param value The value to use to fill
            void fill(int value);

            /// @brief Get the underlying integer value
            /// @return a copy of m_value
            int value(void);

            /// @brief Increment m_value by 1
            void incr(void);

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            void spec_mutate(Token *op, Obj *other, StmtMut *stmt)                        override;
            std::shared_ptr<Obj> unaryop(Token *op)                                       override;
            void set_const(void)                                                          override;
            std::shared_ptr<Obj> add(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> sub(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> multiply(Token *op, Obj *other)                          override;
            std::shared_ptr<Obj> divide(Token *op, Obj *other)                            override;
            std::shared_ptr<Obj> modulo(Token *op, Obj *other)                            override;
            std::shared_ptr<Obj> power(Token *op, Obj *other)                             override;
            std::shared_ptr<Obj> gtequality(Token *op, Obj *other)                        override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;
            std::shared_ptr<Obj> bitwise(Token *op, Obj *other)                           override;
            std::shared_ptr<Obj> bitshift(Token *op, Obj *other)                          override;

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

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            void spec_mutate(Token *op, Obj *other, StmtMut *stmt)                        override;
            std::shared_ptr<Obj> unaryop(Token *op)                                       override;
            void set_const(void)                                                          override;
            std::shared_ptr<Obj> add(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> sub(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> multiply(Token *op, Obj *other)                          override;
            std::shared_ptr<Obj> divide(Token *op, Obj *other)                            override;
            std::shared_ptr<Obj> power(Token *op, Obj *other)                             override;
            std::shared_ptr<Obj> gtequality(Token *op, Obj *other)                        override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            double m_value;
        };

        /// @brief The structure that represents EARL 32bit integers
        struct Bool : public Obj {
            Bool(bool value = false);

            /// @brief Get the underlying integer value
            bool value(void);
            void toggle(void);

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            std::shared_ptr<Obj> unaryop(Token *op)                                       override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            bool m_value;
        };

        struct Char : public Obj {
            // Char(std::string value = "");
            Char(char value = '\0');

            /// @brief Get the underlying string value
            char value(void);
            std::shared_ptr<Bool> isupper(void) const;
            std::shared_ptr<Bool> islower(void) const;
            std::shared_ptr<Char> toupper(void) const;
            std::shared_ptr<Char> tolower(void) const;

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;
            std::shared_ptr<Obj> add(Token *op, Obj *other)                               override;

        private:
            char m_value;
        };

        /// @brief The structure that represents EARL UNITs
        struct Void : public Obj {
            Void() = default;

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
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
            Token *tok(void) const;

            // Implements
            Type type(void) const                                                         override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            std::string to_cxxstring(void)                                                override;

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

            /// @brief Get the underlying list value
            std::vector<std::shared_ptr<Obj>> &value(void);

            /// @brief Get a sublist of the vector from `start` to `finish`
            std::vector<std::shared_ptr<Obj>> slice(Obj *start, Obj *end, Expr *expr);

            /// @brief Get the `nth` element from the list
            /// @note This is called from the intrinsic `nth` member function
            /// @param idx The object that contains the index
            /// @note `idx` MUST BE an integer value
            std::shared_ptr<Obj> nth(std::shared_ptr<Obj> &idx, Expr *expr);

            /// @brief Reverse a list
            std::shared_ptr<List> rev(void);
            void append_copy(std::shared_ptr<Obj> value);
            void append_copy(std::vector<std::shared_ptr<Obj>> &values);

            /// @brief Append a list of values to a list
            /// @param values The values to append
            void append(std::vector<std::shared_ptr<Obj>> &values);
            void append(std::shared_ptr<Obj> value);

            /// @brief Remove an element in the list at a specific index
            /// @param idx The index of the element to remove
            void pop(Obj *idx, Expr *expr);
            std::shared_ptr<List> filter(Obj *closure, std::shared_ptr<Ctx> &ctx);
            void foreach(Obj *closure, std::shared_ptr<Ctx> &ctx);
            std::shared_ptr<List> map(Closure *closure, std::shared_ptr<Ctx> &ctx);
            std::shared_ptr<Obj> back(void);
            std::shared_ptr<Bool> contains(Obj *value);
            std::shared_ptr<Obj> fold(Closure *closure, std::shared_ptr<Obj> acc, std::shared_ptr<Ctx> &ctx);

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            void spec_mutate(Token *op, Obj *other, StmtMut *stmt)                        override;
            Iterator iter_begin(void)                                                     override;
            Iterator iter_end(void)                                                       override;
            void iter_next(Iterator &it)                                                  override;
            std::shared_ptr<Obj> add(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            std::vector<std::shared_ptr<Obj>> m_value;
        };

        struct Slice : public Obj {
            Slice(std::shared_ptr<Obj> start, std::shared_ptr<Obj> end);

            std::shared_ptr<Obj> &start(void);
            std::shared_ptr<Obj> &end(void);

            // Implements
            Type type(void) const                                                         override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;

        private:
            std::shared_ptr<Obj> m_start;
            std::shared_ptr<Obj> m_end;
        };

        struct Tuple : public Obj {
            Tuple(std::vector<std::shared_ptr<Obj>> values = {});

            std::vector<std::shared_ptr<Obj>> &value(void);
            std::shared_ptr<Obj> nth(Obj *idx, Expr *expr);
            std::shared_ptr<Obj> back(void);
            std::shared_ptr<Tuple> filter(Obj *closure, std::shared_ptr<Ctx> &ctx);
            void foreach(Obj *closure, std::shared_ptr<Ctx> &ctx);
            std::shared_ptr<Tuple> rev(void);
            std::shared_ptr<Bool> contains(Obj *value);
            std::shared_ptr<Obj> fold(Closure *closure, std::shared_ptr<Obj> acc, std::shared_ptr<Ctx> &ctx);

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            Iterator iter_begin(void)                                                     override;
            Iterator iter_end(void)                                                       override;
            void iter_next(Iterator &it)                                                  override;
            std::shared_ptr<Obj> add(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            std::vector<std::shared_ptr<Obj>> m_values;
        };

        struct Time : public Obj {
            Time(void);
            Time(std::time_t now);

            std::shared_ptr<Time> update(void);
            std::shared_ptr<Int> raw(void);
            std::shared_ptr<Tuple> readable(void);
            std::shared_ptr<Int> years(void);
            std::shared_ptr<Int> months(void);
            std::shared_ptr<Int> days(void);
            std::shared_ptr<Int> hours(void);
            std::shared_ptr<Int> minutes(void);
            std::shared_ptr<Int> seconds(void);

            // Implements
            Type type(void) const override;
            void mutate(Obj *other, StmtMut *stmt) override;
            std::shared_ptr<Obj> copy(void) override;
            bool eq(Obj *other) override;
            std::string to_cxxstring(void) override;
            std::shared_ptr<Obj> gtequality(Token *op, Obj *other) override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other) override;

        private:
            std::time_t m_now;
        };

        /// @brief The structure that represents EARL strings
        struct Str : public Obj {
            Str(std::string value = "");
            Str(std::vector<std::shared_ptr<Char>> chars);

            std::string value(void);
            const std::string &value_asref(void) const;
            std::vector<std::shared_ptr<Char>> value_as_earlchar(void);
            std::shared_ptr<Char> __get_elem(size_t idx);
            std::shared_ptr<Char> nth(Obj *idx, const Expr *const expr);
            std::shared_ptr<List> split(Obj *delim, Expr *expr);
            std::shared_ptr<Str> substr(Obj *idx1, Obj *idx2, Expr *expr);
            void pop(Obj *idx, Expr *expr);
            std::shared_ptr<Obj> back(void);
            std::shared_ptr<Str> rev(void);
            void append(char c);
            void append(const std::string &value);
            void append(std::vector<std::shared_ptr<Obj>> &values, Expr *expr);
            void append(Obj *c);
            std::shared_ptr<Str> filter(Obj *closure, std::shared_ptr<Ctx> &ctx);
            void foreach(Obj *closure, std::shared_ptr<Ctx> &ctx);
            std::shared_ptr<Bool> contains(Char *value);
            void update_changed(void);
            std::shared_ptr<earl::value::Str> trim(Expr *expr);
            void remove_char(int idx, Expr *expr);
            std::shared_ptr<Bool> startswith(const Str *const str) const;
            std::shared_ptr<Bool> endswith(const Str *const str) const;

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            void spec_mutate(Token *op, Obj *other, StmtMut *stmt)                        override;
            Iterator iter_begin(void)                                                     override;
            Iterator iter_end(void)                                                       override;
            void iter_next(Iterator &it)                                                  override;
            std::shared_ptr<Obj> add(Token *op, Obj *other)                               override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            std::string m_value;
            std::vector<std::shared_ptr<Char>> m_chars;
            std::vector<unsigned> m_changed;
        };

        struct Module : public Obj {
            Module(std::shared_ptr<Ctx> ctx);

            /// @brief Get the module context
            std::shared_ptr<Ctx> &value(void);

            // Implements
            Type type(void) const                                                         override;

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
            bool is_experimental(void) const;
            void disable_experimental_flag(void);

            [[deprecated]]
            std::vector<std::shared_ptr<earl::variable::Obj>> &get_members(void);

            std::shared_ptr<Ctx> &ctx(void);

            // Implements
            Type type(void) const                                                         override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            std::string to_cxxstring(void)                                                override;

        private:
            StmtClass *m_stmtclass;
            std::shared_ptr<Ctx> m_ctx;

            std::vector<std::shared_ptr<variable::Obj>> m_members;
            std::vector<std::shared_ptr<function::Obj>> m_methods;
            std::vector<Token *> m_member_assignees;
        };

        template <typename T>
        struct Dict : public Obj {
            Dict(Type kty);

            void insert(T key, std::shared_ptr<Obj> value);
            Type ktype(void) const;
            std::shared_ptr<Obj> nth(Obj *key, Expr *expr);
            std::unordered_map<T, std::shared_ptr<Obj>> &extract(void);
            bool has_key(T key) const;
            bool has_value(Obj *value) const;

            // Implements
            Type type(void) const                                                         override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::string to_cxxstring(void)                                                override;
            Iterator iter_begin(void)                                                     override;
            Iterator iter_end(void)                                                       override;
            void iter_next(Iterator &it)                                                  override;

        private:
            std::unordered_map<T, std::shared_ptr<Obj>> m_map;
            Type m_kty;
        };

        struct Enum : public Obj {
            Enum(StmtEnum *stmt,
                 std::unordered_map<std::string, std::shared_ptr<variable::Obj>> elems,
                 uint32_t attrs,
                 std::vector<std::string> info);

            const std::string &id(void) const;
            std::shared_ptr<variable::Obj> get_entry(const std::string &id);
            bool has_entry(const std::string &id) const;
            bool is_pub(void) const;
            std::unordered_map<std::string, std::shared_ptr<variable::Obj>> &extract(void);
            const std::vector<std::string> &get_info(void);
            bool is_experimental(void) const;
            void disable_experimental_flag(void);

            // Implements
            Type type(void) const                                                         override;
            std::string to_cxxstring(void)                                                override;

        private:
            StmtEnum *m_stmt;
            std::unordered_map<std::string, std::shared_ptr<variable::Obj>> m_elems;
            Token *m_id;
            uint32_t m_attrs;
            std::vector<std::string> m_info;
        };

        struct File : public Obj {
            enum class Mode {
                Read = 1 << 0,
                Write = 1 << 1,
                Binary = 1 << 2,
            };

            File(std::shared_ptr<Str> fp,
                 std::shared_ptr<Str> mode,
                 std::fstream stream);

            void set_open(void);
            void set_closed(void);
            void dump(void);
            void close(void);
            std::shared_ptr<Str> read(void);
            void write(std::shared_ptr<Obj> value);
            void writelines(std::shared_ptr<List> &value);

            // Implements
            Type type(void) const                                                         override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            std::string to_cxxstring(void)                                                override;

        private:
            std::shared_ptr<Str> m_fp;
            std::shared_ptr<Str> m_mode;
            std::fstream m_stream;
            bool m_open;
            uint32_t m_mode_actual;
        };

        struct Option : public Obj {
            Option(std::shared_ptr<Obj> value = nullptr);

            std::shared_ptr<Obj> &value(void);
            bool is_some(void) const;
            bool is_none(void) const;
            void set_value(std::shared_ptr<Obj> other);

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> binop(Token *op, Obj *other)                             override;
            bool boolean(void)                                                            override;
            void mutate(Obj *other, StmtMut *stmt)                                        override;
            std::shared_ptr<Obj> copy(void)                                               override;
            bool eq(Obj *other)                                                           override;
            std::shared_ptr<Obj> unaryop(Token *op)                                       override;
            std::string to_cxxstring(void)                                                override;
            std::shared_ptr<Obj> equality(Token *op, Obj *other)                          override;

        private:
            std::shared_ptr<Obj> m_value;
        };

        struct Break : public Obj {
            Break() = default;

            // Implements
            Type type(void) const                                                         override;
        };

        struct Continue : public Obj {
            Continue() = default;

            // Implements
            Type type(void) const                                                         override;
        };

        struct Return : public Obj {
            Return() = default;

            // Implements
            Type type(void) const                                                         override;
            std::shared_ptr<Obj> copy(void)                                               override;
        };

        std::string type_to_str(earl::value::Type ty);
        const char *type_to_cstr(earl::value::Type ty);

        [[nodiscard]]
        bool type_is_compatable(Type ty1, Type ty2);

        /// @brief Check if two EARL values are type compatable
        /// @param obj1 The first object
        /// @param obj2 The second object
        [[nodiscard]]
        bool type_is_compatable(const Obj *const obj1, const Obj *const obj2);

        bool is_typekw(const std::string &id);
        Type get_typekw_proper(const std::string &id);

        [[nodiscard]]
        bool is_builtin_ident(const std::string &id);

        std::shared_ptr<Obj> get_builtin_ident(const std::string &id, std::shared_ptr<Ctx> &ctx);
    };

    /**
     * How variables are created in EARL during runtime
     */
    namespace variable {
        using event_listener_t = std::optional<std::variant<std::shared_ptr<earl::value::FunctionRef>,
                                                            std::shared_ptr<earl::value::Closure>>
                                               >;

        /// @brief The structure to represent EARL variables
        struct Obj {
            Obj(Token *id, std::shared_ptr<value::Obj> value, uint32_t attrs = 0, std::string info = "");
            ~Obj() = default;

            Token *gettok(void);

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
            void reset(std::shared_ptr<value::Obj> value);
            std::string get_info(void);
            uint32_t attrs(void) const;
            void disable_experimental_flag(void);
            void set_event_listener(event_listener_t m_event_listener);

            // Public for speed
            event_listener_t m_event_listener;

        private:
            Token *m_id;
            std::shared_ptr<value::Obj> m_value;
            uint32_t m_attrs;
            bool m_constness;
            std::string m_info;
        };
    };

    /**
     * How functions are created in EARL during runtime
     */
    namespace function {

        /// @brief The structure to represent EARL functions
        struct Obj {
            Obj(StmtDef *stmtdef,
                std::vector<std::pair<std::pair<Token *, __Type *>, uint32_t>> params,
                Token *tok,
                std::optional<__Type *> explicit_type,
                std::vector<std::string> info);
            ~Obj() = default;

            bool is_explicit_typed(void) const;
            __Type *get_explicit_type(void);
            StmtDef *get_stmtdef(void);
            Token *gettok(void);
            const std::string &id(void) const;
            size_t params_len(void) const;
            StmtBlock *block(void) const;
            void load_parameters(std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                 std::shared_ptr<FunctionCtx> &new_ctx,
                                 std::shared_ptr<Ctx> &old_ctx);
            bool is_world(void) const;
            bool is_pub(void) const;
            std::shared_ptr<Obj> copy(void);
            bool param_at_is_ref(size_t i) const;
            uint32_t attrs(void) const;
            const std::vector<std::string> &info(void) const;
            void change_id(const std::string &newid);
            void disable_experimental_flag(void);

        private:
            StmtDef *m_stmtdef;
            std::vector<std::pair<std::pair<Token *, __Type *>, uint32_t>> m_params;
            Token *m_tok;
            std::optional<__Type *> m_explicit_type;
            std::vector<std::string> m_info;
            std::string m_id;
        };
    };
};

/*** DICTIONARY IMPLEMENTATION ***/

#include <cassert>

#include "utils.hpp"
#include "err.hpp"

template <typename T>
earl::value::Dict<T>::Dict::Dict(earl::value::Type kty) {
    m_kty = kty;
    m_iterable = true;
}

template <typename T> void
earl::value::Dict<T>::insert(T key, std::shared_ptr<earl::value::Obj> value) {
    m_map[key] = value;
}

template <typename T> earl::value::Type
earl::value::Dict<T>::ktype(void) const {
    return m_kty;
}

template <typename T>
std::shared_ptr<earl::value::Obj>
earl::value::Dict<T>::nth(earl::value::Obj *key, Expr *expr) {
    if constexpr (std::is_same_v<T, int>) {
        if (key->type() != earl::value::Type::Int) {
            Err::err_wexpr(expr);
            const std::string msg = "key must be of type int";
            throw InterpreterException(msg);
        }
        int k = dynamic_cast<earl::value::Int *>(key)->value();
        auto value = m_map.find(k);
        if (value == m_map.end())
            return std::make_shared<earl::value::Option>();
        return std::make_shared<earl::value::Option>(value->second);
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        if (key->type() != earl::value::Type::Str) {
            Err::err_wexpr(expr);
            const std::string msg = "key must be of type str";
            throw InterpreterException(msg);
        }
        std::string k = dynamic_cast<earl::value::Str *>(key)->value();
        auto value = m_map.find(k);
        if (value == m_map.end())
            return std::make_shared<earl::value::Option>();
        return std::make_shared<earl::value::Option>(value->second);
    }
    else if constexpr (std::is_same_v<T, double>) {
        if (key->type() != earl::value::Type::Float) {
            Err::err_wexpr(expr);
            const std::string msg = "key must be of type float";
            throw InterpreterException(msg);
        }
        double k = dynamic_cast<earl::value::Float *>(key)->value();
        auto value = m_map.find(k);
        if (value == m_map.end())
            return std::make_shared<earl::value::Option>();
        return std::make_shared<earl::value::Option>(value->second);
    }
    else if constexpr (std::is_same_v<T, char>) {
        if (key->type() != earl::value::Type::Char) {
            Err::err_wexpr(expr);
            const std::string msg = "key must be of type char";
            throw InterpreterException(msg);
        }
        char k = dynamic_cast<earl::value::Char *>(key)->value();
        auto value = m_map.find(k);
        if (value == m_map.end())
            return std::make_shared<earl::value::Option>();
        return std::make_shared<earl::value::Option>(value->second);
    }
    assert(false && "unreachable");
    return nullptr; // unreachable
}

template <typename T> std::unordered_map<T, std::shared_ptr<earl::value::Obj>> &
earl::value::Dict<T>::extract(void) {
    return m_map;
}

template <typename T> bool
earl::value::Dict<T>::has_key(T key) const {
    return m_map.find(key) != m_map.end();
}

template <typename T> bool
earl::value::Dict<T>::has_value(earl::value::Obj *value) const {
    for (auto &pair : m_map)
        if (pair.second->eq(value))
            return true;
    return false;
}

// Implements
template <typename T> earl::value::Type
earl::value::Dict<T>::type(void) const {
    switch (m_kty) {
    case earl::value::Type::Int: return Type::DictInt;
    case earl::value::Type::Str: return Type::DictStr;
    case earl::value::Type::Char: return Type::DictChar;
    case earl::value::Type::Float: return Type::DictFloat;
    default: assert(false && "unreachable"); break;
    }
    return (earl::value::Type)0; // unreachable
}

template <typename T> void
earl::value::Dict<T>::mutate(earl::value::Obj *other, StmtMut *stmt) {
    UNIMPLEMENTED("Dict::mutate");
}

template <typename T> std::shared_ptr<earl::value::Obj>
earl::value::Dict<T>::copy(void) {
    auto new_dict = std::make_shared<Dict<T>>(m_kty);
    for (auto &pair : m_map)
        new_dict->insert(pair.first, pair.second->copy());
    return new_dict;
}

template <typename T> bool
earl::value::Dict<T>::eq(earl::value::Obj *other) {
    UNIMPLEMENTED("Dict::eq");
}

template <typename T> std::string
earl::value::Dict<T>::to_cxxstring(void) {
    std::string res = "<" + earl::value::type_to_str(this->type()) + " { ";
    auto map = this->extract();
    int i = 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
        using Tx = std::decay_t<T>;
        if constexpr (std::is_same_v<Tx, int>)
            res += std::to_string(it->first);
        else if constexpr (std::is_same_v<Tx, double>)
            res += std::to_string(it->first);
        else if constexpr (std::is_same_v<Tx, char>)
            res += std::string(1, it->first);
        else if constexpr (std::is_same_v<Tx, std::string>)
            res += it->first;
        else
            res += it->first;
        res += ": ";
        res += it->second->to_cxxstring();
        if (i != map.size()-1)
            res += ", ";
        ++i;
    }
    res += " }>";
    return res;
}

template <typename T> earl::value::Iterator
earl::value::Dict<T>::iter_begin(void) {
    if constexpr (std::is_same_v<std::decay_t<T>, int> ||
                  std::is_same_v<std::decay_t<T>, double> ||
                  std::is_same_v<std::decay_t<T>, char> ||
                  std::is_same_v<std::decay_t<T>, std::string>)
        return m_map.begin();
    else
        static_assert("Dictionary Iterator: Unsupported BEGIN type");
    return {}; // unreachable
}

template <typename T> earl::value::Iterator
earl::value::Dict<T>::iter_end(void) {
    if constexpr (std::is_same_v<std::decay_t<T>, int> ||
                  std::is_same_v<std::decay_t<T>, double> ||
                  std::is_same_v<std::decay_t<T>, char> ||
                  std::is_same_v<std::decay_t<T>, std::string>)
        return m_map.end();
    else
        static_assert("Dictionary Iterator: Unsupported END type");
    return {}; // unreachable
}

template <typename T>
void
earl::value::Dict<T>::iter_next(earl::value::Iterator &it) {
    std::visit([&](auto &iter) {
        std::advance(iter, 1);
    }, it);
}

#endif // EARL_H
