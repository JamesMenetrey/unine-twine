BUILD_DIR = build
Wolfssl_Output_Name = wasm_example
Wolfssl_C_Files = main.c

Wolfssl_C_Files += $(WOLFSSL_ROOT)/wolfcrypt/src/aes.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/arc4.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/asn.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/blake2b.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/camellia.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/coding.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/chacha.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/chacha20_poly1305.c\
					$(WOLFSSL_ROOT)/src/crl.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/des3.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/dh.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/tfm.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/ecc.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/error.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/hash.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/kdf.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/hmac.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/integer.c\
					$(WOLFSSL_ROOT)/src/internal.c\
					$(WOLFSSL_ROOT)/src/wolfio.c\
					$(WOLFSSL_ROOT)/src/keys.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/logging.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/md4.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/md5.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/memory.c\
					$(WOLFSSL_ROOT)/src/ocsp.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/pkcs7.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/pkcs12.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/poly1305.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wc_port.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wolfmath.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/pwdbased.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/random.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/ripemd.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/rsa.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/dsa.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha3.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha256.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha512.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/signature.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sp_c32.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sp_c64.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sp_int.c\
					$(WOLFSSL_ROOT)/src/ssl.c\
					$(WOLFSSL_ROOT)/src/tls.c\
					$(WOLFSSL_ROOT)/src/tls13.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wc_encrypt.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wolfevent.c

Wolfssl_C_Files += $(WOLFSSL_ROOT)/wolfcrypt/benchmark/benchmark.c

Wolfssl_Include_Paths = -I.
Wolfssl_Include_Paths += -I$(WOLFSSL_ROOT)/wolfcrypt/benchmark/
Wolfssl_Include_Paths += -I$(WOLFSSL_ROOT)/ \
						 -I$(WOLFSSL_ROOT)/wolfcrypt/


Wolfssl_C_Extra_Flags := -DHAVE_WOLFSSL_BENCHMARK

include ../common.mk

.PHONY: all
all:
	mkdir -p $(BUILD_DIR)
	clang \
		-lm \
		$(COMPILE_FLAGS) \
		$(Wolfssl_C_Extra_Flags) \
		$(Wolfssl_Include_Paths) \
		-o $(BUILD_DIR)/$(Wolfssl_Output_Name) $(Wolfssl_C_Files) --rtlib=compiler-rt