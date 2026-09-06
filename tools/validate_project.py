from pathlib import Path
import re, sys

root = Path(__file__).resolve().parents[1]
wf = (root / ".github/workflows/ps5.yml").read_text()
mk = (root / "Makefile").read_text()

assert "EPINOR-NP-Fake-Signin.elf" in mk
assert "test -s EPINOR-NP-Fake-Signin.elf" in wf
assert "dist/EPINOR-NP-Fake-Signin.elf" in wf
assert not (root / ".github/workflows/release.yml").exists()
assert "bash ./tools/prepare_upstream.sh" in mk
print("Project validation: PASS")
