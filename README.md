<div align="center">

# 🎮 EPINOR PS5 Account Activator & NP Fake Signin

**A lightweight, all-in-one PS5 ELF payload that seamlessly combines offline account activation with PlayStation Network (NP) Fake Signin in a single execution.**

[![Build Status](https://img.shields.io/github/actions/workflow/status/Epinorr/ps5-account-activator/ps5.yml?branch=main&style=flat-square&logo=github&label=Build)](../../actions)
[![Latest Release](https://img.shields.io/github/v/release/Epinorr/ps5-account-activator?style=flat-square&logo=playstation&label=Release)](../../releases/latest)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-PS5-003791?style=flat-square&logo=playstation)](https://github.com/ps5-payload-dev/sdk)

[📥 Download Latest ELF](../../releases/latest) • [⚙️ How It Works](#-execution-flow--behavior) • [🛠️ Build from Source](#️-building-from-source)

</div>

---

## ✨ Key Features

* **⚡ One-Shot Execution:** Automatically detects the active foreground user, checks registry states, and performs offline activation + fake sign-in without requiring multiple payloads.
* **🛡️ Idempotent & Safe:** Smart state verification ensures registry values and NP `.dat` files are only written when necessary, preventing redundant writes or data corruption.
* **🌍 Automatic Region Detection:** Dynamically adjusts account locale and country settings based on your PS5's system language.
* **🔔 Clean System Notifications:** Displays clear, real-time status notifications directly on your PS5 screen (`Coded by EPINOR`).
* **🪶 Optimized Binary:** Stripped of legacy PS4 memory-patching overhead, compiled with garbage-collected sections for a minimal footprint.

---

## ⚙️ Execution Flow & Behavior

When injected, the payload evaluates the current foreground user and follows an intelligent, non-destructive flow:

1. **Foreground User Detection:** Identifies the currently logged-in local user profile.
2. **Offline Activation Check:** Reads the user's Account ID; if unactivated (`0`), generates and writes a valid offline Account ID to the registry.
3. **Sign-In State Verification:** Checks if the profile is already in an active Fake Sign-in state.
4. **NP Provisioning:** If signed out, generates HMAC-MD5 signed `.dat` files (`auth.dat`, `account.dat`, `token.dat`, `config.dat`) and updates the required NP registry flags.

### State Matrix

| Initial Account State | Initial NP State | Action Taken by Payload | PS5 Notification |
| :--- | :--- | :--- | :--- |
| ❌ Inactive | ❌ Signed Out | **Activates Account** ➔ **Applies Fake Signin** | `Activated + Signed in🐈` |
| ✅ Active | ❌ Signed Out | *Skips Activation* ➔ **Applies Fake Signin** | `Signed in🐈` |
| ✅ Active | ✅ Signed In | *Skips Activation* ➔ *Skips Fake Signin* | `Already Activated + Signed in🐈` |

---

## 🚀 Usage & Requirements

### Requirements
* A Jailbroken / Exploited **PlayStation 5** console.
* Any standard PS5 ELF payload loader (e.g., via `payloadmanager`, built-in ELF loader on port `9021`, or web-based payload sender).

### Quick Start
1. Download `AIO-Activator.elf` from the **[Latest Release](../../releases/latest)** page.
2. Log into the PS5 user profile you wish to activate.
3. Send the `.elf` file to your PS5 using your preferred payload sender (e.g., `nc <PS5_IP> 9021 < AIO-Activator.elf`).
4. Wait for the on-screen notification confirming activation and sign-in, then reboot or re-login to apply all profile changes.

---

## 🛠️ Building from Source

This project uses the **[PS5 Payload SDK](https://github.com/ps5-payload-dev/sdk)**. Note that GitHub Actions automatically builds, validates, and packages the ELF on every push and release tag.

### Prerequisites (Ubuntu / Debian)
```bash
sudo apt-get update
sudo apt-get install -y clang-18 lld-18 python3 xxd git make
```

### Build Steps
1. **Install the PS5 Payload SDK (v0.42):**
   ```bash
   git clone --depth 1 --branch v0.42 [https://github.com/ps5-payload-dev/sdk.git](https://github.com/ps5-payload-dev/sdk.git)
   make -C sdk DESTDIR=/tmp/ps5-payload-sdk clean install
   export PS5_PAYLOAD_SDK=/tmp/ps5-payload-sdk
   ```

2. **Clone and Compile the Payload:**
   ```bash
   git clone [https://github.com/Epinorr/ps5-account-activator.git](https://github.com/Epinorr/ps5-account-activator.git)
   cd ps5-account-activator
   make clean && make all
   ```
   *Note: The build process automatically runs `tools/prepare_upstream.sh` to fetch upstream binary `.dat` templates and generate the required C headers.*

The compiled binary will be generated at the project root as `AIO-Activator.elf`.

---

## 📁 Project Structure

```text
├── .github/workflows/
│   └── ps5.yml                # Automated CI/CD build & GitHub Release pipeline
├── include/
│   ├── account_activator.h    # Offline account activation definitions
│   └── notification.h         # Unified PS5 notification wrapper
├── src/
│   ├── account_activator.c    # Registry offline activation logic
│   └── notification.c         # System notification sender (EPINOR formatted)
├── tools/
│   ├── prepare_upstream.sh    # Fetches & patches NP .dat templates into C headers
│   └── validate_project.py    # Pre-build structural sanity checks
├── hmac_md5.h                 # Standalone RFC 2104 HMAC-MD5 implementation
├── np-fake-signin.c           # Main entry point & NP Fake Signin provisioning
└── Makefile                   # Prospero toolchain build configuration
```

---

## 🤝 Credits & Acknowledgments

This combined utility builds upon the research and open-source work of the PS5 scene:

* **[earthonion/np-fake-signin](https://github.com/earthonion/np-fake-signin)** — Original NP Fake Signin research, `.dat` structure analysis, and template generation.
* **[etaHEN](https://github.com/etaHEN/etaHEN) & [PS5Dev/OffAct](https://github.com/ps5-payload-dev/offact)** — Offline account activation algorithm and registry key mappings.
* **[ps5-payload-dev/sdk](https://github.com/ps5-payload-dev/sdk)** — Prospero Clang toolchain and SDK libraries.

---

## 📄 License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)** — see the [LICENSE](LICENSE) file for full details.