SHELL := /bin/bash

ifndef PS5_PAYLOAD_SDK
$(error PS5_PAYLOAD_SDK is undefined)
endif

include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

ELF := AIO-Activator.elf

CFLAGS := -std=gnu11 -DPS5 -O2 -Wall -Wextra -Werror \
          -Wno-unused-parameter -ffunction-sections -fdata-sections \
          -I. -Iinclude -Iinclude/generated

LDFLAGS := -Wl,--gc-sections
LDADD := -lSceUserService -lSceRegMgr -lSceSystemService -lkernel

SRCS := np-fake-signin.c \
        src/account_activator.c \
        src/notification.c

OBJS := $(SRCS:.c=.o)

.PHONY: all generated clean

all: $(ELF)

$(ELF): generated $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDADD)
	$(PS5_PAYLOAD_SDK)/bin/prospero-strip --strip-all $@

%.o: %.c generated include/account_activator.h include/notification.h
	$(CC) $(CFLAGS) -c -o $@ $<

generated:
	bash ./tools/prepare_upstream.sh

clean:
	rm -f $(ELF) $(OBJS)
	rm -rf include/generated output .vendor .build
	rm -f include/auth_dat.h include/config_dat.h