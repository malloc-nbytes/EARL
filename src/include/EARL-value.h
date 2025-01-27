#ifndef EARL_VALUE_H
#define EARL_VALUE_H

enum EARL_value_type {
        EARL_VALUE_TYPE_INTEGER,
};

struct EARL_value {
        enum EARL_value_type type;
        union {
                int integer;
        } value;
};

struct EARL_value *
EARL_value_alloc(enum EARL_value_type type, void *data);

int
EARL_value_get_int(struct EARL_value *value);

#endif // EARL_VALUE_H
