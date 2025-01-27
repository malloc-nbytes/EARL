#ifndef EVM_ROUTINES_H
#define EVM_ROUTINES_H

#include "opcode.h"
#include "EVM.h"

enum opcode EVM_routines_read_byte(struct EARL_vm *vm);

void EVM_routines_init(struct EARL_vm *vm);

// Stack routines
void EVM_routines_stack_push(struct EARL_vm *vm, struct EARL_value *value);
struct EARL_value *EVM_routines_stack_pop(struct EARL_vm *vm);

#endif // EVM_ROUTINES_H
