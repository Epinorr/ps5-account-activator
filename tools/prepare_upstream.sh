#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENDOR="${ROOT}/.vendor/np-fake-signin"

if [ ! -d "${VENDOR}/template" ]; then
  rm -rf "${ROOT}/.vendor"
  mkdir -p "${ROOT}/.vendor"
  git clone --depth 1 --branch 1.1 https://github.com/earthonion/np-fake-signin.git "${VENDOR}"
fi

mkdir -p "${ROOT}/include/generated"

# The upstream make flow produces these headers from its template dat files.
python3 "${VENDOR}/gen_dat/patch_dat_files.py" patch \
  "${VENDOR}/template" "${ROOT}/output" "${NP_USER:-User1}"

xxd -i "${ROOT}/output/auth.dat" > "${ROOT}/include/generated/auth_dat.h"
xxd -i "${ROOT}/output/config.dat" > "${ROOT}/include/generated/config_dat.h"

# Normalize symbol names used by the original C source.
sed -i \
  -e 's/unsigned char output_auth_dat\[\]/unsigned char auth_dat[]/' \
  -e 's/unsigned int output_auth_dat_len/unsigned int auth_dat_len/' \
  "${ROOT}/include/generated/auth_dat.h"

sed -i \
  -e 's/unsigned char output_config_dat\[\]/unsigned char config_dat[]/' \
  -e 's/unsigned int output_config_dat_len/unsigned int config_dat_len/' \
  "${ROOT}/include/generated/config_dat.h"

cp "${ROOT}/include/generated/auth_dat.h" "${ROOT}/include/auth_dat.h"
cp "${ROOT}/include/generated/config_dat.h" "${ROOT}/include/config_dat.h"
