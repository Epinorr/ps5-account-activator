# EPINOR Combined PS5 NP Fake Signin
# PS5-only: offline account activation + idempotent NP Fake Signin.

ifndef PS5_PAYLOAD_SDK
$(error PS5_PAYLOAD_SDK is undefined)
endif

include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

ELF := EPINOR-NP-Fake-Signin.elf

CFLAGS := -std=gnu11 -DPS5 -O2 -Wall -Wextra -Werror \
          -Wno-unused-parameter -I. -Iinclude -Iinclude/generated

LDADD := -lSceUserService -lSceRegMgr -lSceSystemService -lkernel

SRCS := np-fake-signin.c \
        src/account_activator.c \
        src/notification.c

.PHONY: all generated clean

all: $(ELF)

# Generate the two binary-data headers before the C target is considered buildable.
# This is a single phony prerequisite so `make clean all` cannot try to compile
# before the generated headers exist.
generated:
	bash bash ./tools/prepare_upstream.sh

$(ELF): generated $(SRCS) include/auth_dat.h include/config_dat.h \
        include/account_activator.h include/notification.h hmac_md5.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDADD)
	$(PS5_PAYLOAD_SDK)/bin/prospero-strip --strip-all $@

clean:
	rm -f $(ELF) *.o src/*.o
	rm -rf include/generated output .vendor .build
	rm -f include/auth_dat.h include/config_dat.h
