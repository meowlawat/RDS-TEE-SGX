#ifndef USER_TYPES_H
#define USER_TYPES_H

#include <stdint.h>

// 1. The Full "Sensitive" Record (Inside Vault)
typedef struct _patient_record_t {
    uint64_t subject_id;
    char name[64];          // PII
    int age;
    char diagnosis[64];
    char social_security[16]; // PII
    char insurance[32];
} patient_record_t;

// 2. The "Shadow View" (Returned to User)
// We use a generic buffer to simulate the "projected" view
typedef struct _shadow_view_t {
    uint64_t subject_id;     // 0 if hidden
    int age;                 // -1 if hidden
    char visible_data[256];  // Combined allowed strings
    int is_sanitized;        // Flag: 0 = Full, 1 = Sanitized
} shadow_view_t;

// 3. Roles
typedef enum _user_role_t {
    ROLE_ONCOLOGIST = 1,
    ROLE_RESEARCHER = 2
} user_role_t;

#endif
