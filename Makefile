SHELL := /bin/bash

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

# Generation is the only prerequisite for the generated binary-data headers.
# Keeping them out of the ELF prerequisite list prevents make from requiring
# the files to exist before the generator has run.
$(ELF): generated $(SRCS) include/account_activator.h include/notification.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDADD)
	$(PS5_PAYLOAD_SDK)/bin/prospero-strip --strip-all $@

generated:
	bash ./tools/prepare_upstream.sh

clean:
	rm -f $(ELF) *.o src/*.o
	rm -rf include/generated output .vendor .build
	rm -f include/auth_dat.h include/config_dat.h
