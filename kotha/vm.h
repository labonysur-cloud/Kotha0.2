/*
 * Kotha Virtual Machine (VM) Header - Professional Edition
 * Stack-based VM with function calls, heap, and GC
 */

#ifndef VM_H
#define VM_H

#include <stdint.h>

/* Configuration */
#define MAX_STACK 2048
#define MAX_CODE 8192
#define MAX_FRAMES 256
#define MAX_HEAP 65536
#define MAX_STRINGS 1024
#define MAX_CONSTANTS 1024
#define MAX_FUNCTIONS 256

/* Opcodes */
typedef enum {
    // Control flow
    OP_HALT,
    OP_NOP,
    
    // Stack operations
    OP_PUSH,        // Push constant
    OP_POP,         // Pop and discard
    OP_DUP,         // Duplicate top of stack
    
    // Arithmetic
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_NEG,         // Negate
    
    // Comparisons
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,
    
    // Logical
    OP_AND,
    OP_OR,
    OP_NOT,
    
    // Variables (local)
    OP_LOAD_LOCAL,  // Load local variable
    OP_STORE_LOCAL, // Store local variable
    OP_LOAD_GLOBAL, // Load global variable
    OP_STORE_GLOBAL,// Store global variable
    
    // Constants
    OP_LOAD_CONST,  // Load from constant pool
    
    // Jumps
    OP_JMP,
    OP_JMP_FALSE,
    OP_JMP_TRUE,
    
    // Functions
    OP_CALL,        // Call function
    OP_RETURN,      // Return from function
    OP_ENTER,       // Enter function (setup frame)
    OP_LEAVE,       // Leave function (cleanup frame)
    
    // Heap & Strings
    OP_ALLOC,       // Allocate heap memory
    OP_FREE,        // Free heap memory
    OP_LOAD_HEAP,   // Load from heap
    OP_STORE_HEAP,  // Store to heap
    OP_LOAD_STR,    // Load string from pool
    OP_CONCAT,      // Concatenate strings
    
    // I/O
    OP_PRINT,
    OP_PRINT_STR,
    OP_INPUT,
    
    // Exception handling
    OP_TRY,
    OP_THROW,
    OP_ENDTRY,
    
    // Debugging
    OP_LINE,        // Set current line number
    OP_BREAKPOINT   // Debugger breakpoint
} OpCode;

/* Value types */
typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING,
    VAL_HEAP_PTR,
    VAL_NULL
} ValueType;

/* Runtime value */
typedef struct {
    ValueType type;
    union {
        int int_val;
        float float_val;
        int string_id;   // Index into string pool
        int heap_ptr;    // Heap pointer
    } as;
} Value;

/* Instruction with optional argument */
typedef struct {
    OpCode code;
    int arg;
    int line;  // Source line number for debugging
} Instruction;

/* Call frame (activation record) */
typedef struct {
    int return_addr;     // Return address
    int frame_pointer;   // Frame pointer (base of local vars)
    int num_locals;      // Number of local variables
    int function_id;     // Function identifier
} CallFrame;

/* Heap object - Mark & Sweep GC */
typedef struct HeapObject {
    int size;
    int marked;          // Mark bit for GC
    struct HeapObject *next;  // Next object in allocation list
    char data[];         // Flexible array member
} HeapObject;

/* String pool entry */
typedef struct {
    char *str;
    int length;
    int marked;  // Mark bit for GC
} StringEntry;

/* Constant pool entry */
typedef struct {
    Value value;
} ConstantEntry;

/* Function table entry */
typedef struct {
    char *name;
    int address;
    int num_params;
} FunctionEntry;

/* Virtual Machine */
typedef struct {
    // Code
    Instruction code[MAX_CODE];
    int code_size;
    
    // Stack
    Value stack[MAX_STACK];
    int sp;  // Stack pointer
    int ip;  // Instruction pointer
    
    // Call frames
    CallFrame frames[MAX_FRAMES];
    int frame_count;
    int fp;  // Frame pointer
    
    // Global variables
    Value globals[MAX_STACK];
    int global_count;
    
    // Constant pool
    ConstantEntry constants[MAX_CONSTANTS];
    int constant_count;
    
    // Function table
    FunctionEntry functions[MAX_FUNCTIONS];
    int function_count;
    
    // String pool
    StringEntry strings[MAX_STRINGS];
    int string_count;
    
    // Heap - Mark & Sweep
    uint8_t heap[MAX_HEAP];
    int heap_used;
    HeapObject *first_object;  // Head of allocated objects list
    int bytes_allocated;       // Total bytes allocated
    int gc_threshold;          // GC trigger threshold
    
    // Exception handling
    int handler_stack[MAX_STACK];
    int hp;  // Handler stack pointer
    
    // Debugging
    int current_line;
    int debug_mode;
    
    // Statistics
    int instruction_count;
    int gc_count;
} VM;

/* VM Functions */
void vm_init(VM *vm);
void vm_free(VM *vm);
void vm_add_instr(VM *vm, OpCode op, int arg);
void vm_add_instr_line(VM *vm, OpCode op, int arg, int line);
void vm_run(VM *vm);
int vm_execute_instruction(VM *vm);

/* Stack operations */
void vm_push(VM *vm, Value val);
Value vm_pop(VM *vm);
Value vm_peek(VM *vm, int distance);

/* Constant pool */
int vm_add_constant(VM *vm, Value val);
Value vm_get_constant(VM *vm, int index);

/* String pool */
int vm_add_string(VM *vm, const char *str);
const char* vm_get_string(VM *vm, int id);
void vm_free_string(VM *vm, int id);

/* Function table */
int vm_add_function(VM *vm, const char *name, int address, int num_params);
int vm_get_function(VM *vm, const char *name);

/* Heap management */
int vm_alloc_heap(VM *vm, int size);
void vm_free_heap(VM *vm, int ptr);
void* vm_get_heap_ptr(VM *vm, int ptr);

/* Garbage collection */
void vm_gc_mark(VM *vm);
void vm_gc_sweep(VM *vm);
void vm_gc_collect(VM *vm);

/* Function calls */
void vm_call_function(VM *vm, int function_addr, int num_args);
void vm_return_function(VM *vm);

/* Debug helpers */
void vm_print_state(VM *vm);
void vm_disassemble(VM *vm);
void vm_print_stack_trace(VM *vm);
const char* vm_opcode_name(OpCode op);

/* Error handling */
void vm_error(VM *vm, const char *format, ...);
void vm_runtime_error(VM *vm, const char *format, ...);


/* Code Generation */
struct IRInstr;
VM *codegen_vm(struct IRInstr *ir);

#endif /* VM_H */
