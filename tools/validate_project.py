#!/usr/bin/env python3
from pathlib import Path
import re
root = Path(__file__).resolve().parents[1]
required = [
    ".github/workflows/ps5.yml",
    "Makefile",
    "np-fake-signin.c",
    "src/account_activator.c",
    "src/notification.c",
    "include/account_activator.h",
    "include/notification.h",
    "tools/prepare_upstream.sh",
]
for rel in required:
    assert (root / rel).is_file(), rel
mk = (root/"Makefile").read_text()
assert re.search(r"^\$\([A-Za-z_]+\): generated", mk, re.M)
assert re.search(r"^generated:", mk, re.M)
assert "include/auth_dat.h" in mk
src = (root/"np-fake-signin.c").read_text()
assert 'account_activator_run(' in src
assert 'is_ps5_fake_signed_in' in src
assert 'Already signed in' in src
assert 'Coded by EPINOR' in src
print("Project validation: PASS")
