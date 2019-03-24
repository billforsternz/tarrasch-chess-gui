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
repository I used more unwieldy branches). I now typically use just two branches, master and releases.
I do all work in master, and then merge master into releases at each release (so the releases
branch reflects the most recent release).

To complete this version discussion; For the record, Tarrasch V1 was primitive but useful in some contexts. It predates any attempt
on my part to develop in the open with a hosted repository.

Building Tarrasch
=================

It is now easy (since 2016 when I changed to wxWidgets V3 and grabbed the wxWidgets
dependency problem by the horns) to grab the Tarrasch source code, rebuild it and hack on it (at least
on Windows and Linux).

Steps

1) Download and install a recent version of Visual Studio, with C++ support. I've tried
VS2010, VS2015 and VS2017

2) Put this repository into a convenient working directory, eg
git clone https://github.com/billforsternz/tarrasch-chess-gui workdir

3) Just the one project file is required, it's called Tarrasch.vcxproj.
It works on VS2010, VS2015 and VS2017. The project structure is now super lean,
mean and easy. There are a bunch of C++ source files (obviously) plus a
simple resource file and icon (actually two icon files now) in the src
directory. The project does assume the existance of a wxWidgets
directory using standard wxWidgets conventions. The directory should be
named wxWidgets (note: until March 2019 it was wxWidgets-3.1.0 but this
was unnecessarily inflexible) and it only needs to contain the include and lib
directories. Still this is a problem as these files collectively are
really too big to store in the repository, but....

4) I have prepared a .zip (well actually a .7z for extra space savings) that you
can download from my website. Just download, unzip into the right place with
7zip and you are ready to go. Download instructions a bit further down.

Note that if you get nothing but compile errors like this;

Cannot open include file: 'wx/wx.h': No such file or directory

That is a strong indication that you either ommitted step 4) or didn't unzip into
the right place in the directory hierarchy. It is really important to have a directory
wxWidgets (not wxWidgets-3.1.0 any more! change made with Tarrasch V3.12) in your
working directory and to have valid lib and include directories within that.

Building Tarrasch on Linux and Mac
==================================

Note: Recently (late October 2017) as the result of some great work by John
Cheetham, Tarrasch on Linux has moved forward significantly. To try this out on
Ubuntu 16.04 LTS (say), perform the following steps then simply make in the top
level directory;

- sudo apt install libwxgtk3.0-dev
- sudo apt install libwxgtk-media3.0-dev
- sudo apt install libwxgtk-webview3.0-dev
- wx-config --version
 
 (should return 3.0.2)

Look for John's merged pull request for further discussion.

The following comments are still relevant, but predate this recent development.

For the moment these are second class citizens unfortunately. In due course I hope
this changes. However you can certainly have a play if you are keen. I can't/won't
outline the steps required in detail, but basically;

1) Grab repository as above

2) Install wxWidgets and build one or more samples, in particular samples/richtext
and samples/aui

3) Once you have samples/richtext working, just replace richtext.cpp with all the
source files in the repository src directory. Pay attention to file src/Portability.h
and make sure that THC_MAC or THC_LINUX are defined. (thc is a convenient prefix
it stands for triple happy chess - Triplehappy.com is the Tarrasch's home website)

wxWidgets Dependency
====================

For Linux see previous section.

For Windows either;

A) Download an already prepared installation in the form of a single compressed .7z (7zip) archive
from the project website, and uncompress it with 7zip. Three versions are now available;
 [VS2010 version (wxWidgets-3.1.0)](http://triplehappy.com/downloads/wxWidgets-3.1.0-vs2010.7z),
 [VS2015 version (wxWidgets-3.1.0)](http://triplehappy.com/downloads/wxWidgets-3.1.0-vs2015.7z) and
 [VS2017 version (wxWidgets-3.1.2)](http://triplehappy.com/downloads/wxWidgets-3.1.2-vs2017.7z)

Use the version corresponding to your compiler. Note that the VS2017 version is a more recent
version of wxWidgets. Originally I only had the VS2010 and VS2015 versions, when I added the
VS2017 version I changed to the more normal wxWidgets/ subdirectory convention within the
project directory instead of my old and inflexible wxWidgets-3.1.0/ conventions. Once you have
unzipped with 7zip, move the lib and include directories into place in the project's wxWidgets/
subdirectory.

When I started using VS2017 I originally created a new project file TarraschVS2017.vcxproj
but at the time of releasing Tarrasch V3.12 I updated the original Tarraschc.vcxproj to
support VS2017 as well, a better solution I think.

B) Install wxWidgets from wxwidgets.org and follow the MSW (Microsoft Windows) instructions
in build/msw to build your own installation from scratch. It's pretty easy, eg for VS2017
load up wx_vc15.sln. Important: Before rebuilding the solution for release and debug you
must individually configure each wxWidgets subproject using properties > configuration properties > C/C++ > code generation and change
/MD to /MT (release) and /MDd to /MTd (debug). This enables a fully static final executable.
It's a bit of a pain, but Visual Studio makes life a little easier by going straight to the
right configuration dialog as you double click each subproject (don't close the configuration
dialog).

Once you've successfully rebuilt wxWidgets (both debug and release) copy the include and lib
directory trees into place in the wxWidgets/ project subdirectory.

The .7z archives only include the 26 wxWidgets libraries required by Tarrasch. These libraries
are all in the wxWidgets\lib\vc_lib subdirectory as follows;

- wxWidgets\lib\vc_lib\wxbase31u.lib
- wxWidgets\lib\vc_lib\wxbase31ud.lib
- wxWidgets\lib\vc_lib\wxbase31ud_xml.lib
- wxWidgets\lib\vc_lib\wxbase31u_xml.lib
- wxWidgets\lib\vc_lib\wxexpat.lib
- wxWidgets\lib\vc_lib\wxexpatd.lib
- wxWidgets\lib\vc_lib\wxjpeg.lib
- wxWidgets\lib\vc_lib\wxjpegd.lib
- wxWidgets\lib\vc_lib\wxmsw31ud_adv.lib
- wxWidgets\lib\vc_lib\wxmsw31ud_core.lib
- wxWidgets\lib\vc_lib\wxmsw31ud_html.lib
- wxWidgets\lib\vc_lib\wxmsw31ud_richtext.lib
- wxWidgets\lib\vc_lib\wxmsw31ud_aui.lib
- wxWidgets\lib\vc_lib\wxmsw31u_adv.lib
- wxWidgets\lib\vc_lib\wxmsw31u_core.lib
- wxWidgets\lib\vc_lib\wxmsw31u_html.lib
- wxWidgets\lib\vc_lib\wxmsw31u_richtext.lib
- wxWidgets\lib\vc_lib\wxmsw31u_aui.lib
- wxWidgets\lib\vc_lib\wxpng.lib
- wxWidgets\lib\vc_lib\wxpngd.lib
- wxWidgets\lib\vc_lib\wxregexu.lib
- wxWidgets\lib\vc_lib\wxregexud.lib
- wxWidgets\lib\vc_lib\wxtiff.lib
- wxWidgets\lib\vc_lib\wxtiffd.lib
- wxWidgets\lib\vc_lib\wxzlib.lib
- wxWidgets\lib\vc_lib\wxzlibd.lib


Bill Forster <billforsternz@gmail.com>
