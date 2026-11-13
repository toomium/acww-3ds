"""Tests for tools/check_ci_artifacts.py (made-up workflow text)."""

import unittest

import check_ci_artifacts as c

GOOD = """
jobs:
  a:
    steps:
      - uses: actions/checkout@v4
      - name: Upload
        uses: actions/upload-artifact@v4
        with:
          name: release
          path: dist/release/
      - uses: actions/upload-pages-artifact@v3
        with:
          path: _site
"""

BAD = """
jobs:
  a:
    steps:
      - name: Upload
        uses: actions/upload-artifact@v4
        with:
          name: oops
          path: |
            dist/release/
            build/upstream/build/host3ds_usa/acww.3dsx
          retention-days: 3
"""


class CheckCiArtifacts(unittest.TestCase):
    def test_allowed(self):
        self.assertEqual(c.uploaded_paths(GOOD), ["dist/release/", "_site"])
        self.assertEqual(c.check("good.yml", GOOD), [])

    def test_not_allowed(self):
        self.assertEqual(c.check("bad.yml", BAD), ["bad.yml: uploads 'build/upstream/build/host3ds_usa/acww.3dsx'"])

    def test_no_path(self):
        text = "steps:\n  - uses: actions/upload-artifact@v4\n    with:\n      name: x\n"
        self.assertEqual(len(c.check("x.yml", text)), 1)


if __name__ == "__main__":
    unittest.main()
