#ifndef EVM_ROUTINES_H
#define EVM_ROUTINES_H

#include "opcode.h"
#include "EVM.h"

opcode_t EVM_routines_read_byte(EARL_vm_t *vm);
void     EVM_routines_init(EARL_vm_t *vm);

// Stack routines
void          EVM_routines_stack_push(EARL_vm_t *vm, EARL_value_t *value);
EARL_value_t *EVM_routines_stack_pop(EARL_vm_t *vm);

#endif // EVM_ROUTINES_H
