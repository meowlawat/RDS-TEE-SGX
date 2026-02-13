#include "Enclave_t.h"
#include "user_types.h"
#include "sgx_trts.h"
#include <string.h>
#include <stdio.h> /* for snprintf */

// Helper to print debug info (calls out to App)
void printf(const char *fmt, ...) {
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

// --- THE TACL PROJECTION LOGIC ---
void ecall_project_shadow(patient_record_t* in_rec, user_role_t role, shadow_view_t* out_view) {
    
    // 1. Initialize Output (Secure Wipe)
    memset(out_view, 0, sizeof(shadow_view_t));

    // 2. ACCESS CONTROL LOGIC (The "Policy Engine")
    if (role == ROLE_ONCOLOGIST) {
        // --- FULL ACCESS ---
        out_view->subject_id = in_rec->subject_id;
        out_view->age = in_rec->age;
        out_view->is_sanitized = 0;
        
        // Construct Full View
        snprintf(out_view->visible_data, 256, 
            "NAME:%s | DIAG:%s | SSN:%s | INS:%s", 
            in_rec->name, in_rec->diagnosis, in_rec->social_security, in_rec->insurance);

    } else if (role == ROLE_RESEARCHER) {
        // --- SANITIZED ACCESS (Privacy Preserving) ---
        // HIDE Identity (Subject ID, Name, SSN)
        out_view->subject_id = 0; // NULLIFY ID
        out_view->age = in_rec->age; // ALLOW Age
        out_view->is_sanitized = 1;

        // Construct Partial View (Only Diagnosis & Insurance)
        snprintf(out_view->visible_data, 256, 
            "NAME:***MASKED*** | DIAG:%s | SSN:***MASKED***", 
            in_rec->diagnosis);
    }
    
    // 3. MEMORY CLEANUP
    // (SGX stack is auto-wiped on return, but good practice to mention)
}
