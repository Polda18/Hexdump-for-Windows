# Hexdump for Windows
_COMING SOON_

A `hexdump` tool for Windows, known from Linux bash. This repository, once finished,
will hold source code for a port of a well known Linux terminal tool called `hexdump`
for Windows. For simplicity, I stripped down `stdin` input for data, only accepting
files, and stripped down functions for custom format, as I personally don't use them
and most people do not need those features, I believe. Plus I would need to learn
more about these features anyway before implementing them in the Windows port. The
programming language of my choice for this project is C for the sake of practice
in low level programming. I also added `-h` switch to replace original Linux manual
page listing for `man hexdump`, which doesn't exist on Windows, or I would have to
create the `man` page separately in order for the Powershell `man` cmdlet to work
with `hexdump`. I recreate the entire tool from scratch, not looking into the original
source code, at least for version `1` of this tool because features I want to implement
are fairly simple and shouldn't be much of an issue to get working properly without
a source code reference. I might end up asking for help, but currently, I'm set off
and working on the basic parts.

If you want full feature port, let me know and I'll set them up for version `2`. Versions
`1.X` will be intended for bugs hunting if any bugs will be found, I also plan releasing
snapshot versions before version `1` (versions `0.X`) purely for you to test the tool
before an official release is hit, and help me find most important bugs before the tool
is ready for an official launch.

I'll also try and suggest Microsoft to include this tool in the system for future releases.
I think that tool might be super useful for developers that rely on Windows platform and
cannot for some reason access Linux tools (not even from WSL). `hexdump` is essential
tool for debugging and testing. I think that it should be included in Windows, alongside
with a `hexedit` tool, which I guess might be subject for future project as well...
