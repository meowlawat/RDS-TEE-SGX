SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= HW
SGX_ARCH ?= x64
SGX_DEBUG ?= 1

SGX_COMMON_FLAGS := -m64 -O2
SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r

# Enclave Flags - THIS FIXES THE METADATA ERROR
Enclave_Link_Flags := -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l{SGX_TRTS_LIB} -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcrypto -l{SGX_TSERVICE_LIB} -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=Enclave/Enclave.lds

ifeq ($(SGX_MODE), HW)
	Enclave_Link_Flags := $(subst {SGX_TRTS_LIB},sgx_trts,$(Enclave_Link_Flags))
	Enclave_Link_Flags := $(subst {SGX_TSERVICE_LIB},sgx_tservice,$(Enclave_Link_Flags))
else
	Enclave_Link_Flags := $(subst {SGX_TRTS_LIB},sgx_trts_sim,$(Enclave_Link_Flags))
	Enclave_Link_Flags := $(subst {SGX_TSERVICE_LIB},sgx_tservice_sim,$(Enclave_Link_Flags))
endif

.PHONY: all clean

all: app

# 1. Generate Edger8r Files
App/Enclave_u.c App/Enclave_u.h: Enclave/Enclave.edl
	$(SGX_EDGER8R) --untrusted Enclave/Enclave.edl --search-path Enclave --search-path $(SGX_SDK)/include --untrusted-dir App

Enclave/Enclave_t.c Enclave/Enclave_t.h: Enclave/Enclave.edl
	$(SGX_EDGER8R) --trusted Enclave/Enclave.edl --search-path Enclave --search-path $(SGX_SDK)/include --trusted-dir Enclave

# 2. Compile Enclave Objects
Enclave/Enclave.o: Enclave/Enclave.cpp Enclave/Enclave_t.c
	g++ $(SGX_COMMON_FLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector -fPIC -c Enclave/Enclave.cpp -o Enclave/Enclave.o -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -IEnclave

Enclave/Enclave_t.o: Enclave/Enclave_t.c
	g++ $(SGX_COMMON_FLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector -fPIC -c Enclave/Enclave_t.c -o Enclave/Enclave_t.o -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -IEnclave

# 3. Link Enclave (THE FIX)
Enclave/enclave.so: Enclave/Enclave.o Enclave/Enclave_t.o Enclave/Enclave.lds
	g++ Enclave/Enclave.o Enclave/Enclave_t.o -o Enclave/enclave.so $(Enclave_Link_Flags)

# 4. Sign Enclave
enclave.signed.so: Enclave/enclave.so Enclave/Enclave.config.xml
	$(SGX_ENCLAVE_SIGNER) sign -key enclave_private_test.pem -enclave Enclave/enclave.so -out enclave.signed.so -config Enclave/Enclave.config.xml

# 5. Compile App
App/App.o: App/App.cpp App/Enclave_u.h
	g++ $(SGX_COMMON_FLAGS) -c App/App.cpp -o App/App.o -I$(SGX_SDK)/include -IApp -IEnclave

App/Enclave_u.o: App/Enclave_u.c
	g++ $(SGX_COMMON_FLAGS) -c App/App.cpp -o App/Enclave_u.o -I$(SGX_SDK)/include -IApp -IEnclave

# 6. Link App
app: App/App.o App/Enclave_u.c enclave.signed.so
	g++ $(SGX_COMMON_FLAGS) -o app App/App.cpp App/Enclave_u.c -L$(SGX_LIBRARY_PATH) -lsgx_urts -I$(SGX_SDK)/include -IApp -IEnclave

clean:
	rm -f App/*.o Enclave/*.o Enclave/*.so *.signed.so app App/Enclave_u.* Enclave/Enclave_t.*
