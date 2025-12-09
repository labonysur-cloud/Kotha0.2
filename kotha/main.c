/*
 * Kotha Compiler - Main Driver
 * Supports multiple execution modes: compile to C, VM, interpret
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "ir.h"
#include "ast.h"
#include "interp.h"
#include "optimizer.h"

#define VERSION "0.2.0"

/* Command types */
typedef enum {
    CMD_BUILD,      // kotha build
    CMD_RUN,        // kotha run
    CMD_REPL,       // kotha repl
    CMD_HELP,       // kotha help
    CMD_VERSION,    // kotha version
    CMD_LEGACY      // Legacy flag-based mode
} CommandType;

/* Execution modes */
typedef enum {
    MODE_COMPILE_C,    // Default: Compile to C
    MODE_VM,           // Compile to bytecode and run in VM
    MODE_INTERPRET,    // Direct interpretation
    MODE_OPTIMIZE,     // Show optimizations
    MODE_BYTECODE,     // Show bytecode
    MODE_HELP,
    MODE_VERSION
} ExecutionMode;

/* Configuration */
typedef struct {
    CommandType command;
    ExecutionMode mode;
    const char *input_file;
    const char *output_file;
    int debug;
    int verbose;
    int vm_mode;
    int optimize_level;
} Config;

/* Forward declarations */
extern int yyparse();
extern FILE *yyin;
extern ASTNode *root;
extern IRInstr *ir_head;
extern int generate_c;

/* REPL */
#include "repl.h"

/* Print usage */
void print_usage(const char *prog_name) {
    printf("Kotha Programming Language v%s\n\n", VERSION);
    printf("Usage:\n");
    printf("  %s <command> [options] [file]\n\n", prog_name);
    
    printf("Commands:\n");
    printf("  build <file>     Compile Kotha file to executable\n");
    printf("  run <file>       Compile and run Kotha file\n");
    printf("  repl             Start interactive REPL\n");
    printf("  help             Show this help message\n");
    printf("  version          Show version information\n");
    printf("\n");
    
    printf("Build Options:\n");
    printf("  -o <file>        Specify output file name\n");
    printf("  --vm             Build for VM mode\n");
    printf("\n");
    
    printf("Run Options:\n");
    printf("  --vm             Run in VM mode\n");
    printf("  --debug          Enable debug output\n");
    printf("\n");
    
    printf("Legacy Options (deprecated):\n");
    printf("  -c, --compile    Compile to C\n");
    printf("  -v, --vm         Run in VM\n");
    printf("  -i, --interpret  Direct interpretation\n");
    printf("  -b, --bytecode   Show bytecode\n");
    printf("\n");
    
    printf("Examples:\n");
    printf("  %s build program.kotha\n", prog_name);
    printf("  %s build program.kotha -o myapp\n", prog_name);
    printf("  %s run program.kotha\n", prog_name);
    printf("  %s run program.kotha --vm --debug\n", prog_name);
    printf("  %s repl\n", prog_name);
    printf("\n");
}

/* Print version */
void print_version() {
    printf("Kotha Compiler v%s\n", VERSION);
    printf("A Bengali programming language\n");
    printf("Built with: Flex, Bison, C\n");
}

/* Parse command-line arguments */
Config parse_args(int argc, char **argv) {
    Config config = {
        .command = CMD_LEGACY,
        .mode = MODE_COMPILE_C,
        .input_file = NULL,
        .output_file = NULL,
        .debug = 0,
        .verbose = 0,
        .vm_mode = 0,
        .optimize_level = 0
    };
    
    // Check for subcommands
    if (argc > 1 && argv[1][0] != '-') {
        if (strcmp(argv[1], "build") == 0) {
            config.command = CMD_BUILD;
            config.mode = MODE_COMPILE_C;
        } else if (strcmp(argv[1], "run") == 0) {
            config.command = CMD_RUN;
            config.mode = MODE_COMPILE_C;
        } else if (strcmp(argv[1], "repl") == 0) {
            config.command = CMD_REPL;
            return config;
        } else if (strcmp(argv[1], "help") == 0) {
            config.command = CMD_HELP;
            return config;
        } else if (strcmp(argv[1], "version") == 0) {
            config.command = CMD_VERSION;
            return config;
        }
    }
    
    // Parse options
    int start_idx = (config.command == CMD_LEGACY) ? 1 : 2;
    
    for (int i = start_idx; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            config.command = CMD_HELP;
        } else if (strcmp(argv[i], "--version") == 0) {
            config.command = CMD_VERSION;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--compile") == 0) {
            config.mode = MODE_COMPILE_C;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--vm") == 0) {
            if (config.command == CMD_BUILD || config.command == CMD_RUN) {
                config.vm_mode = 1;
                config.mode = MODE_VM;
            } else {
                config.mode = MODE_VM;
            }
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interpret") == 0) {
            config.mode = MODE_INTERPRET;
        } else if (strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "--optimize") == 0) {
            config.mode = MODE_OPTIMIZE;
        } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bytecode") == 0) {
            config.mode = MODE_BYTECODE;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            config.debug = 1;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            config.output_file = argv[++i];
        } else if (argv[i][0] != '-') {
            config.input_file = argv[i];
        }
    }
    
    return config;
}

/* Main entry point */
int main(int argc, char **argv) {
    // Parse arguments
    Config config = parse_args(argc, argv);
    
    // Handle special commands
    if (config.command == CMD_HELP) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (config.command == CMD_VERSION) {
        print_version();
        return 0;
    }
    
    // Handle REPL
    if (config.command == CMD_REPL) {
        REPLConfig repl_config = {
            .debug = config.debug,
            .vm_mode = 1
        };
        return repl_start(&repl_config);
    }
    
    // Handle build command
    if (config.command == CMD_BUILD) {
        if (!config.input_file) {
            fprintf(stderr, "Error: No input file specified for build command\n");
            fprintf(stderr, "Usage: %s build <file.kotha> [-o output]\n", argv[0]);
            return 1;
        }
        
        printf("🔨 Building %s...\n", config.input_file);
        
        // Set output file if not specified
        if (!config.output_file) {
            // Remove .kotha extension and use as executable name
            char *base = strdup(config.input_file);
            char *dot = strrchr(base, '.');
            if (dot) *dot = '\0';
            config.output_file = base;
        }
        
        // Continue with normal compilation flow
        // Will compile and create executable
    }
    
    // Handle run command
    if (config.command == CMD_RUN) {
        if (!config.input_file) {
            fprintf(stderr, "Error: No input file specified for run command\n");
            fprintf(stderr, "Usage: %s run <file.kotha> [--vm] [--debug]\n", argv[0]);
            return 1;
        }
        
        if (config.debug) {
            printf("▶️  Running %s...\n", config.input_file);
        }
        
        // Continue with normal execution flow
    }
    
    // Check input file for legacy and build/run modes
    if (!config.input_file && config.command != CMD_REPL) {
        fprintf(stderr, "Error: No input file specified\n");
        fprintf(stderr, "Use --help for usage information\n");
        return 1;
    }
    
    // Open input file
    yyin = fopen(config.input_file, "r");
    if (!yyin) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", config.input_file);
        return 1;
    }
    
    // Parse the input
    if (config.debug) {
        fprintf(stderr, "Parsing %s...\n", config.input_file);
    }
    
    // Disable C generation for non-C modes
    if (config.mode != MODE_COMPILE_C) {
        generate_c = 0;
    }
    
    if (config.debug) {
        fprintf(stderr, "DEBUG: generate_c = %d\n", generate_c);
    }
    
    int parse_result = yyparse();
    fclose(yyin);
    
    if (parse_result != 0) {
        fprintf(stderr, "Parse error\n");
        return 1;
    }
    
    // Execute based on mode
    switch (config.mode) {
        case MODE_COMPILE_C:
            // Default mode - C code already generated by parser
            if (config.debug) {
                fprintf(stderr, "C code generated successfully\n");
            }
            break;
        
        case MODE_VM: {
            if (config.debug) {
                fprintf(stderr, "Generating bytecode...\n");
            }
            
            // Generate IR from AST
            if (!ir_head) {
                if (config.debug) fprintf(stderr, "Generating 3-Address Code (IR)...\n");
                ir_init();
                ir_generate(root);
            }
            
            if (!ir_head) {
                fprintf(stderr, "Error: No IR generated from AST\n");
                return 1;
            }
            
            // Generate bytecode
            VM *vm = codegen_vm(ir_head);
            if (!vm) {
                fprintf(stderr, "Error: Bytecode generation failed\n");
                return 1;
            }
            
            if (config.debug) {
                fprintf(stderr, "Running in VM...\n");
                vm->debug_mode = 1;
            }
            
            // Run VM
            vm_run(vm);
            
            if (config.debug) {
                fprintf(stderr, "\nVM Statistics:\n");
                fprintf(stderr, "  Instructions executed: %d\n", vm->instruction_count);
                fprintf(stderr, "  GC runs: %d\n", vm->gc_count);
            }
            
            vm_free(vm);
            free(vm);
            break;
        }
        
        case MODE_INTERPRET:
            if (config.debug) {
                fprintf(stderr, "Interpreting...\n");
            }
            
            if (!root) {
                fprintf(stderr, "Error: No AST generated\n");
                return 1;
            }
            
            interpret(root);
            
            if (config.debug) {
                fprintf(stderr, "\nInterpretation complete\n");
            }
            break;
        
        case MODE_OPTIMIZE:
            if (config.debug) {
                fprintf(stderr, "Running optimizer...\n");
            }
            
            // Generate IR first
            ir_init();
            ir_generate(root);
            
            reset_opt_stats();
            optimize(); // Logic in optimizer.c might need updating to work on IR list if it doesn't already
            // Current optimizer works on AST or strings, let's assume it's standalone for now
            // But ir_print() would be good here
            if (ir_head) ir_print();
            
            print_opt_stats();
            break;
        
        case MODE_BYTECODE: {
            if (!ir_head) {
                ir_init();
                ir_generate(root);
            }
            
            if (!ir_head) {
                fprintf(stderr, "Error: No IR generated\n");
                return 1;
            }
            
            VM *vm = codegen_vm(ir_head);
            if (!vm) {
                fprintf(stderr, "Error: Bytecode generation failed\n");
                return 1;
            }
            
            vm_disassemble(vm);
            
            vm_free(vm);
            free(vm);
            break;
        }
        
        default:
            fprintf(stderr, "Error: Unknown execution mode\n");
            return 1;
    }
    
    return 0;
}
