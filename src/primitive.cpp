#include <vector>
#include <unordered_map>

#include "earl.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "err.hpp"

std::unordered_map<earl::primitive::Type, std::vector<earl::primitive::Type>> earl::primitive::compat_types;

static const std::unordered_map<std::string, earl::primitive::Type> str_to_type_map = {
    {COMMON_EARLTY_INT32, earl::primitive::Type::Int},
    {COMMON_EARLTY_STR, earl::primitive::Type::Str},
    {COMMON_EARLTY_UNIT, earl::primitive::Type::Void},
};

void earl::primitive::fill_typemap(void) {
    compat_types[earl::primitive::Type::Int] = std::vector{earl::primitive::Type::Int};
    compat_types[earl::primitive::Type::Str] = std::vector{earl::primitive::Type::Str};
    compat_types[earl::primitive::Type::List] = std::vector{earl::primitive::Type::List};
    compat_types[earl::primitive::Type::Void] = std::vector{earl::primitive::Type::Void};
}

/*** INT ***/

earl::types::Int::Int(int value) : m_value(value) {}

earl::types::PrimitiveType earl::types::Int::get_type(void) const {
    return earl::types::PrimitiveType::Int;
}

/*** STR ***/

earl::types::Str::Str(std::string value) : m_value(std::move(value)) {}

earl::types::PrimitiveType earl::types::Str::get_type(void) const {
    return earl::types::PrimitiveType::Str;
}

/*** VOID ***/

earl::types::Void::Void(void *value) : m_value(value) {}

earl::types::PrimitiveType earl::types::Void::get_type(void) const {
    return earl::types::PrimitiveType::Void;
}

/*** LIST ***/

earl::types::List::List(std::vector<Type> value) : m_value(std::move(value)) {}

earl::types::PrimitiveType earl::types::List::get_type(void) const {
    return earl::types::PrimitiveType::List;
}

bool earl::primitive::types_compatable(Type ty1, Type ty2) {
    auto &candidates = earl::primitive::compat_types.at(ty1);
    for (auto &potential_ty : candidates) {
        if (potential_ty == ty2)
            return true;
    }
    return false;
}

bool earl::primitive::types_compatable(const std::vector<earl::primitive::Type> &ty1,
                                       const std::vector<earl::primitive::Type> &ty2) {
    if (ty1.size() != ty2.size())
        return false;
    for (size_t i = 0; i < ty1.size(); ++i) {
        if (ty1.at(i) != ty2.at(i))
            return false;
    }
    return true;
}

static void parse_list_type(const char *s, std::vector<earl::primitive::Type> &acc) {
    if (s[0] == '[') {
        acc.push_back(earl::primitive::Type::List);
        parse_list_type(++s, acc);
    }
    else {
        int pos = 0;
        while (s[pos] != ']' && s[pos++] != '\0');
        auto primitive = earl::primitive::of_str(std::string(s, s+pos));
        acc.insert(acc.end(), primitive.begin(), primitive.end());
    }
}

std::vector<earl::primitive::Type> earl::primitive::of_str(const std::string &s) {
    if (s[0] == '[') {
        std::vector<earl::primitive::Type> acc;
        parse_list_type(s.c_str(), acc);
        return acc;
    }
    auto entry = str_to_type_map.find(s);
    if (entry == str_to_type_map.end()) {
        ERR_WARGS(Err::Type::Fatal, "%s is not a valid EARL type", s.c_str());
    }
    return std::vector{entry->second};
}
