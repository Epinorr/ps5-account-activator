#!/usr/bin/env python3
import sys
from pathlib import Path

def main():
    root = Path(__file__).resolve().parents[1]

    required_files = [
        "Makefile",
        ".github/workflows/ps5.yml",
        "np-fake-signin.c",
        "src/account_activator.c",
        "src/notification.c",
        "include/account_activator.h",
        "include/notification.h",
        "tools/prepare_upstream.sh",
    ]

    missing = [f for f in required_files if not (root / f).is_file()]
    if missing:
        print(f"Error: Missing required files: {', '.join(missing)}", file=sys.stderr)
        sys.exit(1)

    mk = (root / "Makefile").read_text()
    if "AIO-Activator.elf" not in mk:
        print("Error: Target ELF name mismatch in Makefile", file=sys.stderr)
        sys.exit(1)

    src = (root / "np-fake-signin.c").read_text()
    if "account_activator_run" not in src or "notify_epinor" not in src:
        print("Error: Core integration calls missing in np-fake-signin.c", file=sys.stderr)
        sys.exit(1)

    print("Project validation passed successfully.")

if __name__ == "__main__":
    main()