#ifndef EARL_VALUE_H
#define EARL_VALUE_H

enum earl_value_type {
        EARL_VALUE_TYPE_INTEGER,
};

struct earl_value {
        enum earl_value_type type;
        union {
                int integer;
        } value;
};

struct earl_value *
earl_value_alloc(enum earl_value_type type, void *data);

int
earl_value_get_int(struct earl_value *value);

#endif // EARL_VALUE_H
