SHELL := /bin/bash

ifndef PS5_PAYLOAD_SDK
$(error PS5_PAYLOAD_SDK is undefined)
endif

include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

ELF := EPINOR-NP-Fake-Signin.elf

SRCS := np-fake-signin.c \
        src/account_activator.c \
        src/notification.c

GENERATED_HEADERS := include/auth_dat.h include/config_dat.h

CFLAGS := -std=gnu11 -DPS5 -O2 -Wall -Wextra -Werror \
          -Wno-unused-parameter -I. -Iinclude -Iinclude/generated

LDADD := -lSceUserService -lSceRegMgr -lSceSystemService -lkernel

.PHONY: all generated clean

all: $(ELF)

# IMPORTANT:
# The headers are real Make targets. This prevents GNU make from failing
# during dependency discovery before the generation recipe has run.
$(ELF): $(SRCS) $(GENERATED_HEADERS) \
        include/account_activator.h include/notification.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDADD)
	$(PS5_PAYLOAD_SDK)/bin/prospero-strip --strip-all $@

$(GENERATED_HEADERS): generated
	@test -s $@

# Generates the original upstream NP data and converts it to C headers.
# `bash` is used explicitly so the executable permission bit is irrelevant.
generated:
	bash ./tools/prepare_upstream.sh

clean:
	rm -f $(ELF) *.o src/*.o
	rm -rf include/generated output .vendor .build
	rm -f $(GENERATED_HEADERS)
