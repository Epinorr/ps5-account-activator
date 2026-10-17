#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENDOR="${ROOT}/.vendor/np-fake-signin"

if [ ! -d "${VENDOR}/template" ]; then
  rm -rf "${ROOT}/.vendor"
  mkdir -p "${ROOT}/.vendor"
  git clone --depth 1 --branch 1.1 https://github.com/earthonion/np-fake-signin.git "${VENDOR}"
fi

mkdir -p "${ROOT}/include/generated" "${ROOT}/output"

# Generate headers from upstream template dat files
python3 "${VENDOR}/gen_dat/patch_dat_files.py" patch \
  "${VENDOR}/template" "${ROOT}/output" "${NP_USER:-User1}"

cd "${ROOT}/output"
xxd -i auth.dat > "${ROOT}/include/generated/auth_dat.h"
xxd -i config.dat > "${ROOT}/include/generated/config_dat.h"

grep -q 'unsigned char auth_dat\[' "${ROOT}/include/generated/auth_dat.h"
grep -q 'unsigned char config_dat\[' "${ROOT}/include/generated/config_dat.h"