from pathlib import Path

root = Path(__file__).resolve().parents[1]
mk = (root / 'Makefile').read_text()
wf = (root / '.github/workflows/ps5.yml').read_text()
src = (root / 'np-fake-signin.c').read_text()
prep = (root / 'tools/prepare_upstream.sh').read_text()

assert 'generated:' in mk
assert '$(ELF): generated' in mk
assert 'bash ./tools/prepare_upstream.sh' in mk
assert 'ELF := EPINOR-NP-Fake-Signin.elf' in mk
assert 'test -s EPINOR-NP-Fake-Signin.elf' in wf
assert 'dist/EPINOR-NP-Fake-Signin.elf' in wf
assert 'xxd' in wf
assert 'basename inputs' in prep
assert '#ifndef PS5\n#include "hmac_md5.h"\n#endif' in src
assert not (root / '.github/workflows/release.yml').exists()
print('Project validation: PASS')
