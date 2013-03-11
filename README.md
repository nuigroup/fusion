## Design Goals
Fusion is an open source, modular, cross-platform interaction framework
designed to rapidly prototype and develop multi-modal application interfaces. It seeks to expose an easy-to-consume API for user input in multiple simultaneous input modes. Initially, we provide modules for audio input to be recognized by a grammar and Sphinx ASR, and touch/mouse input.  See [Put That There 1](http://www.youtube.com/watch?v=RyBEUyEtxQo) and [Put That There 2](http://www.youtube.com/watch?v=0Pr2KIPQOKE) for an example of multimodal interaction.

See the [Fusion page](http://ccf.nuigroup.com/) at the NUI Group's website for a video of Fusion's demo in action. 

This project was initially written for the Natural User Interface Group as part of Google Summer of Code 2011.

## Features
* __Cross-Platform__
Fusion is fully cross-platform. It runs on Windows, Linux, MacOSX and FreeBSD.
The same configurations and models apply across platforms, so if your application is
cross-platform, we support it.

* __Daemon__
Fusion runs as a daemon process. That means you can
launch it on a different headless computer if you want (although currently some things
depend on the daemon running on localhost, and responsiveness could be an issue).

* __Threading__
With Fusion, you have the ability to enable threading for any module you want.
That means you are finally able to use your Core 2 Duo / Quad Core to its full
potential. Obviously, you can still use non-threaded mode.

* __Remote API__
Fusion has a JSON API to control all its settings and adjust your pipeline remotely
and on the fly. Interaction events are also served as JSON objects.

* __HTML5 Administration__
Fusion's web interface makes use of the most recent developments in web
programming with open standards like HTML5. All that is required is a compatible
HTML5 web browser. As of today, Firefox 3.6, Safari and Chromium have successfully
run the web interface.

## Building
Fusion currently runs on Windows 7/8, OS X 10.6+, and Linux (tested on Ubuntu 11.04+).  It currently uses the SCons tool to build on all platforms.

This means you have to install SCons to be able to compile Fusion.
The platform specific compile notes below include instruction on how to install
SCons. Once you have SCons installed open a command line in the Fusion directory
(the one with this README in it) and run the following command:

To build fusion (and dependencies if needed) run:
scons

To clean fusion (remove any intermediate files, output libs and executables):
scons -c  (or 'scons --clean')

To also clean the dependencies built in contrib:
scons -c --clean-contrib

For more info about SCons, other options, or how to install it on any system,
you can check the SCons user guide at:
http://www.scons.org/doc/production/HTML/scons-user.html

### Windows 7/8
To build Fusion on Windows, you need Visual C++ 2010 and SCons (which also requires Python).
1. Get Visual C++ Express 2010 (not needed if you have Visual Studio 2010). Download the free version here: http://www.microsoft.com/express/Downloads/#2010-Visual-CPP

2. Get Python and SCons. Download and run the following installers:
[Python](http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi)
[SCons](http://prdownloads.sourceforge.net/scons/scons-2.0.1.win32.exe)

OK, you're ready to build.  If you installed everything to default locations just run: `scons`

If scons command is not found, try `C:\Python2.7\Scripts\scons`

Now you should be able to start fusion. Just double click the fusion executable or run it from the command line to use a config:
```fusion -l config/ballworld/ballworld.cfg```

Now that the daemon is running, you can simply open your browser
(needs to be a browser with HTML5 and support for
the canvas object. Javascript also has to be enabled.) and navigate to the admin interface at http://127.0.0.1:7500/.

### OS X and Linux
On OS X and Linux, Fusion is also built with SCons (http://www.scons.org).

1. Download [SCons](http://prdownloads.sourceforge.net/scons/scons-2.0.1.zip)
2. Unzip the file, open a terminal, and navigate to that directory:
```cd scons-2.0.1```
```python setup.py install```

Now you're ready to build fusion.  Change directory to the Fusion sources.

1. First, you need to build rtaudio. This can take some configuration, so check out `contrib/rtaudio/install`. Most times, though, you should just be able to build with `cd contrib/rtaudio && ./configure && make`.
2. Next, go back to the root of the project and `scons`. This will (hopefully) build everything. Note: If you need to build fusion as 32bit, you can do so by forcing the -m32 flag on the compilers for both moved and all libs in contrib by running:
```CC='gcc -m32' CXX='g++ -m32' scons```
3. Run `./ccf`, and navigate to http://127.0.0.1:7500/ with a Canvas/HTML5/JS-enabled browser for configuration. Exit the server with `curl 127.0.0.1:7500/pipeline/quit`.
4. Leave the config as-is and run the platform-specific Python file in `demo` for a quick demo of Fusion's functionality.

## Documentation
### OS X or Linux:
1. Make sure you have Doxygen installed 
2. cd docs
3. make

### Windows:
1. Run Doxygen on the Doxyfile in docs/
