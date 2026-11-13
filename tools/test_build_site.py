"""build_site.py with a made-up program file (no game content)."""

import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_site


class BuildSite(unittest.TestCase):
    def test_site_alone(self):
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "site"
            build_site.build(out, None, "")
            self.assertTrue((out / "index.html").is_file())
            self.assertTrue((out / "js" / "zip.mjs").is_file())
            self.assertFalse((out / "release").exists())

    def test_site_with_a_release(self):
        with tempfile.TemporaryDirectory() as tmp:
            rel = Path(tmp) / "dist"
            rel.mkdir()
            (rel / "acww.3dsx").write_bytes(b"made up")
            out = Path(tmp) / "site"
            build_site.build(out, rel, "v0.0.1")
            self.assertEqual((out / "release" / "acww.3dsx").read_bytes(), b"made up")
            info = json.loads((out / "release" / "release.json").read_text())
            self.assertEqual(info, {"tag": "v0.0.1", "size": 7, "sha256": hashlib.sha256(b"made up").hexdigest()})

    def test_site_with_a_release_and_its_cia(self):
        with tempfile.TemporaryDirectory() as tmp:
            rel = Path(tmp) / "dist"
            rel.mkdir()
            (rel / "acww.3dsx").write_bytes(b"made up")
            (rel / "acww.cia").write_bytes(b"made-up cia")
            out = Path(tmp) / "site"
            build_site.build(out, rel, "v0.0.2")
            self.assertEqual((out / "release" / "acww.cia").read_bytes(), b"made-up cia")
            self.assertIs(json.loads((out / "release" / "release.json").read_text())["cia"], True)

    def test_missing_program_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(SystemExit):
                build_site.build(Path(tmp) / "site", Path(tmp), "v1")


if __name__ == "__main__":
    unittest.main()
