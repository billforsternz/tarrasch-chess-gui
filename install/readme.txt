14.42.34433
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

Source: "Tarrasch.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "wxbase32u_vc14x_x64.dll"; Flags: ignoreversion
Source: "wxmsw32u_core_vc14x_x64.dll"; Flags: ignoreversion
Source: "wxmsw32u_html_vc14x_x64.dll"; Flags: ignoreversion
Source: "wxbase32u_xml_vc14x_x64.dll"; Flags: ignoreversion
Source: "wxmsw32u_aui_vc14x_x64.dll"; Flags: ignoreversion
Source: "wxmsw32u_richtext_vc14x_x64.dll"; Flags: ignoreversion
Source: "msvcp140.dll"; Flags: ignoreversion
Source: "msvcp140_1.dll"; Flags: ignoreversion
Source: "msvcp140_2.dll"; Flags: ignoreversion
Source: "msvcp140_atomic_wait.dll"; Flags: ignoreversion
Source: "msvcp140_codecvt_ids.dll"; Flags: ignoreversion
Source: "vcruntime140.dll"; Flags: ignoreversion
Source: "vcruntime140_1.dll"; Flags: ignoreversion
Source: "vcruntime140_threads.dll"; Flags: ignoreversion
Source: "Engines\791556.pb.gz"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\lc0.exe"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\libopenblas.dll"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\mimalloc-override.dll"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\mimalloc-redirect.dll"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\stockfish-windows-x86-64-sse41-popcnt.exe"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\TarraschToyEngine.exe"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "Engines\Sargon-1978.exe"; DestDir: "{app}\Engines"; Flags: ignoreversion
Source: "book.pgn"; DestDir: "{app}"; Flags: ignoreversion
Source: "book.pgn_compiled"; DestDir: "{app}"; Flags: ignoreversion
Source: "great-players-demo.tdb"; DestDir: "{app}"; Flags: ignoreversion
Source: "web.zip"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
