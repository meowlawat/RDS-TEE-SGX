#ifndef _APP_H_
#define _APP_H_

#include <stdio.h>
#include "sgx_error.h"
#include "sgx_eid.h"

#if defined(__cplusplus)
extern "C" {
#endif

// 1. Define the Enclave Filename for Linux (IMPORTANT)
// Windows uses .dll, Linux uses .so. We fix this here.
#define ENCLAVE_FILENAME "enclave.signed.so"

// 2. OCALL Prototype
// This allows the Enclave to call "printf" in the main app
void ocall_print_string(const char *str);

#if defined(__cplusplus)
}
#endif

#endif /* _APP_H_ */
