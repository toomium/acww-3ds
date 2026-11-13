# Release notes

Each version's notes, written before it's tagged: `docs/releases/<tag>.md`
(for example `v0.1.0.md`, or `v0.2.0-beta.1.md` for a pre-release).

When the tag is pushed, the release job (`.github/workflows/build.yml`) puts
the file at the top of the release's notes on GitHub. Without a file, the notes
list the pull requests merged since the last release. The release carries the
port's `acww.3dsx` and the HOME Menu version `acww.cia`; the web builder's site is published again with it.

Write for players: what's new, what was fixed, known issues, and what to
replace on the SD card.
