/*
 * Kotha REPL Implementation
 */

#include "repl.h"
#include "vm.h"
#include "ir.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.2.0"

/* External parser functions */
extern int yyparse();
extern FILE *yyin;
extern ASTNode *root;
extern IRInstr *ir_head;
extern void ir_init();
extern void ir_generate(ASTNode *node);
extern VM* codegen_vm(IRInstr *ir);

/* Print REPL help */
static void print_repl_help() {
    printf("\n🐯 Kotha REPL Commands:\n");
    printf("  :help, :h       Show this help message\n");
    printf("  :quit, :q       Exit REPL\n");
    printf("  :clear, :c      Clear screen\n");
    printf("  :vars, :v       Show all variables\n");
    printf("  :reset, :r      Reset VM state\n");
    printf("\nEnter Kotha code to execute it immediately.\n\n");
}

/* Handle REPL commands */
int repl_handle_command(const char *cmd, VM *vm) {
    // Remove newline
    char command[256];
    strncpy(command, cmd, sizeof(command) - 1);
    command[strcspn(command, "\n")] = 0;
    
    if (strcmp(command, ":help") == 0 || strcmp(command, ":h") == 0) {
        print_repl_help();
        return 0;
    }
    
    if (strcmp(command, ":quit") == 0 || strcmp(command, ":q") == 0) {
        printf("Goodbye! 👋\n");
        return 1; // Exit REPL
    }
    
    if (strcmp(command, ":clear") == 0 || strcmp(command, ":c") == 0) {
        system("clear");
        printf("🐯 Kotha REPL v%s\n", VERSION);
        printf("Type :help for commands, :quit to exit\n\n");
        return 0;
    }
    
    if (strcmp(command, ":vars") == 0 || strcmp(command, ":v") == 0) {
        printf("\nGlobal Variables:\n");
        if (vm->global_count == 0) {
            printf("  (none)\n");
        } else {
            for (int i = 0; i < vm->global_count; i++) {
                printf("  var[%d] = ", i);
                if (vm->globals[i].type == VAL_INT) {
                    printf("%d\n", vm->globals[i].as.int_val);
                } else if (vm->globals[i].type == VAL_FLOAT) {
                    printf("%.2f\n", vm->globals[i].as.float_val);
                } else {
                    printf("(other)\n");
                }
            }
        }
        printf("\n");
        return 0;
    }
    
    if (strcmp(command, ":reset") == 0 || strcmp(command, ":r") == 0) {
        vm_free(vm);
        vm_init(vm);
        printf("VM state reset.\n\n");
        return 0;
    }
    
    printf("Unknown command: %s\n", command);
    printf("Type :help for available commands.\n\n");
    return 0;
}

/* Execute a single line */
int repl_execute_line(const char *line, VM *vm) {
    // Create temporary file with the line
    FILE *temp = fopen("/tmp/kotha_repl_temp.kotha", "w");
    if (!temp) {
        fprintf(stderr, "Error: Cannot create temporary file\n");
        return -1;
    }
    
    fprintf(temp, "%s\n", line);
    fclose(temp);
    
    // Parse the line
    FILE *input = fopen("/tmp/kotha_repl_temp.kotha", "r");
    if (!input) {
        fprintf(stderr, "Error: Cannot open temporary file\n");
        return -1;
    }
    
    extern FILE *yyin;
    yyin = input;
    
    // Reset parser state
    root = NULL;
    ir_head = NULL;
    
    int parse_result = yyparse();
    fclose(input);
    
    if (parse_result != 0) {
        printf("Syntax error\n");
        return -1;
    }
    
    if (!root) {
        return 0; // Empty line
    }
    
    // Generate IR
    ir_init();
    ir_generate(root);
    
    if (!ir_head) {
        return 0;
    }
    
    // Generate bytecode and execute
    VM *temp_vm = codegen_vm(ir_head);
    if (!temp_vm) {
        fprintf(stderr, "Error: Bytecode generation failed\n");
        return -1;
    }
    
    // Copy globals from persistent VM
    memcpy(temp_vm->globals, vm->globals, sizeof(vm->globals));
    temp_vm->global_count = vm->global_count;
    
    // Execute
    vm_run(temp_vm);
    
    // Copy globals back to persistent VM
    memcpy(vm->globals, temp_vm->globals, sizeof(temp_vm->globals));
    vm->global_count = temp_vm->global_count;
    
    vm_free(temp_vm);
    free(temp_vm);
    
    // Cleanup
    remove("/tmp/kotha_repl_temp.kotha");
    
    return 0;
}

/* Start REPL */
int repl_start(REPLConfig *config) {
    printf("🐯 Kotha REPL v%s\n", VERSION);
    printf("Type :help for commands, :quit to exit\n\n");
    
    // Initialize VM
    VM vm;
    vm_init(&vm);
    vm.debug_mode = config->debug;
    
    char line[1024];
    int line_num = 1;
    
    while (1) {
        printf("kotha[%d]> ", line_num);
        fflush(stdout);
        
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        
        // Skip empty lines
        if (line[0] == '\n') {
            continue;
        }
        
        // Handle REPL commands
        if (line[0] == ':') {
            if (repl_handle_command(line, &vm)) {
                break; // Exit requested
            }
            continue;
        }
        
        // Execute line
        repl_execute_line(line, &vm);
        line_num++;
    }
    
    vm_free(&vm);
    return 0;
}
