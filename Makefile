# EPINOR Combined PS5 NP Fake Signin
# Account activation + idempotent offline fake sign-in.

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

.PHONY: all clean

all: $(ELF)

$(ELF): $(SRCS) include/auth_dat.h include/config_dat.h \
        include/account_activator.h include/notification.h hmac_md5.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDADD)
	$(PS5_PAYLOAD_SDK)/bin/prospero-strip --strip-all $@

clean:
	rm -f $(ELF) *.o src/*.o
	rm -rf include/generated
	rm -f include/auth_dat.h include/config_dat.h
