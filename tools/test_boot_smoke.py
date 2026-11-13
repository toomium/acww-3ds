"""Tests for boot_smoke.py: the decision logic, with a fake program (no game needed)."""

import stat
import tempfile
import unittest
from pathlib import Path

import boot_smoke

START = "card: the ROM image held in memory\n"


class ClassifyTest(unittest.TestCase):
    def test_first_file_missing_passes(self):
        ok, what = boot_smoke.classify(2, START + "host: game panic: File can't open. [/str/bsize.arc] (:0)\nhost: fatal: panic (frame 1)\n")
        self.assertTrue(ok)
        self.assertIn("/str/bsize.arc", what)

    def test_frame_limit_passes(self):
        self.assertTrue(boot_smoke.classify(0, START + "host: exit at frame 60 (ACWW_EXIT_FRAME)\n")[0])

    def test_failures(self):
        crash = START + "host: signal 11 (Segmentation fault) at frame 1\n"
        self.assertIn("signal 11", boot_smoke.classify(5, crash)[1])
        for code, log in [
            (5, crash),                                                    # the runtime's crash handler
            (-11, crash),                                                  # death by signal
            (None, START),                                                 # timeout
            (1, "usage: acww [-v] <rom.nds>\n"),                           # the ROM wasn't read
            (2, START + "host: game panic: something else (:0)\n"),        # another panic
            (0, START),                                                    # ended without the frame limit
        ]:
            self.assertFalse(boot_smoke.classify(code, log)[0], (code, log))


class RunTest(unittest.TestCase):
    def fake(self, body):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        p = Path(tmp.name) / "acww"
        p.write_text("#!/bin/sh\n" + body)
        p.chmod(p.stat().st_mode | stat.S_IXUSR)
        return str(p)

    def test_fake_programs(self):
        self.assertTrue(boot_smoke.run(self.fake("echo 'card: the ROM image x' >&2\necho \"host: game panic: File can't open. [/a] (:0)\" >&2\nexit 2\n"))[0])
        self.assertFalse(boot_smoke.run(self.fake("kill -SEGV $$\n"))[0])
        old, boot_smoke.TIMEOUT = boot_smoke.TIMEOUT, 1
        self.addCleanup(setattr, boot_smoke, "TIMEOUT", old)
        self.assertFalse(boot_smoke.run(self.fake("exec sleep 5\n"))[0])

    def test_rom_is_made_up(self):
        rom = boot_smoke.make_rom()
        self.assertEqual(len(rom), 0x10000)
        self.assertEqual(rom[0:12], bytes(12))   # no title, no game code


if __name__ == "__main__":
    unittest.main()
