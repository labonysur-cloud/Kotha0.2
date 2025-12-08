/*
 * Kotha Bytecode Generator
 * Converts IR (Intermediate Representation) to VM bytecode
 */

#include "vm.h"
#include "ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LABELS 256
#define MAX_VARS 256

/* Label mapping structure */
typedef struct {
    char *name;
    int address;
} LabelEntry;

/* Variable mapping structure */
typedef struct {
    char *name;
    int index;
} VarEntry;

/* Code generation state */
static LabelEntry labels[MAX_LABELS];
static int label_count = 0;

static VarEntry vars[MAX_VARS];
static int var_count = 0;

/* Function tracking */
static int param_count = 0;  // Track parameters for current call

/* Initialize code generation state */
void codegen_vm_init() {
    label_count = 0;
    var_count = 0;
    param_count = 0;
    memset(labels, 0, sizeof(labels));
    memset(vars, 0, sizeof(vars));
}

/* Get or create variable index */
static int get_var_index(const char *name) {
    if (!name) return -1;
    
    // Check if variable already exists
    for (int i = 0; i < var_count; i++) {
        if (vars[i].name && strcmp(vars[i].name, name) == 0) {
            return vars[i].index;
        }
    }
    
    // Create new variable
    if (var_count >= MAX_VARS) {
        fprintf(stderr, "Codegen Error: Too many variables\n");
        return -1;
    }
    
    vars[var_count].name = strdup(name);
    vars[var_count].index = var_count;
    return vars[var_count++].index;
}

/* Get label address (returns -1 if not found) */
static int get_label_address(const char *name) {
    if (!name) return -1;
    
    for (int i = 0; i < label_count; i++) {
        if (labels[i].name && strcmp(labels[i].name, name) == 0) {
            return labels[i].address;
        }
    }
    return -1;
}

/* Add label */
static void add_label(const char *name, int address) {
    if (!name || label_count >= MAX_LABELS) return;
    
    labels[label_count].name = strdup(name);
    labels[label_count].address = address;
    label_count++;
}

/* Helper to check if string is a number literal */
static int is_number(const char *s) {
    if (!s) return 0;
    
    // Skip optional sign
    if (*s == '-' || *s == '+') s++;
    
    if (!*s) return 0;
    
    int has_digit = 0;
    int has_dot = 0;
    
    while (*s) {
        if (isdigit(*s)) {
            has_digit = 1;
        } else if (*s == '.') {
            if (has_dot) return 0; // Multiple dots
            has_dot = 1;
        } else {
            return 0; // Non-digit char
        }
        s++;
    }
    
    return has_digit;
}

/* Helper to check if string is a float literal */
static int is_float(const char *s) {
    if (!is_number(s)) return 0;
    if (strchr(s, '.')) return 1;
    return 0;
}

/* Helper to load an operand (literal or variable) */
static void emit_load(VM *vm, const char *arg) {
    if (!arg) {
        // Load NULL/0 if needed, or assume implicit
        vm_add_instr(vm, OP_PUSH, 0); // Default to 0/NULL
        return;
    }
    
    if (arg[0] == '"') {
        // String literal
        char *str_content = strdup(arg + 1);
        if (str_content[strlen(str_content)-1] == '"') {
            str_content[strlen(str_content)-1] = '\0'; // Remove trailing quote
        }
        int id = vm_add_string(vm, str_content);
        vm_add_instr(vm, OP_LOAD_STR, id);
        free(str_content);
    } else if (is_number(arg)) {
        // Number literal
        if (is_float(arg)) {
             // Float - add to constant pool
             Value val;
             val.type = VAL_FLOAT;
             val.as.float_val = atof(arg);
             int id = vm_add_constant(vm, val);
             vm_add_instr(vm, OP_LOAD_CONST, id);
        } else {
             // Integer - push directly if small or const pool
             int val = atoi(arg);
             vm_add_instr(vm, OP_PUSH, val);
        }
    } else {
        // Variable
        int idx = get_var_index(arg);
        vm_add_instr(vm, OP_LOAD_LOCAL, idx);
    }
}

/* Generate VM bytecode from IR */
VM* codegen_vm(IRInstr *ir) {
    if (!ir) return NULL;
    
    // Allocate and initialize VM
    VM *vm = (VM*)malloc(sizeof(VM));
    if (!vm) {
        fprintf(stderr, "Codegen Error: Failed to allocate VM\n");
        return NULL;
    }
    vm_init(vm);
    
    // Initialize code generation
    codegen_vm_init();
    
    // First pass: collect labels and estimate size
    IRInstr *curr = ir;
    int addr = 0;
    while (curr) {
        if (curr->op == IR_LABEL) {
            add_label(curr->result, addr);
            
            // Check if this is a function label (starts with "func_" or "function_")
            if (curr->result && (strncmp(curr->result, "func_", 5) == 0 || 
                                  strncmp(curr->result, "function_", 9) == 0)) {
                // Register function in VM function table
                // Extract function name (skip "func_" or "function_" prefix)
                const char *func_name = curr->result;
                int func_idx = vm_get_function(vm, func_name);
                if (func_idx >= 0) {
                    // Update existing function with address
                    vm->functions[func_idx].address = addr;
                } else {
                    // Add new function (params will be set during IR_CALL)
                    vm_add_function(vm, func_name, addr, 0);
                }
            }
        } else {
            // Count instructions (estimate is fine for label addresses as long as we maintain consistency)
            // But VM uses instruction index, not byte offset. 
            // So we must count exactly how many instructions we will emit.
            // This requires duplicating the logic of emission or using a more robust 2-pass system.
            // For now, let's keep the simple mapping:
            
            switch (curr->op) {
                case IR_ADD:
                case IR_SUB:
                case IR_MUL:
                case IR_DIV:
                case IR_MOD:
                case IR_EQ:
                case IR_NEQ:
                case IR_LT:
                case IR_GT:
                case IR_LTE:
                case IR_GTE:
                    addr += 4; // LOAD arg1, LOAD arg2, OP, STORE result
                    break;
                case IR_ASSIGN:
                    addr += 2; // LOAD arg1, STORE result
                    break;
                case IR_PRINT:
                    addr += 2; // LOAD arg1, PRINT
                    break;
                case IR_GOTO:
                    addr += 1; // JMP
                    break;
                case IR_IF_FALSE:
                    addr += 2; // LOAD arg1, JMP_FALSE
                    break;
                case IR_RETURN:
                    addr += 2; // LOAD return_val (if any), RETURN
                    break;
                case IR_PARAM:
                    addr += 1; // LOAD param
                    break;
                case IR_CALL:
                    addr += 3; // CALL, STORE/POP result
                    break;
                case IR_TRY_START:
                    addr += 1; 
                    break;
                case IR_TRY_END:
                    addr += 1;
                    break;
                case IR_THROW:
                    addr += 1;
                    break;
                default:
                    addr += 1; // NOP or others
                    break;
            }
        }
        curr = curr->next;
    }
    
    // Second pass: generate bytecode
    curr = ir;
    while (curr) {
        switch (curr->op) {
            case IR_NOP:
                // No operation
                break;
                
            case IR_ASSIGN: {
                // result = arg1
                emit_load(vm, curr->arg1);
                
                int dst_idx = get_var_index(curr->result);
                vm_add_instr(vm, OP_STORE_LOCAL, dst_idx); 
                break;
            }
            
            case IR_ADD:
            case IR_SUB:
            case IR_MUL:
            case IR_DIV:
            case IR_MOD: {
                // result = arg1 OP arg2
                emit_load(vm, curr->arg1);
                emit_load(vm, curr->arg2);
                
                int result_idx = get_var_index(curr->result);
                
                switch (curr->op) {
                    case IR_ADD: vm_add_instr(vm, OP_ADD, 0); break;
                    case IR_SUB: vm_add_instr(vm, OP_SUB, 0); break;
                    case IR_MUL: vm_add_instr(vm, OP_MUL, 0); break;
                    case IR_DIV: vm_add_instr(vm, OP_DIV, 0); break;
                    case IR_MOD: vm_add_instr(vm, OP_MOD, 0); break;
                    default: break;
                }
                
                vm_add_instr(vm, OP_STORE_LOCAL, result_idx);
                break;
            }
            
            case IR_EQ:
            case IR_NEQ:
            case IR_LT:
            case IR_GT:
            case IR_LTE:
            case IR_GTE: {
                // result = arg1 CMP arg2
                emit_load(vm, curr->arg1);
                emit_load(vm, curr->arg2);
                
                int result_idx = get_var_index(curr->result);
                
                switch (curr->op) {
                    case IR_EQ:  vm_add_instr(vm, OP_EQ, 0); break;
                    case IR_LT:  vm_add_instr(vm, OP_LT, 0); break;
                    case IR_GT:  vm_add_instr(vm, OP_GT, 0); break;
                    case IR_NEQ: 
                        vm_add_instr(vm, OP_EQ, 0);
                        vm_add_instr(vm, OP_PUSH, 0);
                        vm_add_instr(vm, OP_EQ, 0); // NOT
                        break;
                    case IR_LTE:
                        vm_add_instr(vm, OP_GT, 0);
                        vm_add_instr(vm, OP_PUSH, 0);
                        vm_add_instr(vm, OP_EQ, 0); // NOT
                        break;
                    case IR_GTE:
                        vm_add_instr(vm, OP_LT, 0);
                        vm_add_instr(vm, OP_PUSH, 0);
                        vm_add_instr(vm, OP_EQ, 0); // NOT
                        break;
                    default: break;
                }
                
                vm_add_instr(vm, OP_STORE_LOCAL, result_idx);
                break;
            }
            
            case IR_PRINT: {
                // print arg1
                emit_load(vm, curr->arg1);
                vm_add_instr(vm, OP_PRINT, 0);
                break;
            }
            
            case IR_LABEL:
                // Labels are already processed in first pass
                break;
            
            case IR_GOTO: {
                // goto result
                int addr = get_label_address(curr->result);
                if (addr >= 0) {
                    vm_add_instr(vm, OP_JMP, addr);
                } else {
                    fprintf(stderr, "Codegen Error: Undefined label '%s'\n", curr->result);
                }
                break;
            }
            
            case IR_IF_FALSE: {
                // if false arg1 goto result
                emit_load(vm, curr->arg1);
                int addr = get_label_address(curr->result);
                
                if (addr >= 0) {
                    vm_add_instr(vm, OP_JMP_FALSE, addr);
                } else {
                    fprintf(stderr, "Codegen Error: Undefined label '%s'\n", curr->result);
                }
                break;
            }
            
            case IR_RETURN:
                // Push return value if present
                if (curr->arg1) {
                    emit_load(vm, curr->arg1);
                }
                vm_add_instr(vm, OP_RETURN, 0);
                break;
            
            case IR_TRY_START: {
                // try start (arg1 = catch label)
                int addr = get_label_address(curr->arg1);
                if (addr >= 0) {
                    vm_add_instr(vm, OP_TRY, addr);
                }
                break;
            }
            
            case IR_TRY_END:
                vm_add_instr(vm, OP_ENDTRY, 0);
                break;
            
            case IR_THROW:
                vm_add_instr(vm, OP_THROW, 0);
                break;
            
            case IR_PARAM:
                // Push parameter onto stack for upcoming call
                emit_load(vm, curr->arg1);
                param_count++;
                break;
            
            case IR_CALL: {
                // arg1 = function name, arg2 = number of args (optional)
                if (!curr->arg1) {
                    fprintf(stderr, "Codegen Error: IR_CALL missing function name\n");
                    break;
                }
                
                // Get or create function index
                int func_idx = vm_get_function(vm, curr->arg1);
                if (func_idx < 0) {
                    // Function not yet registered, add placeholder
                    // Address will be updated when function label is found
                    func_idx = vm_add_function(vm, curr->arg1, -1, param_count);
                }
                
                if (func_idx >= 0) {
                    // Emit call instruction
                    vm_add_instr(vm, OP_CALL, func_idx);
                    
                    // Store return value if result is specified
                    if (curr->result) {
                        int dst_idx = get_var_index(curr->result);
                        vm_add_instr(vm, OP_STORE_LOCAL, dst_idx);
                    } else {
                        // Pop unused return value
                        vm_add_instr(vm, OP_POP, 0);
                    }
                }
                
                param_count = 0;  // Reset for next call
                break;
            }
            
            default:
                fprintf(stderr, "Codegen Warning: Unknown IR opcode %d\n", curr->op);
                break;
        }
        
        curr = curr->next;
    }
    
    // Add final HALT if not already present
    if (vm->code_size == 0 || vm->code[vm->code_size - 1].code != OP_HALT) {
        vm_add_instr(vm, OP_HALT, 0);
    }
    
    return vm;
}

/* Cleanup function */
void codegen_vm_cleanup() {
    for (int i = 0; i < label_count; i++) {
        if (labels[i].name) {
            free(labels[i].name);
            labels[i].name = NULL;
        }
    }
    
    for (int i = 0; i < var_count; i++) {
        if (vars[i].name) {
            free(vars[i].name);
            vars[i].name = NULL;
        }
    }
    
    label_count = 0;
    var_count = 0;
}
