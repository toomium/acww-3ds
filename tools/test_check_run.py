"""check_run.py's logic with made-up hash files (no game content, no run)."""

import contextlib
import io
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check_run


class Compare(unittest.TestCase):
    def test_identical(self):
        self.assertEqual(check_run.compare({"a": "1", "b": "2"}, {"b": "2", "a": "1"}), [])

    def test_differences(self):
        out = check_run.compare({"a": "1", "b": "2", "c": "3"}, {"a": "9", "b": "2", "d": "4"})
        self.assertEqual(out, ["DIFFERS: a", "MISSING in the run: c", "NOT in the baseline: d"])

    def test_read_hashes(self):
        with tempfile.TemporaryDirectory() as tmp:
            f = Path(tmp) / "h.txt"
            f.write_text("abc  shot-1.ppm\ndef  game.sav\n")
            self.assertEqual(check_run.read_hashes(f), {"shot-1.ppm": "abc", "game.sav": "def"})


class Main(unittest.TestCase):
    def call(self, args, **patches):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            try:
                code = check_run.main(["check_run.py", *args])
            except SystemExit as e:
                code = str(e)
        return code, out.getvalue()

    def test_errors(self):
        self.assertIn("no ROM given", self.call([])[0])
        self.assertIn("no ROM given", self.call([""])[0])
        self.assertIn("ROM file not found", self.call(["/nonexistent.nds"])[0])
        with tempfile.TemporaryDirectory() as tmp:
            rom = Path(tmp) / "r.nds"
            rom.write_bytes(b"x")
            with mock.patch.object(check_run, "BINARY", Path(tmp) / "none"):
                self.assertIn("PC build missing", self.call([str(rom)])[0])

    def test_baseline_and_compare(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            rom = tmp / "r.nds"
            rom.write_bytes(b"x")
            (tmp / "acww").write_bytes(b"")
            hashes = [{"a": "1"}, {"a": "1"}, {"a": "1"}, {"a": "2"}]
            with mock.patch.object(check_run, "BINARY", tmp / "acww"), \
                 mock.patch.object(check_run, "CHECK", tmp / "check"), \
                 mock.patch.object(check_run, "BASELINE", tmp / "check" / "baseline.txt"), \
                 mock.patch.object(check_run, "run", side_effect=lambda *a: (tmp / "check").mkdir(exist_ok=True) or hashes.pop(0)):
                code, out = self.call([str(rom)])
                self.assertEqual(code, 1)
                self.assertIn("make check-baseline", out)
                self.assertEqual(self.call(["--baseline", str(rom)])[0], 0)
                code, out = self.call([str(rom)])
                self.assertEqual((code, out), (0, "No differences from the baseline.\n"))
                code, out = self.call([str(rom)])
                self.assertEqual(code, 1)
                self.assertIn("DIFFERS: a", out)


if __name__ == "__main__":
    unittest.main()
