REM Repopulate minimal wx lib directory for Tarrasch from full installation
REM Run this from base project directory
mkdir wxWidgets-3.1.0\lib
mkdir wxWidgets-3.1.0\lib\vc_lib
mkdir wxWidgets-3.1.0\lib\vc_lib\mswu
mkdir wxWidgets-3.1.0\lib\vc_lib\mswu\wx
mkdir wxWidgets-3.1.0\lib\vc_lib\mswu\wx\msw
mkdir wxWidgets-3.1.0\lib\vc_lib\mswud
mkdir wxWidgets-3.1.0\lib\vc_lib\mswud\wx
mkdir wxWidgets-3.1.0\lib\vc_lib\mswud\wx\msw
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31u.lib                 wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31u.pdb                 wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31ud.lib                wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31ud.pdb                wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31ud_xml.lib            wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31ud_xml.pdb            wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31u_xml.lib             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxbase31u_xml.pdb             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxexpat.lib                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxexpat.pdb                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxexpatd.lib                  wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxexpatd.pdb                  wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxjpeg.lib                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxjpeg.pdb                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxjpegd.lib                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxjpegd.pdb                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_adv.lib             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_adv.pdb             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_aui.lib             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_aui.pdb             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_core.lib            wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_core.pdb            wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_html.lib            wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_html.pdb            wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_richtext.lib        wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31ud_richtext.pdb        wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_adv.lib              wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_adv.pdb              wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_aui.lib              wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_aui.pdb              wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_core.lib             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_core.pdb             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_html.lib             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_html.pdb             wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_richtext.lib         wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxmsw31u_richtext.pdb         wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxpng.lib                     wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxpng.pdb                     wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxpngd.lib                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxpngd.pdb                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxregexu.lib                  wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxregexu.pdb                  wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxregexud.lib                 wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxregexud.pdb                 wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxtiff.lib                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxtiff.pdb                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxtiffd.lib                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxtiffd.pdb                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxzlib.lib                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxzlib.pdb                    wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxzlibd.lib                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\wxzlibd.pdb                   wxWidgets-3.1.0\lib\vc_lib
copy wxWidgets-3.1.0-full\lib\vc_lib\mswu\build.cfg                wxWidgets-3.1.0\lib\vc_lib\mswu             
copy wxWidgets-3.1.0-full\lib\vc_lib\mswu\wx\setup.h               wxWidgets-3.1.0\lib\vc_lib\mswu\wx
copy wxWidgets-3.1.0-full\lib\vc_lib\mswu\wx\msw\rcdefs.h          wxWidgets-3.1.0\lib\vc_lib\mswu\wx\msw
copy wxWidgets-3.1.0-full\lib\vc_lib\mswud\build.cfg               wxWidgets-3.1.0\lib\vc_lib\mswud
copy wxWidgets-3.1.0-full\lib\vc_lib\mswud\wx\setup.h              wxWidgets-3.1.0\lib\vc_lib\mswud\wx
copy wxWidgets-3.1.0-full\lib\vc_lib\mswud\wx\msw\rcdefs.h         wxWidgets-3.1.0\lib\vc_lib\mswud\wx\msw
