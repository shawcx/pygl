import os
import subprocess
import sys

import pytest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def test_shaders_example_renders(tmp_path):
    pytest.importorskip('SDL2')

    env = dict(os.environ, PYGL_FRAMES='3', PYTHONPATH=ROOT)
    proc = subprocess.run([sys.executable, os.path.join(ROOT, 'example', 'shaders.py')],
                          cwd=tmp_path, env=env, capture_output=True, text=True,
                          timeout=60)
    if proc.returncode != 0:
        if 'video' in proc.stderr.lower() or 'display' in proc.stderr.lower():
            pytest.skip('no display for SDL2')
        pytest.fail(proc.stderr or proc.stdout)

    shot = tmp_path / 'pygl_screenshot.ppm'
    assert shot.exists()
    assert shot.read_bytes().startswith(b'P6\n800 600\n255\n')
    assert 'program interface:' in proc.stdout
