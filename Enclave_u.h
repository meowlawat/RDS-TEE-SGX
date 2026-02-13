#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */

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

#ifndef OCALL_PRINT_STRING_DEFINED__
#define OCALL_PRINT_STRING_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));
#endif

sgx_status_t ecall_shadow_record(sgx_enclave_id_t eid, ShadowRecord* retval, Record* input, int role);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
