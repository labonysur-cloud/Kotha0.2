/*
 * Kotha Virtual Machine - Professional Edition
 * Complete implementation with functions, heap, GC, and debugging
 */

#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Helper macros */
#define STACK_MAX (vm->sp + 1)
#define IS_EMPTY(vm) (vm->sp < 0)
#define BINARY_OP(op) do { \
    Value b = vm_pop(vm); \
    Value a = vm_pop(vm); \
    Value result = {VAL_INT, {.int_val = 0}}; \
    if (a.type == VAL_INT && b.type == VAL_INT) { \
        result.as.int_val = a.as.int_val op b.as.int_val; \
    } else if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) { \
        float fa = (a.type == VAL_FLOAT) ? a.as.float_val : (float)a.as.int_val; \
        float fb = (b.type == VAL_FLOAT) ? b.as.float_val : (float)b.as.int_val; \
        result.type = VAL_FLOAT; \
        result.as.float_val = fa op fb; \
    } \
    vm_push(vm, result); \
} while(0)

/* Initialize VM */
void vm_init(VM *vm) {
    if (!vm) return;
    
    memset(vm, 0, sizeof(VM));
    vm->sp = -1;
    vm->ip = 0;
    vm->fp = 0;
    vm->hp = -1;
    vm->frame_count = 0;
    vm->global_count = 0;
    vm->constant_count = 0;
    vm->function_count = 0;
    vm->string_count = 0;
    vm->heap_used = 0;
    vm->first_object = NULL;
    vm->bytes_allocated = 0;
    vm->gc_threshold = 1024 * 1024;  // 1MB initial threshold
    vm->current_line = 0;
    vm->debug_mode = 0;
    vm->instruction_count = 0;
    vm->gc_count = 0;
}

/* Free VM resources */
void vm_free(VM *vm) {
    if (!vm) return;
    
    // Free all strings in pool
    for (int i = 0; i < vm->string_count; i++) {
        if (vm->strings[i].str) {
            free(vm->strings[i].str);
            vm->strings[i].str = NULL;
        }
    }
    
    // Free all function names
    for (int i = 0; i < vm->function_count; i++) {
        if (vm->functions[i].name) {
            free(vm->functions[i].name);
            vm->functions[i].name = NULL;
        }
    }
}

/* Add instruction */
void vm_add_instr(VM *vm, OpCode op, int arg) {
    vm_add_instr_line(vm, op, arg, 0);
}

void vm_add_instr_line(VM *vm, OpCode op, int arg, int line) {
    if (!vm || vm->code_size >= MAX_CODE) {
        fprintf(stderr, "VM Error: Code size exceeded\n");
        return;
    }
    
    vm->code[vm->code_size].code = op;
    vm->code[vm->code_size].arg = arg;
    vm->code[vm->code_size].line = line;
    vm->code_size++;
}

/* Stack operations */
void vm_push(VM *vm, Value val) {
    if (vm->sp >= MAX_STACK - 1) {
        vm_runtime_error(vm, "Stack overflow");
        return;
    }
    vm->stack[++vm->sp] = val;
}

Value vm_pop(VM *vm) {
    if (vm->sp < 0) {
        vm_runtime_error(vm, "Stack underflow");
        Value null_val = {VAL_NULL, {.int_val = 0}};
        return null_val;
    }
    return vm->stack[vm->sp--];
}

Value vm_peek(VM *vm, int distance) {
    if (vm->sp - distance < 0) {
        Value null_val = {VAL_NULL, {.int_val = 0}};
        return null_val;
    }
    return vm->stack[vm->sp - distance];
}

/* Constant pool */
int vm_add_constant(VM *vm, Value val) {
    if (vm->constant_count >= MAX_CONSTANTS) {
        fprintf(stderr, "VM Error: Constant pool full\n");
        return -1;
    }
    
    vm->constants[vm->constant_count].value = val;
    return vm->constant_count++;
}

Value vm_get_constant(VM *vm, int index) {
    if (index < 0 || index >= vm->constant_count) {
        Value null_val = {VAL_NULL, {.int_val = 0}};
        return null_val;
    }
    return vm->constants[index].value;
}

/* String pool */
int vm_add_string(VM *vm, const char *str) {
    if (!str || vm->string_count >= MAX_STRINGS) {
        return -1;
    }
    
    // Check if string already exists (deduplication)
    for (int i = 0; i < vm->string_count; i++) {
        if (vm->strings[i].str && strcmp(vm->strings[i].str, str) == 0) {
            return i;
        }
    }
    
    // Add new string
    vm->strings[vm->string_count].str = strdup(str);
    vm->strings[vm->string_count].length = strlen(str);
    vm->strings[vm->string_count].marked = 0;
    return vm->string_count++;
}

const char* vm_get_string(VM *vm, int id) {
    if (id < 0 || id >= vm->string_count) {
        return "";
    }
    return vm->strings[id].str;
}

void vm_free_string(VM *vm, int id) {
    if (id < 0 || id >= vm->string_count) return;
    
    // Strings are now managed by mark & sweep GC
    // This function is kept for API compatibility but does nothing
}

/* Function table */
int vm_add_function(VM *vm, const char *name, int address, int num_params) {
    if (!vm || !name || vm->function_count >= MAX_FUNCTIONS) {
        return -1;
    }
    
    // Check if function already exists
    for (int i = 0; i < vm->function_count; i++) {
        if (vm->functions[i].name && strcmp(vm->functions[i].name, name) == 0) {
            // Update existing function
            vm->functions[i].address = address;
            vm->functions[i].num_params = num_params;
            return i;
        }
    }
    
    // Add new function
    vm->functions[vm->function_count].name = strdup(name);
    vm->functions[vm->function_count].address = address;
    vm->functions[vm->function_count].num_params = num_params;
    return vm->function_count++;
}

int vm_get_function(VM *vm, const char *name) {
    if (!vm || !name) return -1;
    
    for (int i = 0; i < vm->function_count; i++) {
        if (vm->functions[i].name && strcmp(vm->functions[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Heap management - Mark & Sweep */
int vm_alloc_heap(VM *vm, int size) {
    // Trigger GC if we exceed threshold
    if (vm->bytes_allocated > vm->gc_threshold) {
        vm_gc_collect(vm);
        
        // Increase threshold for next time
        if (vm->bytes_allocated > vm->gc_threshold) {
            vm->gc_threshold = vm->bytes_allocated * 2;
        }
    }
    
    // Check if we have space
    int total_size = sizeof(HeapObject) + size;
    if (vm->heap_used + total_size > MAX_HEAP) {
        vm_gc_collect(vm);
        
        if (vm->heap_used + total_size > MAX_HEAP) {
            vm_runtime_error(vm, "Out of heap memory");
            return -1;
        }
    }
    
    // Allocate object
    int ptr = vm->heap_used;
    HeapObject *obj = (HeapObject*)&vm->heap[ptr];
    obj->size = size;
    obj->marked = 0;
    obj->next = vm->first_object;
    
    // Add to object list
    vm->first_object = obj;
    
    vm->heap_used += total_size;
    vm->bytes_allocated += total_size;
    
    return ptr;
}

void vm_free_heap(VM *vm, int ptr) {
    // Objects are now freed during sweep phase
    // This function is kept for API compatibility
}

void* vm_get_heap_ptr(VM *vm, int ptr) {
    if (ptr < 0 || ptr >= vm->heap_used) return NULL;
    
    HeapObject *obj = (HeapObject*)&vm->heap[ptr];
    return obj->data;
}

/* Garbage collection - Mark & Sweep Algorithm */

/* Mark phase: trace from roots and mark all reachable objects */
void vm_gc_mark(VM *vm) {
    if (!vm) return;
    
    // First, unmark all objects
    HeapObject *obj = vm->first_object;
    while (obj) {
        obj->marked = 0;
        obj = obj->next;
    }
    
    // Unmark all strings
    for (int i = 0; i < vm->string_count; i++) {
        vm->strings[i].marked = 0;
    }
    
    // Mark from roots: Stack
    for (int i = 0; i <= vm->sp; i++) {
        Value val = vm->stack[i];
        
        if (val.type == VAL_HEAP_PTR) {
            int ptr = val.as.heap_ptr;
            if (ptr >= 0 && ptr < vm->heap_used) {
                HeapObject *heap_obj = (HeapObject*)&vm->heap[ptr];
                heap_obj->marked = 1;
            }
        } else if (val.type == VAL_STRING) {
            int str_id = val.as.string_id;
            if (str_id >= 0 && str_id < vm->string_count) {
                vm->strings[str_id].marked = 1;
            }
        }
    }
    
    // Mark from roots: Globals
    for (int i = 0; i < vm->global_count; i++) {
        Value val = vm->globals[i];
        
        if (val.type == VAL_HEAP_PTR) {
            int ptr = val.as.heap_ptr;
            if (ptr >= 0 && ptr < vm->heap_used) {
                HeapObject *heap_obj = (HeapObject*)&vm->heap[ptr];
                heap_obj->marked = 1;
            }
        } else if (val.type == VAL_STRING) {
            int str_id = val.as.string_id;
            if (str_id >= 0 && str_id < vm->string_count) {
                vm->strings[str_id].marked = 1;
            }
        }
    }
    
    // Mark from roots: Call frames (local variables)
    for (int f = 0; f < vm->frame_count; f++) {
        CallFrame *frame = &vm->frames[f];
        int base = frame->frame_pointer;
        
        for (int i = 0; i < frame->num_locals && (base + i) <= vm->sp; i++) {
            Value val = vm->stack[base + i];
            
            if (val.type == VAL_HEAP_PTR) {
                int ptr = val.as.heap_ptr;
                if (ptr >= 0 && ptr < vm->heap_used) {
                    HeapObject *heap_obj = (HeapObject*)&vm->heap[ptr];
                    heap_obj->marked = 1;
                }
            } else if (val.type == VAL_STRING) {
                int str_id = val.as.string_id;
                if (str_id >= 0 && str_id < vm->string_count) {
                    vm->strings[str_id].marked = 1;
                }
            }
        }
    }
    
    // Mark from roots: Constants pool
    for (int i = 0; i < vm->constant_count; i++) {
        Value val = vm->constants[i].value;
        
        if (val.type == VAL_STRING) {
            int str_id = val.as.string_id;
            if (str_id >= 0 && str_id < vm->string_count) {
                vm->strings[str_id].marked = 1;
            }
        }
    }
}

/* Sweep phase: reclaim unmarked objects */
void vm_gc_sweep(VM *vm) {
    if (!vm) return;
    
    // Sweep heap objects
    HeapObject **obj_ptr = &vm->first_object;
    int bytes_freed = 0;
    
    while (*obj_ptr) {
        HeapObject *obj = *obj_ptr;
        
        if (!obj->marked) {
            // Remove from list
            *obj_ptr = obj->next;
            
            // Track freed bytes
            bytes_freed += sizeof(HeapObject) + obj->size;
        } else {
            // Keep object, move to next
            obj_ptr = &obj->next;
        }
    }
    
    // Compact heap by rebuilding it from marked objects
    int new_heap_used = 0;
    HeapObject *new_first = NULL;
    HeapObject **new_tail = &new_first;
    
    obj_ptr = &vm->first_object;
    while (*obj_ptr) {
        HeapObject *obj = *obj_ptr;
        int obj_size = sizeof(HeapObject) + obj->size;
        
        // Copy to new position
        if (new_heap_used != (uint8_t*)obj - vm->heap) {
            memmove(&vm->heap[new_heap_used], obj, obj_size);
        }
        
        // Update object pointer
        HeapObject *new_obj = (HeapObject*)&vm->heap[new_heap_used];
        *new_tail = new_obj;
        new_tail = &new_obj->next;
        
        new_heap_used += obj_size;
        obj_ptr = &obj->next;
    }
    
    *new_tail = NULL;
    vm->first_object = new_first;
    vm->heap_used = new_heap_used;
    vm->bytes_allocated -= bytes_freed;
    
    // Sweep strings
    for (int i = 0; i < vm->string_count; i++) {
        if (!vm->strings[i].marked && vm->strings[i].str) {
            free(vm->strings[i].str);
            vm->strings[i].str = NULL;
            vm->strings[i].length = 0;
        }
    }
}

/* Collect garbage */
void vm_gc_collect(VM *vm) {
    if (!vm) return;
    
    int before = vm->bytes_allocated;
    
    vm_gc_mark(vm);
    vm_gc_sweep(vm);
    vm->gc_count++;
    
    if (vm->debug_mode) {
        int freed = before - vm->bytes_allocated;
        printf("[GC #%d] Collected %d bytes (before: %d, after: %d)\n",
               vm->gc_count, freed, before, vm->bytes_allocated);
    }
}

/* Function calls */
void vm_call_function(VM *vm, int function_addr, int num_args) {
    if (vm->frame_count >= MAX_FRAMES) {
        vm_runtime_error(vm, "Stack overflow (too many function calls)");
        return;
    }
    
    CallFrame *frame = &vm->frames[vm->frame_count++];
    frame->return_addr = vm->ip;
    frame->frame_pointer = vm->sp - num_args + 1;
    frame->num_locals = num_args;
    frame->function_id = function_addr;
    
    vm->ip = function_addr;
    vm->fp = frame->frame_pointer;
}

void vm_return_function(VM *vm) {
    if (vm->frame_count == 0) {
        vm->ip = vm->code_size; // Halt
        return;
    }
    
    CallFrame *frame = &vm->frames[--vm->frame_count];
    
    // Pop local variables
    vm->sp = frame->frame_pointer - 1;
    
    // Restore IP
    vm->ip = frame->return_addr;
    
    // Restore FP
    if (vm->frame_count > 0) {
        vm->fp = vm->frames[vm->frame_count - 1].frame_pointer;
    } else {
        vm->fp = 0;
    }
}

/* Execute single instruction */
int vm_execute_instruction(VM *vm) {
    if (vm->ip >= vm->code_size) {
        return 0; // Halt
    }
    
    Instruction instr = vm->code[vm->ip++];
    vm->instruction_count++;
    
    if (instr.line > 0) {
        vm->current_line = instr.line;
    }
    
    switch (instr.code) {
        case OP_HALT:
            return 0;
        
        case OP_NOP:
            break;
        
        case OP_PUSH: {
            Value val = {VAL_INT, {.int_val = instr.arg}};
            vm_push(vm, val);
            break;
        }
        
        case OP_POP:
            vm_pop(vm);
            break;
        
        case OP_DUP: {
            Value val = vm_peek(vm, 0);
            vm_push(vm, val);
            break;
        }
        
        case OP_ADD:
            BINARY_OP(+);
            break;
        
        case OP_SUB:
            BINARY_OP(-);
            break;
        
        case OP_MUL:
            BINARY_OP(*);
            break;
        
        case OP_DIV: {
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if ((b.type == VAL_INT && b.as.int_val == 0) ||
                (b.type == VAL_FLOAT && b.as.float_val == 0.0f)) {
                vm_runtime_error(vm, "Division by zero");
                break;
            }
            Value result = {VAL_INT, {.int_val = 0}};
            if (a.type == VAL_INT && b.type == VAL_INT) {
                result.as.int_val = a.as.int_val / b.as.int_val;
            } else {
                float fa = (a.type == VAL_FLOAT) ? a.as.float_val : (float)a.as.int_val;
                float fb = (b.type == VAL_FLOAT) ? b.as.float_val : (float)b.as.int_val;
                result.type = VAL_FLOAT;
                result.as.float_val = fa / fb;
            }
            vm_push(vm, result);
            break;
        }
        
        case OP_MOD: {
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            if (a.type == VAL_INT && b.type == VAL_INT) {
                if (b.as.int_val == 0) {
                    vm_runtime_error(vm, "Modulo by zero");
                    break;
                }
                Value result = {VAL_INT, {.int_val = a.as.int_val % b.as.int_val}};
                vm_push(vm, result);
            }
            break;
        }
        
        case OP_NEG: {
            Value val = vm_pop(vm);
            if (val.type == VAL_INT) {
                val.as.int_val = -val.as.int_val;
            } else if (val.type == VAL_FLOAT) {
                val.as.float_val = -val.as.float_val;
            }
            vm_push(vm, val);
            break;
        }
        
        case OP_EQ: {
            Value b = vm_pop(vm);
            Value a = vm_pop(vm);
            int result = 0;
            if (a.type == VAL_INT && b.type == VAL_INT) {
                result = (a.as.int_val == b.as.int_val);
            }
            Value val = {VAL_INT, {.int_val = result}};
            vm_push(vm, val);
            break;
        }
        
        case OP_LT:
            BINARY_OP(<);
            break;
        
        case OP_GT:
            BINARY_OP(>);
            break;
        
        
        case OP_LOAD_LOCAL: {
            // When no frames, treat as stack-relative addressing
            int index = (vm->frame_count > 0) ? (vm->fp + instr.arg) : instr.arg;
            if (index >= 0 && index < MAX_STACK) {
                // If accessing beyond current stack, push default value
                if (index > vm->sp) {
                    Value zero = {VAL_INT, {.int_val = 0}};
                    vm_push(vm, zero);
                } else {
                    vm_push(vm, vm->stack[index]);
                }
            }
            break;
        }
        
        case OP_STORE_LOCAL: {
            // When no frames, treat as stack-relative addressing
            int index = (vm->frame_count > 0) ? (vm->fp + instr.arg) : instr.arg;
            if (index >= 0 && index < MAX_STACK) {
                Value val = vm_pop(vm);
                // Expand stack if needed
                while (vm->sp < index) {
                    Value zero = {VAL_INT, {.int_val = 0}};
                    vm_push(vm, zero);
                }
                vm->stack[index] = val;
            }
            break;
        }
        
        case OP_LOAD_GLOBAL: {
            if (instr.arg >= 0 && instr.arg < vm->global_count) {
                vm_push(vm, vm->globals[instr.arg]);
            }
            break;
        }
        
        case OP_STORE_GLOBAL: {
            if (instr.arg >= 0 && instr.arg < MAX_STACK) {
                vm->globals[instr.arg] = vm_pop(vm);
                if (instr.arg >= vm->global_count) {
                    vm->global_count = instr.arg + 1;
                }
            }
            break;
        }
        
        case OP_LOAD_CONST: {
            Value val = vm_get_constant(vm, instr.arg);
            vm_push(vm, val);
            break;
        }
        
        case OP_JMP:
            vm->ip = instr.arg;
            break;
        
        case OP_JMP_FALSE: {
            Value cond = vm_pop(vm);
            int is_false = (cond.type == VAL_INT && cond.as.int_val == 0);
            if (is_false) {
                vm->ip = instr.arg;
            }
            break;
        }
        
        case OP_CALL: {
            // arg contains function index
            if (instr.arg < 0 || instr.arg >= vm->function_count) {
                vm_runtime_error(vm, "Invalid function index: %d", instr.arg);
                break;
            }
            
            FunctionEntry *func = &vm->functions[instr.arg];
            vm_call_function(vm, func->address, func->num_params);
            break;
        }
        
        case OP_RETURN: {
            // Check if there's a return value on the stack
            Value return_val = {VAL_NULL, {.int_val = 0}};
            int has_return = 0;
            
            // If stack has values beyond frame pointer, assume return value
            if (vm->sp >= vm->fp) {
                return_val = vm_pop(vm);
                has_return = 1;
            }
            
            vm_return_function(vm);
            
            // Push return value back for caller
            if (has_return) {
                vm_push(vm, return_val);
            }
            break;
        }
        
        case OP_PRINT: {
            Value val = vm_pop(vm);
            if (val.type == VAL_INT) {
                printf("%d\n", val.as.int_val);
            } else if (val.type == VAL_FLOAT) {
                printf("%f\n", val.as.float_val);
            }
            break;
        }
        
        case OP_PRINT_STR: {
            Value val = vm_pop(vm);
            if (val.type == VAL_STRING) {
                printf("%s\n", vm_get_string(vm, val.as.string_id));
            }
            break;
        }
        
        case OP_INPUT: {
            // Read integer input from user
            int input_val;
            if (scanf("%d", &input_val) == 1) {
                Value val = {VAL_INT, {.int_val = input_val}};
                vm_push(vm, val);
            } else {
                // Clear input buffer on error
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                Value val = {VAL_INT, {.int_val = 0}};
                vm_push(vm, val);
            }
            break;
        }
        
        case OP_LOAD_STR: {
            Value val = {VAL_STRING, {.string_id = instr.arg}};
            vm_push(vm, val);
            break;
        }
        
        case OP_LINE:
            vm->current_line = instr.arg;
            break;
        
        default:
            vm_runtime_error(vm, "Unknown opcode: %d", instr.code);
            return 0;
    }
    
    return 1; // Continue execution
}

/* Main execution loop */
void vm_run(VM *vm) {
    if (!vm) return;
    
    while (vm->ip < vm->code_size) {
        if (!vm_execute_instruction(vm)) {
            break;
        }
    }
}

/* Error handling */
void vm_error(VM *vm, const char *format, ...) {
    fprintf(stderr, "VM Error: ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void vm_runtime_error(VM *vm, const char *format, ...) {
    fprintf(stderr, "\n🐯 Kotha Runtime Error\n");
    fprintf(stderr, "━━━━━━━━━━━━━━━━━━━━━━\n");
    
    if (vm->current_line > 0) {
        fprintf(stderr, "Line %d: ", vm->current_line);
    }
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n\n");
    
    vm_print_stack_trace(vm);
}

/* Debug utilities */
const char* vm_opcode_name(OpCode op) {
    switch (op) {
        case OP_HALT: return "HALT";
        case OP_NOP: return "NOP";
        case OP_PUSH: return "PUSH";
        case OP_POP: return "POP";
        case OP_DUP: return "DUP";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_NEG: return "NEG";
        case OP_EQ: return "EQ";
        case OP_LT: return "LT";
        case OP_GT: return "GT";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_LOAD_CONST: return "LOAD_CONST";
        case OP_JMP: return "JMP";
        case OP_JMP_FALSE: return "JMP_FALSE";
        case OP_CALL: return "CALL";
        case OP_RETURN: return "RETURN";
        case OP_PRINT: return "PRINT";
        case OP_PRINT_STR: return "PRINT_STR";
        case OP_INPUT: return "INPUT";
        case OP_LOAD_STR: return "LOAD_STR";
        case OP_LINE: return "LINE";
        default: return "UNKNOWN";
    }
}

void vm_print_state(VM *vm) {
    printf("=== VM State ===\n");
    printf("IP: %d, SP: %d, FP: %d\n", vm->ip, vm->sp, vm->fp);
    printf("Frames: %d, Line: %d\n", vm->frame_count, vm->current_line);
    printf("Instructions: %d, GC runs: %d\n", vm->instruction_count, vm->gc_count);
    printf("Stack: [");
    for (int i = 0; i <= vm->sp && i < 10; i++) {
        if (vm->stack[i].type == VAL_INT) {
            printf("%d", vm->stack[i].as.int_val);
        } else if (vm->stack[i].type == VAL_FLOAT) {
            printf("%.2f", vm->stack[i].as.float_val);
        }
        if (i < vm->sp && i < 9) printf(", ");
    }
    if (vm->sp >= 10) printf("...");
    printf("]\n================\n");
}

void vm_disassemble(VM *vm) {
    printf("=== Bytecode Disassembly ===\n");
    for (int i = 0; i < vm->code_size; i++) {
        Instruction instr = vm->code[i];
        printf("%04d: %-15s", i, vm_opcode_name(instr.code));
        if (instr.arg != 0 || instr.code == OP_PUSH) {
            printf(" %d", instr.arg);
        }
        if (instr.line > 0) {
            printf(" (line %d)", instr.line);
        }
        printf("\n");
    }
    printf("============================\n");
}

void vm_print_stack_trace(VM *vm) {
    if (vm->frame_count == 0) {
        fprintf(stderr, "Stack trace: <main>\n");
        return;
    }
    
    fprintf(stderr, "Stack trace:\n");
    for (int i = vm->frame_count - 1; i >= 0; i--) {
        CallFrame *frame = &vm->frames[i];
        fprintf(stderr, "  at function_%d (addr %d)\n", 
                frame->function_id, frame->return_addr);
    }
}
