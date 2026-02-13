#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <chrono> 
#include <vector>  // For dynamic array (1M records)
#include <cstdlib> // For rand()

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"
#include "user_types.h"

// Global Enclave ID
sgx_enclave_id_t global_eid = 0;

// OCALL Implementation
void ocall_print_string(const char *str) {
    printf("%s", str); // Simple print
}

// --- HELPER: GET REAL MEMORY USAGE (Linux) ---
long get_memory_usage_kb() {
    long rss = 0;
    std::ifstream statm("/proc/self/statm");
    if (statm.is_open()) {
        long ignore;
        statm >> ignore >> rss; 
        statm.close();
    }
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    return rss * page_size_kb;
}

// --- HELPER: GENERATE DATASET ---
void generate_dataset(std::vector<patient_record_t>& dataset, int count) {
    printf("[SYSTEM] Generating %d records in Host RAM... ", count);
    dataset.reserve(count);
    
    for(int i=0; i<count; i++) {
        patient_record_t rec;
        rec.subject_id = 1000 + i;
        rec.age = 20 + (i % 70); // Random age 20-90
        
        // Alternate diagnoses to simulate variety
        if(i % 3 == 0) snprintf(rec.diagnosis, 64, "Stage I Carcinoma");
        else if(i % 3 == 1) snprintf(rec.diagnosis, 64, "Type 2 Diabetes");
        else snprintf(rec.diagnosis, 64, "Hypertension");

        snprintf(rec.name, 64, "Patient_%d", i);
        snprintf(rec.social_security, 16, "999-00-%04d", i % 10000);
        snprintf(rec.insurance, 32, (i%2==0) ? "BlueCross" : "Medicare");
        
        dataset.push_back(rec);
    }
    printf("Done.\n");
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed random number generator

    // 1. MEASURE BASELINE MEMORY
    long baseline_mem = get_memory_usage_kb();
    
    // 2. LAUNCH ENCLAVE
    sgx_status_t ret = SGX_SUCCESS;
    sgx_launch_token_t token = {0};
    int updated = 0;
    
    printf("====================================================\n");
    printf("[INIT] Launching SGX Enclave (TACL Engine)...\n");
    
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        printf("[ERROR] Failed to create Enclave: %x\n", ret);
        return -1;
    }

    // 3. GENERATE 1 MILLION RECORDS (Simulating Loaded DB)
    // We use a vector because 1M structs on the stack would crash the app.
    int NUM_RECORDS = 1000000; 
    std::vector<patient_record_t> database;
    generate_dataset(database, NUM_RECORDS);

    // 4. MEASURE MEMORY OVERHEAD
    // This now captures both the Enclave AND the Data footprint
    long active_mem = get_memory_usage_kb();
    long system_footprint = active_mem - baseline_mem;

    shadow_view_t public_view;
    int NUM_ITERATIONS = 1000000; // 1 Million Queries

    // 5. BENCHMARKING (High Load)
    printf("[BENCHMARK] Starting Stress Test...\n");
    printf("   -> Records:     1,000,000\n");
    printf("   -> Iterations:  1,000,000 (Random Access)\n");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for(int i=0; i<NUM_ITERATIONS; i++) {
        // Pick a RANDOM index to force cache misses (Real-world scenario)
        int rand_idx = rand() % NUM_RECORDS;
        
        // Pass the specific record pointer to the Enclave
        ecall_project_shadow(global_eid, &database[rand_idx], ROLE_ONCOLOGIST, &public_view);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    // 6. CALCULATE METRICS
    long long total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double avg_latency_us = (double)total_us / (double)NUM_ITERATIONS;
    double throughput_ops = (double)NUM_ITERATIONS / ((double)total_us / 1000000.0);

    // Security Coverage Calculation
    size_t total_size = sizeof(patient_record_t);
    size_t researcher_view_size = sizeof(int) + 64; 
    double security_coverage = 100.0 * (1.0 - ((double)researcher_view_size / (double)total_size));

    // 7. PRINT FINAL REPORT
    printf("\n");
    printf("========== RDS_SGX SCALE REPORT (1M/1M) ==========\n");
    printf("| Metric             | Value                   |\n");
    printf("|--------------------|-------------------------|\n");
    printf("| 1. Avg Latency     | %7.3f us / query    |\n", avg_latency_us);
    printf("| 2. Throughput      | %7.0f Ops / sec     |\n", throughput_ops);
    printf("| 3. System RAM Used | %7ld KB              |\n", system_footprint);
    printf("| 4. Security Cover  | %7.2f %%               |\n", security_coverage);
    printf("====================================================\n");

    sgx_destroy_enclave(global_eid);
    return 0;
}
