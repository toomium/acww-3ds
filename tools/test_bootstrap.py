"""Tests for bootstrap.py, on a small made-up upstream (no game content)."""

import contextlib
import io
import os
import subprocess
import sys
import tempfile
import unittest
import unittest.mock
from pathlib import Path

import bootstrap


def git(cwd, *args, capture=False):
    env = dict(os.environ, GIT_AUTHOR_NAME="t", GIT_AUTHOR_EMAIL="t@t", GIT_COMMITTER_NAME="t", GIT_COMMITTER_EMAIL="t@t")
    r = subprocess.run(["git", *args], cwd=cwd, check=True, capture_output=True, text=True, env=env)
    return r.stdout.strip()


class BootstrapTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        t = Path(self.tmp.name)
        self.up = t / "up"
        self.up.mkdir()
        git(self.up, "init", "-q", "-b", "main")
        (self.up / "a.c").write_text("one\ntwo\nthree\n")
        git(self.up, "add", "-A")
        git(self.up, "commit", "-q", "-m", "base")
        self.commit = git(self.up, "rev-parse", "HEAD")

        self.root = t / "repo"
        (self.root / "patches/acww-decomp").mkdir(parents=True)
        (self.root / "3ds_port/sub").mkdir(parents=True)
        (self.root / "3ds_port/sub/new.c").write_text("ours\n")
        (self.root / "upstream.lock").write_text(f"# test\nurl=file:///nonexistent\ncommit={self.commit}\n")
        (self.up / "a.c").write_text("one\nTWO\nthree\n")
        patch = subprocess.run(["git", "diff"], cwd=self.up, capture_output=True, text=True).stdout
        (self.root / "patches/acww-decomp/0001-x.patch").write_text("Subject: x\n\n" + patch)
        git(self.up, "checkout", "--", "a.c")

        for name, value in [("ROOT", self.root), ("LOCK_FILE", self.root / "upstream.lock"),
                            ("UPSTREAM_DIR", self.root / "build/upstream"), ("PATCH_DIR", self.root / "patches/acww-decomp"),
                            ("PORT_DIR", self.root / "3ds_port")]:
            patcher = unittest.mock.patch.object(bootstrap, name, value)
            patcher.start()
            self.addCleanup(patcher.stop)

    def run_main(self, *args):
        old = sys.argv
        sys.argv = ["bootstrap.py", *args]
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                bootstrap.main()
        finally:
            sys.argv = old
        return out.getvalue()

    def test_fetch_patch_link(self):
        self.run_main("--source", str(self.up))
        up = self.root / "build/upstream"
        self.assertEqual((up / "a.c").read_text(), "one\nTWO\nthree\n")
        link = up / "sub/new.c"
        self.assertTrue(link.is_symlink())
        self.assertFalse(os.path.isabs(os.readlink(link)))
        self.assertEqual(link.read_text(), "ours\n")
        self.assertEqual(git(up, "status", "--porcelain"), "M a.c")  # the links are excluded

    def test_second_run_refreshes_links_only(self):
        self.run_main("--source", str(self.up))
        (self.root / "3ds_port/more.c").write_text("more\n")
        (self.root / "3ds_port/sub/new.c").unlink()
        out = self.run_main("--source", str(self.up))
        self.assertIn("already at", out)
        up = self.root / "build/upstream"
        self.assertTrue((up / "more.c").is_symlink())
        self.assertFalse(os.path.lexists(up / "sub/new.c"))
        self.assertEqual((up / "a.c").read_text(), "one\nTWO\nthree\n")

    def test_clean_starts_over(self):
        self.run_main("--source", str(self.up))
        (self.root / "build/upstream/a.c").write_text("edited\n")
        self.run_main("--clean", "--source", str(self.up))
        self.assertEqual((self.root / "build/upstream/a.c").read_text(), "one\nTWO\nthree\n")

    def test_bad_patch_names_the_patch(self):
        (self.root / "patches/acww-decomp/0002-bad.patch").write_text(
            "diff --git a/a.c b/a.c\n--- a/a.c\n+++ b/a.c\n@@ -1,1 +1,1 @@\n-nothing\n+x\n")
        with self.assertRaises(SystemExit) as cm:
            self.run_main("--source", str(self.up))
        self.assertIn("0002-bad.patch", str(cm.exception))

    def test_wrong_commit_is_refused(self):
        (self.root / "upstream.lock").write_text(f"url=x\ncommit={'0' * 40}\n")
        with self.assertRaises(SystemExit):
            self.run_main("--source", str(self.up))

    def test_port_file_that_exists_upstream_is_refused(self):
        (self.root / "3ds_port/a.c").write_text("clash\n")
        with self.assertRaises(SystemExit) as cm:
            self.run_main("--source", str(self.up))
        self.assertIn("a.c", str(cm.exception))


if __name__ == "__main__":
    unittest.main()
