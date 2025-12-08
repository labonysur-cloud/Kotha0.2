/*
 * Kotha REPL (Read-Eval-Print Loop)
 * Interactive mode for Kotha programming
 */

#ifndef REPL_H
#define REPL_H

#include "vm.h"

/* REPL configuration */
typedef struct {
    int debug;
    int vm_mode;
} REPLConfig;

/* Start REPL */
int repl_start(REPLConfig *config);

/* Execute a single line in REPL */
int repl_execute_line(const char *line, VM *vm);

/* Handle REPL commands (:help, :quit, etc.) */
int repl_handle_command(const char *cmd, VM *vm);

#endif /* REPL_H */
