*Recently updated - Tarrasch is now a lot more developer friendly I hope*

Background
==========

The Tarrasch Chess GUI is a simple desktop chess workbench. Once installed on your computer
it allows you to create and edit .pgn files (i.e standard chess documents) and easily use
separate UCI (i.e. standard) chess engines. You can either play against a chess engine or use
it to analyse positions and games.

The Tarrasch Chess GUI Github binary releases are relatively compact "portable" (.zip, not setup .exe)
versions of Tarrasch without large databases. You can find out more about Tarrasch and download
differently configured packages at [the project's home website](http://triplehappy.com). 

Version information: This repository contains Tarrasch V3, From 2016-11-25 Tarrasch
V3 has replaced the retired Tarrasch V2. Tarrasch V2 uses the peer old-tarrasch-chess-gui
repository on github. My git skills are slowly getting up to speed and for future Tarrasch development I will
avoid this unnecessary proliferation of repositories - i.e. I will prefer further developing this
repository in to starting a new one. One reflection of my improving git skills
is that I have used tagged releases sensibly in this repository (in the peer old-tarrasch-chess-gui
repository I used more unwieldy branches). I now typically stick to two just branches, master and
development. I do all work in development, and then merge development into master at each
release.

To complete this version discussion; For the record, Tarrasch V1 was primitive but useful in some contexts. It predates any attempt
on my part to develop in the open with a hosted repository.

Building Tarrasch
=================

It is now easy to grab the Tarrasch source code, rebuild it and hack on it (at least
on Windows). This is a new development, as for the first 6 or more years of its life
I accepted that using wxWidgets inevitably resulted in a difficult barrier for new
developers. I recently (July-August 2016) took a fresh look at this as part of a long
delayed upgrade to wxWidgets V3 and it turns out it's not so hard.

Steps

1) Download and install a recent version of Visual Studio, with C++ support. I've tried
VS2010 and VS2015

2) Put this repository into a convenient working directory, eg
git clone https://github.com/billforsternz/tarrasch-chess-gui workdir

3) Just the one project file is required, it's called TarraschDb.vcxproj.
It works on VS2010 and VS2015. The project structure is now super lean,
mean and easy. There are a bunch of C++ source files (obviously) plus a
simple resource file and icon (actually two icon files now) in the src
directory. The project does assume the existance of a wxWidgets
directory using standard wxWidgets conventions. The directory should be
named wxWidgets-3.1.0 and it only needs to contain the include and lib
directories. Still this is a problem as these files collectively are
really too big to store in the repository, but....

4) I have prepared a .zip (well actually a .7z for extra space savings) that you
can download from my website. Just download, unzip into the right place with
7zip and you are ready to go. Download instructions a bit further down.

Note that if you get nothing but compile errors like this;

Cannot open include file: 'wx/wx.h': No such file or directory

That is a strong indication that you either ommitted step 4) or didn't unzip into
the right place in the directory hierarchy. It is really important to have a directory
wxWidgets-3.1.0 in your working directory and to have valid lib and include directories
within that.

Building Tarrasch on Linux and Mac
==================================

For the moment these are second class citizens unfortunately. In due course I hope
this changes. However you can certainly have a play if you are keen. I can't/won't
outline the steps required in detail, but basically;

1) Grab repository as above

2) Install wxWidgets and build one or more samples, in particular samples/richtext

3) Once you have samples/richtext working, just replace richtext.cpp with all the
source files in the repository src directory. Pay attention to file src/Portability.h
and make sure that THC_MAC or THC_LINUX are defined. (thc is a convenient prefix
it stands for triple happy chess - Triplehappy.com is the Tarrasch's home website)

wxWidgets Dependency
====================

To obtain a populated wxWidgets-3.1.0 directory on Windows. Either;

A) Download an already prepared installation in the form of a single compressed .7z (7zip) archive
from the project website, and uncompress it with 7zip. Two versions are available;
 [VS2010 version](http://triplehappy.com/downloads/wxWidgets-3.1.0-vs2010.7z) and
 [VS2015 version](http://triplehappy.com/downloads/wxWidgets-3.1.0-vs2015.7z)
These are the same except the completed libraries have been compiled with VS2010 in the first
and VS2015 in the second. Use the one corresponding to the version of Visual Studio you intend
to use.

B) Install wxWidgets from wxwidgets.org and follow the MSW (Microsoft Windows) instructions
in build/msw to build your own installation from scratch. Important: For each wxWidgets subproject
you must go to properties > configuration properties > C/C++ > code generation and change
/MD to /MT (release) and /MDd to /MTd (debug). This enables a fully static final executable.
Once you've successfully rebuilt wxWidgets (both debug and release) copy the include and lib
directory trees into place in the wxWidgets-3.1.0 directory.

The .7z archives only include the 25 wxWidgets libraries required by Tarrasch. These libraries
are all in the wxWidgets-3.1.0\lib\vc_lib subdirectory as follows;

- .\wxWidgets-3.1.0\lib\vc_lib\wxbase31u.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxbase31ud.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxbase31ud_xml.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxbase31u_xml.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxexpat.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxexpatd.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxjpeg.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxjpegd.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31ud_adv.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31ud_core.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31ud_html.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31ud_richtext.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31u_adv.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31u_core.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31u_html.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31u_richtext.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxmsw31u_aui.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxpng.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxpngd.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxregexu.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxregexud.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxtiff.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxtiffd.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxzlib.lib
- .\wxWidgets-3.1.0\lib\vc_lib\wxzlibd.lib

New in Tarrasch V3
==================

Tarrasch V3 seeks to add the following features to the existing Tarrasch V2

* Improved portability, including a working Mac version.
* Database features. Browse existing games that have reached a position of interest and explore move and transposition statistics.
* A tabbed workspace. An essential complement to the database features.
* Publish to .html (and possibly .rtf).
* Work with huge files, easily preview games before loading.

Bill Forster <billforsternz@gmail.com> Last update: 12Aug2016
