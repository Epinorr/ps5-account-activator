# Release checklist

1. Push the repository to GitHub with `.github/workflows/ps5.yml` at the repository root.
2. Run **Build Combined PS5 Payload** from Actions.
3. Download the generated `EPINOR-NP-Fake-Signin.elf` artifact.
4. Test the three expected states on a PS5:
   - inactive account + signed out
   - active account + signed out
   - active account + already signed in
5. After successful device testing, push a tag such as `v1.0.0`.
6. The same workflow will publish the ELF and SHA256 checksum to the GitHub Release.
