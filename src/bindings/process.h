#ifndef MN_BINDINGS_PROCESS_H
#define MN_BINDINGS_PROCESS_H

#include <quickjs/quickjs.h>

/* Inisialisasi global `process` object */
void mn_init_process(JSContext *ctx, int argc, char **argv);

#endif /* MN_BINDINGS_PROCESS_H */
