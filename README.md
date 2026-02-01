To do a release build: `python3 build.py`

To do a debug build: `python3 build.py --debug`

To do a clean release: `python3 build.py --clean`

To do a clean debug: `python3 build.py --clean --debug`

Depending on how python is installed, you may need to use `python` instead of `python3`.

The build script will run the executable immediately.

Tested on windows, ubuntu, fedora (headless), and arch (headless).

![Screenshot of world](/screenshot.png)

The actual world certainly isn't beautiful, but it is a showcase of various OpenGL skills, such as mesh instantiation, loading a mesh from a file vs through math, various light forms such as directional, point, and spot lights, dynamic mesh animation, and shadow mapping.