#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENDOR="${ROOT}/.vendor/np-fake-signin"
UPSTREAM_URL="https://github.com/earthonion/np-fake-signin.git"

mkdir -p "${ROOT}/.vendor" "${ROOT}/output" "${ROOT}/include/generated"

if [ ! -d "${VENDOR}/.git" ]; then
  rm -rf "${VENDOR}"
  git clone --depth 1 --branch 1.1 "${UPSTREAM_URL}" "${VENDOR}"
fi

for f in auth.dat config.dat; do
  test -f "${VENDOR}/template/${f}"
done

# Reuse the upstream project's own patching/generation logic.
python3 "${VENDOR}/gen_dat/patch_dat_files.py" patch \
  "${VENDOR}/template" "${ROOT}/output" "${NP_USER:-User1}"

xxd -i "${ROOT}/output/auth.dat" > "${ROOT}/include/generated/auth_dat.h"
xxd -i "${ROOT}/output/config.dat" > "${ROOT}/include/generated/config_dat.h"

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

test -s "${ROOT}/include/auth_dat.h"
test -s "${ROOT}/include/config_dat.h"
