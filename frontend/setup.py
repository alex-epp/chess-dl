from cx_Freeze import setup, Executable

# Dependencies are automatically detected, but it might need
# fine tuning.
buildOptions = dict(
    packages=['pygame', 'numpy', 'numba'],
    excludes=[],
    include_files=['Assets/sprites.png', 'Assets/sprites.spritepack'])

import sys
base = 'Win32GUI' if sys.platform == 'win32' else None

executables = [
    Executable('main.py', base=base, targetName='chess.exe')
]



setup(name='chess',
      version='1.0',
      description='Chess engine with Python front-end and back-end',
      options=dict(build_exe=buildOptions),
      executables=executables)
