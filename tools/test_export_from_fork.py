"""Tests for export_from_fork.py, on a small made-up fork (no game content)."""

import contextlib
import io
import os
import subprocess
import sys
import tempfile
import unittest
import unittest.mock
from pathlib import Path

import export_from_fork as export


def git(cwd, *args):
    env = dict(os.environ, GIT_AUTHOR_NAME="t", GIT_AUTHOR_EMAIL="t@t", GIT_COMMITTER_NAME="t", GIT_COMMITTER_EMAIL="t@t")
    subprocess.run(["git", *args], cwd=cwd, check=True, capture_output=True, env=env)


def write(root, rel, text, mode=0o644):
    p = Path(root, rel)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(text)
    p.chmod(mode)


class ExportTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.fork = Path(self.tmp.name, "fork")
        self.out = Path(self.tmp.name, "out")
        self.out.mkdir()
        self.fork.mkdir()
        git(self.fork, "init", "-q", "-b", "main")
        write(self.fork, "src/main/a.c", "int a;\n")
        write(self.fork, "modern/host/hw.c", "int hw;\n")
        write(self.fork, "README.md", "upstream\n")
        git(self.fork, "add", "-A")
        git(self.fork, "commit", "-q", "-m", "base")
        git(self.fork, "branch", "base")
        git(self.fork, "checkout", "-q", "-b", "port")

    def commit(self):
        git(self.fork, "add", "-A")
        git(self.fork, "commit", "-q", "-m", "port")

    def run_export(self):
        argv = ["x", "--fork", str(self.fork), "--base", "base", "--branch", "port", "--out", str(self.out)]
        old = sys.argv
        sys.argv = argv
        try:
            with contextlib.redirect_stdout(io.StringIO()):
                export.main()
        finally:
            sys.argv = old

    def test_sorts_files_into_patches_and_port(self):
        write(self.fork, "src/main/a.c", "int a = 1;\n")
        write(self.fork, "modern/host/hw.c", "int hw = 1;\n")
        write(self.fork, "README.md", "ours\n")
        write(self.fork, "modern/host/pica.c", "int p;\n")
        write(self.fork, "tools/run.sh", "#!/bin/sh\n", 0o755)
        write(self.fork, "docs/images/s.png", "png")
        self.commit()
        self.run_export()
        patches = sorted(p.name for p in (self.out / "patches" / "acww-decomp").iterdir())
        self.assertEqual(patches, ["0001-runtime-core.patch", "0002-game-main.patch"])
        self.assertIn("diff --git a/src/main/a.c", (self.out / "patches/acww-decomp/0002-game-main.patch").read_text())
        self.assertTrue((self.out / "3ds_port/modern/host/pica.c").is_file())
        self.assertTrue(os.access(self.out / "3ds_port/tools/run.sh", os.X_OK))
        self.assertFalse((self.out / "docs").exists())  # (the fork's screenshots are not exported)
        self.assertFalse(list(self.out.rglob("README.md")))

    def test_unmatched_file_fails_and_writes_nothing(self):
        write(self.fork, "weird/file.c", "x\n")  # added: fine
        write(self.fork, "README.md", "ours\n")
        write(self.fork, "src/main/a.c", "int a = 2;\n")
        self.commit()
        git(self.fork, "checkout", "-q", "base")
        write(self.fork, "other/b.c", "b\n")
        git(self.fork, "add", "-A")
        git(self.fork, "commit", "-q", "-m", "b")
        git(self.fork, "branch", "base2")
        git(self.fork, "checkout", "-q", "port")
        write(self.fork, "other/b.c", "b2\n")
        self.commit()
        sys.argv = ["x", "--fork", str(self.fork), "--base", "base2", "--branch", "port", "--out", str(self.out)]
        with self.assertRaises(SystemExit) as cm, contextlib.redirect_stdout(io.StringIO()):
            export.main()
        self.assertIn("matches 0 areas", str(cm.exception))
        self.assertFalse((self.out / "patches").exists())

    def test_two_matching_areas_fail(self):
        with unittest.mock.patch.object(export, "AREAS", [("a", "A", ["src/*"]), ("b", "B", ["src/main/*"])]):
            with self.assertRaises(SystemExit) as cm:
                export.area_of("src/main/a.c")
        self.assertIn("matches 2 areas", str(cm.exception))

    def test_game_content_and_symlinks_refused(self):
        with self.assertRaises(SystemExit):
            export.plan([("A", "100644", "game.nds")])
        with self.assertRaises(SystemExit):
            export.plan([("A", "120000", "CLAUDE.md")])

    def test_every_area_pattern_is_disjoint_for_known_paths(self):
        for path in ["src/ov090/x.cpp", "src/main/x.cpp", "modern/host/gpu2d/gpu2d.c", "tools/configure_host.py"]:
            export.area_of(path)


if __name__ == "__main__":
    unittest.main()
