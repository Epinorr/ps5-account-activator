from pathlib import Path

root = Path(__file__).resolve().parents[1]
mk = (root / "Makefile").read_text()
wf = (root / ".github/workflows/ps5.yml").read_text()
src = (root / "np-fake-signin.c").read_text()

assert "$(GENERATED_HEADERS): generated" in mk
assert "bash ./tools/prepare_upstream.sh" in mk
assert "ELF := EPINOR-NP-Fake-Signin.elf" in mk
assert "test -s EPINOR-NP-Fake-Signin.elf" in wf
assert "dist/EPINOR-NP-Fake-Signin.elf" in wf
assert 'account_activator_run(' in src
assert 'Already signed in' in src
assert 'Coded by EPINOR' in src
assert not (root / ".github/workflows/release.yml").exists()

print("Project validation: PASS")
