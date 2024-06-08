#include <iostream>
#include <unordered_map>

#include "earlty.hpp"
#include "common.hpp"
#include "err.hpp"

const std::unordered_map<std::string, EarlTy::Type> EarlTy::typemap = {
    {COMMON_EARLTY_INT32, EarlTy::Type::Int},
    {COMMON_EARLTY_STR, EarlTy::Type::Str},
    {COMMON_EARLTY_UNIT, EarlTy::Type::Void},
};

const std::unordered_map<EarlTy::Type, std::vector<EarlTy::Type>> EarlTy::m_earl_compat_tys = {
    {EarlTy::Type::Int, {EarlTy::Type::Int}},
    {EarlTy::Type::Str, {EarlTy::Type::Str}},
};

// EarlTy::Result EarlTy::of_str2(const std::string &s) {
//     uint16_t depth = 0;

//     size_t i = 0;
//     std::string buf = "";

//     for (; i < s.size() && s[i] == '['; ++i) ++depth;

//     while (i < s.size() && isalpha(s[i])) {
//         std::cout << s[i] << std::endl;
//         buf += s[i];
//         ++i;
//     }

//     return EarlTy::Result{EarlTy::of_str(buf), depth};
// }

EarlTy::Type EarlTy::of_str(const std::string &s) {
    auto it = typemap.find(s);
    if (it != typemap.end()) {
        return it->second;
    } else {
        ERR_WARGS(Fatal, "EarlTy::of_str: type `%s` is not a valid EARL type", s.c_str());
    }
}

bool EarlTy::earlvar_type_compat(EarlTy::Type ty1, EarlTy::Type ty2) {
    auto entry = EarlTy::m_earl_compat_tys.find(ty1);
    if (entry == EarlTy::m_earl_compat_tys.end()) {
        ERR_WARGS(Err::Type::Fatal, "the type %d is not a valid EARL type",
                  static_cast<int>(ty1));
    }
    for (auto compat : entry->second) {
        if (compat == ty2) {
            return true;
        }
    }
    return false;
}

std::string EarlTy::to_string(Type ty) {
    switch (ty) {
    case EarlTy::Type::Int: return COMMON_EARLTY_INT32;
    case EarlTy::Type::Str: return COMMON_EARLTY_STR;
    case EarlTy::Type::Void: return COMMON_EARLTY_UNIT;
    default:
        ERR_WARGS(ErrType::Fatal, "unknown EarlTy::Type: %d", (int)ty);
    }
}
