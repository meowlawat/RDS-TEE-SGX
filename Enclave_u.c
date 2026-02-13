#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_shadow_record_t {
	ShadowRecord ms_retval;
	Record* ms_input;
	int ms_role;
} ms_ecall_shadow_record_t;

typedef struct ms_ocall_print_string_t {
	const char* ms_str;
} ms_ocall_print_string_t;

static sgx_status_t SGX_CDECL Enclave_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string(ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Enclave = {
	1,
	{
		(void*)Enclave_ocall_print_string,
	}
};
sgx_status_t ecall_shadow_record(sgx_enclave_id_t eid, ShadowRecord* retval, Record* input, int role)
{
	sgx_status_t status;
	ms_ecall_shadow_record_t ms;
	ms.ms_input = input;
	ms.ms_role = role;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

