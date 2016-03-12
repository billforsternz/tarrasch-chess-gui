Instructions for building distributable packages.

1) Copy the release executable from .\vc_msw\Tarrasch.exe to TarraschDb.exe in
this directory.
2) Copy the default database you want to ship to default.tdb in this directory.
2) Open tarrasch.iss with the (free, wonderful) innosetup program and use the
Build/Compile command from the menu. You will need a full complement of engines
in an Engines subdirectory beneath this one to avoid errors.
3) Once successful, rename the new setup.exe appropriately, eg to setup-tarrasch-v2.03b.exe.
4) Make sure file install.zip doesn't exist in this directory.
5) Select Tarrasch.exe, Tarrasch.ini (empty file), book.pgn, and the Engines subdirectory
Right click and choose 7zip "Add to install.zip".
6) Rename install.zip appropriately, eg to portable-tarrasch-v2.03b.zip.
