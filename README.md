Background
==========

The Tarrasch Chess GUI is a simple desktop chess workbench. Once installed on your computer
it allows you to create and edit .pgn files (i.e standard chess documents) and easily use
separate UCI (i.e. standard) chess engines. You can either play against a chess engine or use
it to analyse positions and games.

Tarrasch has been around for a few years. Tarrasch V1 was primitive but useful in some
contexts. Tarrasch V2 was much more complete and useful. You can find out more about Tarrasch
and download Tarrasch V2 for Windows at this [website](http://triplehappy.com). Tarrasch
V2 is also an open-source project, although the code is not currently on github.

At the time of writing Tarrasch V3 is under development. I have been working on it
privately for some time, but now I am putting the working repository on Github and the
plan is to develop in the open and keep the repository here updated regularly.

Features
========

Tarrasch V3 seeks to add the following features to the existing Tarrasch V2

* Improved portability, including a working Mac version.
* Database features. Browse existing games that have reached a position of interest and explore move and transposition statistics.
* A tabbed workspace. An essential complement to the database features.
* Publish to .html (and possibly .rtf).
* Work with huge files, easily preview games before loading.

Status
======

As I write this, I am preparing to upload an initial commit that partially implements each of
key features above. However nothing works flawlessly yet, and some things that worked well in
Tarrasch V2 are currently broken. As well as incomplete code, there are other things typical
of work in progress - for example hardwired paths and filenames that should be preferences.

I am using a Mac for Tarrasch V3 development, even though some
things from V2 haven't made it into the Mac version (yet). For example the simple "book" feature
of V2. I chose to use the V3 development as an exercise in learning to use my Mac for programming.
My first step was to get V2 working, but I never got everything working perfectly before moving
onto the V3 features, so I don't have a practical V2 for Mac.

I am using C++ and wxWidgets as a GUI library. There is now a production ready wxWidgets Cocoa
release for Mac (V3.0) and I am using that. People with an interest in the Tarrasch source code
have always struggled most with the annoying wxWidgets dependency. I am not going to apologise
for that as without wxWidgets I wouldn't have a practical chess program! Having said that, I
should check for other github projects that use wxWidgets and find out what good practice is for
incorporating wxWidgets. In the meantime, my practice is to leave it up to the user to figure
out how to download wxWidgets and build a sample project. Once you can do that, replacing the
sample application code with Tarrasch code shouldn't be too onerous.

Apart from the dependency on wxWidgets Tarrasch should be like any other Xcode 5 C++ project.
I do find on Mac everything tends to be very version dependent. When I upgraded from Lion to
Mavericks I also had to upgrade from Xcode 4 to 5, and it took me a while to get things going
again. The github project includes all the ancillary and boilerplate files characteristic of
a wxWidgets cocoa application on Mac.

Bill Forster <billforsternz@gmail.com> 8Feb2014
