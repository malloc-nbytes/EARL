#include "err.hpp"
#include "earl.hpp"
#include "utils.hpp"

using namespace earl::value;

/*** INT ***/

Int::Int(std::optional<int> value) : m_value(value) {}

int Int::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

void Int::fill(int value) {
    m_value = value;
}

Type Int::type(void) const {
    return Type::Int;
}

/*** STR ***/

Str::Str(std::optional<std::string> value) : m_value(std::move(value)) {}

std::string &Str::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

void Str::fill(std::string value) {
    m_value = std::move(value);
}

Type Str::type(void) const {
    return Type::Str;
}

/*** VOID ***/

Void::Void(std::optional<void *> value) : m_value(value) {}

void *Void::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

Type Void::type(void) const {
    return Type::Void;
}

/*** LIST ***/

List::List(std::optional<std::vector<Obj>> value) : m_value(std::move(value)) {}

void List::fill(std::vector<Obj> value) {
    m_value = std::move(value);
}

std::vector<Obj> &List::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

Type List::type(void) const {
    return Type::List;
}

/*** OTHER ***/

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

std::unique_ptr<Obj> earl::value::of_str(const std::string &s) {
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

