This directory contains sufficient resources for a motivated and
resourceful user to work out how to use Tarrasch V3 to publish
interactive chess content for the web.

First, use the "Publish" button  in the pgn games dialog to generate a
markdown (file extension .md) file. Innumerable content management
systems, static site generators, and other web tools can process
markdown files and turn them into publishable .html documents. I use a
very obscure (but nice and simple) tool called "Poole" to publish the
tournament reports for my local club. For example output see
http://www.nzchessmag.com/wellingtonchessclub/2015-mazur-g.html.

A similar, but more recent example, with some refined javascript
(eg arrow keys supported, and onscreen arrows are more dynamic)
http://newzealandchess.co.nz/nzchessmag/interactive/1979-12.html

My approach to interactive chess content is different to most (as far as
I know it is unique). My basic idea is to make the output look like a
chess book, but when users click on moves the diagrams in the chess book
become dynamic and interactive. Obviously you won't want to use this web
publishing system unless you agree with me that this is a good approach!

I have included a very simple example.pgn, the resulting example.md and
a minimal example.html.  Note that the conversion process from markdown
to html needs to add references to the three javascript (file extension
.js) programs in this directory. Tools like Poole let you do this
automatically using a technique called templating. Also note that you'll
also need to include the png (graphics) subdirectory on the webserver.

I am not going to describe the templating process for Poole or any other
tool here - that's why you need to be motivated and resourceful! Suffice
to say there are a near infinite number of tools, blog posts, tutorials
etc. on the web that cover this kind of templating. To be clear, these
resources are generic, not chess specific - but the techniques involved
are directly applicable to converting Tarrasch markdown output to
appropriate html and publishing it on the web.

Finally, note that you need to include at least one diagram in each game
for my web publishing system to work. Indicate a diagram with comment
text "#Diagram" at appropriate points in the .pgn. Good luck!

Bill Forster 2015-12-11, updated 2019-01-14
