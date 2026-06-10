# Runtime Data Shadowing (RDS) via Intel SGX

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-14-blue.svg)](https://isocpp.org/)
[![Intel SGX SDK](https://img.shields.io/badge/Intel%20SGX-v2.17-blue.svg)](https://software.intel.com/en-us/sgx)

[cite_start]**Official Code Repository for "Runtime Data Shadowing: A Trusted Execution Environment (TEE) Approach for Secure Medical Data Sharing"** *(Currently Under Review: 6th International Conference on Emerging Trends and Technologies on Intelligent Systems - ETTIS 2026, Springer)*[cite: 410].

## TL;DR
[cite_start]This project implements Runtime Data Shadowing (RDS), a computing-layered framework that uses Role-Based Access Control (RBAC)-enabled Trusted Execution Environments (TEEs) to securely share medical records[cite: 86]. [cite_start]By anchoring the Trusted Computing Base (TCB) exclusively inside CPU silicon, RDS excludes the host Operating System and hypervisor from trust assumptions[cite: 87, 103]. [cite_start]Plaintext data resides only within the Enclave Page Cache (EPC) and is protected in DRAM via the Memory Encryption Engine (MEE) using AES-128-GCM[cite: 88, 89].

## Key Results & Hardware Benchmarks
[cite_start]We explicitly designed RDS to avoid the non-deterministic state synchronization delays of full enclave-resident databases, achieving deterministic, microsecond-level $O(1)$ projection latency[cite: 105, 157].

* [cite_start]**Latency:** Deterministic per-record query latency of **8.93 µs** ($\sigma$ = **0.12 µs**, $P99$ = **9.22 µs**)[cite: 90].
* [cite_start]**Throughput:** Maintained **112,046 Ops/sec** across 100 endurance trials[cite: 192, 412].
* [cite_start]**Overhead:** Represents a highly optimized **2.9x** architectural execution penalty compared to unprotected native execution[cite: 90, 411].
* [cite_start]**EPC Paging Boundary:** Identified a critical 15x latency degradation (> **140 µs**) triggered systematically when exceeding the **128MB** physical EPC boundary[cite: 203, 413].

## Architecture Overview
[cite_start]The system relies on an interface boundary (`Enclave.edl`) that forces deep buffer copies prior to applying security checks, neutralizing Time-of-Check to Time-of-Use (TOCTOU) vulnerabilities[cite: 175, 177]. 

[cite_start]*(Include your System Architecture Diagram here: e.g., `![Architecture Flow](link_to_image.png)` showing the Host OS vs. the TEE enclave boundary)*[cite: 68].

## Repository Structure
[cite_start]The project contains roughly **3,400** lines of C/C++ code, logically separated across the trust boundary[cite: 171].

* [cite_start]**`/host`**: Untrusted host application (~**1,200** LOC) managing I/O, thread pooling, and database loading[cite: 172].
* **`/enclave`**: The Trusted Access Computation Layer (TACL) core (~**2,200** LOC). [cite_start]Integrates a statically-linked Intel IPP Cryptography library for secure AES-128-GCM workloads[cite: 173, 174].
* [cite_start]**`/data`**: Synthetic patient dataset modeled on standard MIMIC-III layout patterns, padded to **1024-byte** blocks to align with HL7 FHIR Observation sizes[cite: 181, 182].

## System Requirements & Reproduction
[cite_start]To reliably reproduce the latency and throughput metrics, the following environment is required[cite: 179, 180]:

* [cite_start]**CPU:** Intel Processor with SGX Support (Evaluated on Intel Core i5-8500, 3.00 GHz)[cite: 179].
* [cite_start]**Memory:** Minimum **128MB** EPC allocation[cite: 180].
* [cite_start]**OS Kernel:** Ubuntu 20.04 LTS (5.4.0-150-generic)[cite: 180].
* [cite_start]**SDK:** Intel SGX Linux SDK v2.17[cite: 171].
* [cite_start]**Compiler:** gcc 9.4.0[cite: 171].

### Build Instructions
1. Ensure the Intel SGX SDK and SGX driver are correctly installed and sourced in your environment.
2. Clone this repository.
3. Execute `make` in the root directory to compile both the untrusted host and the trusted enclave objects.
4. Run the executable: `./rds_app`
