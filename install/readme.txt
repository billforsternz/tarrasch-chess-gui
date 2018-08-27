Instructions for building distributable packages.

1) Copy the release executable from .\vc_mswu\Tarrasch.exe to this directory.
2) Make sure tarrasch.iss is selecting the .tdb file you want to include
3) Make sure tarrasch,iss has the AppVerName right
4) Open tarrasch.iss with the (free, wonderful) innosetup program and use the
Build/Compile command from the menu. You will need a full complement of engines
in an Engines subdirectory beneath this one to avoid errors.
5) Once successful, rename the new setup.exe appropriately, eg to setup-tarrasch-v3.01a.exe.
6) Make sure file install.zip doesn't exist in this directory.
7) Select Tarrasch.exe, Tarrasch.ini (empty file), the .tdb file you want to include, book.pgn, and the
Web and Engines subdirectory
Right click and choose 7zip "Add to install.zip".
8) Rename install.zip appropriately, eg to portable-tarrasch-v3.01a.zip
