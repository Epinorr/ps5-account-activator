# EPINOR Combined PS5 NP Fake Signin

Version: `2.0.0`

A standalone PS5 payload that combines **offline account activation** with **NP Fake Signin** in a single execution.

## Behavior

The payload follows this order:

1. Detect the current foreground user.
2. Check the account activation state.
3. Activate the account only when its Account ID is zero.
4. Check whether the current user is already in the signed-in state.
5. Skip NP Fake Signin completely when the user is already signed in.
6. Otherwise perform the original NP Fake Signin flow.

This makes the payload idempotent for the two relevant states:

```text
Account inactive + signed out
    -> Activate account
    -> Fake sign in

Account already active + signed out
    -> Skip activation
    -> Fake sign in

Account already active + already signed in
    -> Skip activation
    -> Skip fake sign in
```

Activation failures are also reported through a system notification.

## Download

Ready-to-use ELF builds are published in GitHub Releases.

[Latest Release](../../releases/latest)

## Requirements

- Jailbroken / exploited PS5
- A payload loader capable of executing PS5 ELF payloads

## Credits

The NP Fake Signin implementation is based on the `earthonion/np-fake-signin` project.

The account activation component is based on the tested EPINOR account activator implementation derived from etaHEN / PS5Dev OffAct.

- [earthonion/np-fake-signin](https://github.com/earthonion/np-fake-signin)
- [ps5-payload-dev/sdk](https://github.com/ps5-payload-dev/sdk)
- [etaHEN](https://github.com/etaHEN/etaHEN)

## License

See [LICENSE](LICENSE).

## Developer build

GitHub Actions fetches the PS5 Payload SDK and the upstream NP Fake Signin template data automatically.

```bash
export PS5_PAYLOAD_SDK=/path/to/ps5-payload-sdk
./tools/prepare_upstream.sh
make
```
