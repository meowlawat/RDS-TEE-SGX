#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ShadowRecord
#define _ShadowRecord
typedef struct ShadowRecord {
	int age;
	int gender;
} ShadowRecord;
#endif

#ifndef _Record
#define _Record
typedef struct Record {
	int subject_id;
	int age;
	int gender;
	int insurance;
} Record;
#endif

ShadowRecord ecall_shadow_record(Record* input, int role);

sgx_status_t SGX_CDECL ocall_print_string(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
