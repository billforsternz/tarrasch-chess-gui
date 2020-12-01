/****************************************************************************
 * Top level app file for Tarrasch chess GUI
 *  Similar to boilerplate app code for wxWidgets example apps
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "Portability.h"
#ifdef THC_WINDOWS
#include <windows.h>    // Windows headers for RedirectIoToConsole()
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <time.h>
#endif
#include <locale>
#include "wx/wx.h"
#include "wx/intl.h"
#include "wx/file.h"
#include "wx/listctrl.h"
#include "wx/clipbrd.h"
#include "wx/sysopt.h"
#include "wx/log.h"
#include "wx/filehistory.h"
#include "wx/snglinst.h"
#include "wx/ipc.h"
#include "wx/aui/aui.h"
#ifdef AUI_NOTEBOOK
#include "wx/aui/auibook.h"
#else
#include "wx/notebook.h"
#endif
#include "Appdefs.h"
#include "Tabs.h"
#include "PanelBoard.h"
#include "PanelContext.h"
#include "GameLogic.h"
#include "Lang.h"
#include "CentralWorkSaver.h"
#include "Session.h"
#include "Log.h"
#include "thc.h"
#include "UciInterface.h"
#include "DebugPrintf.h"
#include "AutoTimer.h"
#include "Book.h"
#include "Database.h"
#include "Objects.h"
#include "BookDialog.h"
#include "LogDialog.h"
#include "EngineDialog.h"
#include "MaintenanceDialog.h"
#include "TrainingDialog.h"
#include "GeneralDialog.h"
#include "Repository.h"
#include "CtrlBox.h"
#include "CtrlBoxBookMoves.h"
#include "CtrlChessTxt.h"
#include "DialogDetect.h"

// For Windows Tarrasch4Squares.ico is included in Tarrasch.exe as a resource, for Unix a different system is required
#ifdef THC_UNIX
static const char *bbbbbbbb_xpm[] = {

/* columns rows colors chars-per-pixel */
"218 218 47 1",
"Z c None",
"  c #ffe2b3",
"! c #dca274",
"# c #d39b6f",
"$ c #f5d9ab",
"% c #9b7251",
"& c #cb956b",
"( c #c0aa86",
") c #8b7b61",
"* c #7a6c55",
"+ c #b49f7e",
", c #99876b",
"- c #ecd1a5",
". c #a57957",
"/ c #77583f",
"0 c #694d37",
"1 c #846145",
"2 c #685c49",
"3 c #000000",
"4 c #393228",
"5 c #e1c79d",
"6 c #59422f",
"7 c #312419",
"8 c #c28e66",
"9 c #d7be96",
": c #906a4b",
"; c #b98861",
"< c #b0815c",
"= c #393939",
"> c #b4b4b4",
"? c #53493a",
"@ c #e1e1e1",
"A c #ffffff",
"B c #f5f5f5",
"C c #686868",
"D c #473425",
"E c #d7d7d7",
"F c #a7a7a7",
"G c #999999",
"H c #7a7a7a",
"I c #ccb48f",
"J c #a79475",
"K c #c0c0c0",
"L c #cccccc",
"M c #ececec",
"N c #535353",
"O c #8b8b8b",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!./0/%#!!!!!!!!!!.100%&!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#63333378!!!!!!!#6333337.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#6333333378!!!!!#6333333338!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333330!!!!!%3333333330!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"3?)(                                                   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!D3333333333#!!!!D3333333333#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                  ()?3",
"3333?9                                                 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!33333333333;!!!!33333333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                9?3333",
"333333(                                                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!33333333333;!!!!33333333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                               (333333",
"3333333(                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!63333333333#!!!!/3333333333&!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              (3333333",
"33333333-                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333331!!!!!;3333333331!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             -33333333",
"33333333,                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#633333336#!!!!!!%33333331#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             ,33333333",
"333333334                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#/333330#!!!!!!!;3333333<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             333333333",
"3333333335                                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!873333378!!!!!!#D33333337&!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                            9333333333",
"3333333339                                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#D33333337#!!!!!63333333336!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                            9333333333",
"3333333339                                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!63333333336#!!#133333333333:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                            5333333333",
"333333333$                                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!133333333333D8#63333333333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             ?33333333",
"33333333*                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.333333333333376333333333333337#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             (33333333",
"333333335                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!873333333333333333333333333333331!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              23333333",
"3333333(                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#633333333333333333HC3333333333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              +3333333",
"33333332-                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:3333333333333333NMAABK=3333333333D!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                            9)33333333",
"333333334+$                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!<33333333333333333FAAAAABC3333333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                          5*3333333333",
"33333333334,I$                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#733333333333333333NAAAAAABC3333333337#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                        $,333333333333",
"333333333333332+-                                      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!/3333333333333333333KAAAAAABN333333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                     $(*33333333333333",
"33333333333333333,9                                    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!;33333333333333333333=BAAAAAA@3333333337#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                   $+43333333333333333",
"3333333333333333333)-                                  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#D333333333333333333333OAAAAAAA>333333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                  +4333333333333333333",
"333333333333333333334(                                 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%33333333333333333333333KAAAAAAAC333333337!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                $,33333333333333333333",
"3333333333333333333333(                                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!&733333333333333333333333=MAAAAAA@333333333.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                               $2333333333333333333333",
"33333333333333333333333(                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!13333333333333333333333333HAAAAAAAG333333336!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                               )3333333333333333333333",
"3333333333333333333333335                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!&333333333333333333333333333>AAAAAAB=33333333&!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                              +33333333333333333333333",
"333333333333333333333333*                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!13333333333333333333333333333MAAAAAA>33333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                             -433333333333333333333333",
"33333333333333333333333335                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!&33333333333333333333333333333HAAAAAAB=33333336!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                             ,333333333333333333333333",
"3333333333333333333333333J                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:333333333333333333333333333333KAAAAAAG33333333#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            $3333333333333333333333333",
"33333333333333333333333332                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!7333333333333333333333333333333NBAAAAAM33333333<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            +3333333333333333333333333",
"33333333333333333333333333                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!.33333333333333333333333333333333>AAAAAAH3333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            *3333333333333333333333333",
"33333333333333333333333333                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!/33333333333333333333333333333333NAAAAAAK33333330!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            43333333333333333333333333",
"33333333333333333333333333                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!!3333333333333333333333333333333333LAAAAAB3333333D!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            33333333333333333333333333",
"33333334******************                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!;3333333333333333333333333333333333GAAAAAAO3333333!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            ******************33333333",
"3333333*                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333333333333333333333333333333NAAAAAA>3333333&!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              33333333",
"3333333*                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!/33333333333333333333333333333333333@AAAAA@3333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              33333333",
"3333333*                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!633333333333333333333333333333333333>AAAAAA3333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              33333333",
"3333333*                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!333333333333333333333333333333333333OAAAAAAH333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              33333333",
"3333333*                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!3333333333333333333333333333333333333AAAAAAG333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              33333333",
"3333333*                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!3333333333333333333333333333333333333EAAAAA>333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                              33333333",
"3333333?                                               !!!!!!!!!!!!!!!!!!!!!!!!!!!!3333333333333333333333333333333333333FAAAAAK333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             -33333333",
"33333333(                                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!7333333333333333333333333333333333333CAAAAAE333333!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                             ,33333333",
"333333334$                                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!03333333333333333333333333333333333333MAAAAE333333!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                            -333333333",
"333333333+                                             !!!!!!!!!!!!!!!!!!!!!!!!!!!!/3333333333333333333333333333333333333KAAAAE33333D!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                            J333333333",
"3333333334$                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333333333333333333333333333333333OAAAAE333330!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           $4333333333",
"3333333333+                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!;33333333333333333333333333333333333333MAAAE33333:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           +3333333333",
"3333333333?                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!33333333333333333333333333333333333333>AAA>33333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           ?3333333333",
"33333333333(                                           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!03333333333333333333333333333333333333=BAAO33333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                          933333333333",
"33333333333)                                           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!%33333333333333333333333333333333333333GA@333333!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                          ,33333333333",
"333333333333-                                          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!&3333333333333333333=HHHHHHHHN3333333333N3333330!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                          433333333333",
"333333333333+                                          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!63333333333333C>L@AAAAAAAAAAAAME>O=33333333333:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         9333333333333",
"333333333333)                                          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.3333333333N>BAAAAAAAAAAAAAAAAAAAAALH333333333<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         +333333333333",
"3333333333333                                          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#73333333NKAAAAAAAAAAAAAAAAAAAAAAAAAALN3333333!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         *333333333333",
"33333333333335                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:333333GBAAAAAAAAAAAAAAAAAAAAAAAAAAAABO33333/!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         3333333333333",
"3333333333333I                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!&33333HAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAN3333.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                        $3333333333333",
"3333333333333+                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!13333>AAAAAAAAAME>>>HHHO>>L@AAAAAAAAAA>3333#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                        93333333333333",
"3333333333333+                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!&3333>AAAAALFH333333333333333CGKMAAAAAF3331!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                        93333333333333",
"3333333333333+                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:333CAA@F=3333333333333333333333C>MABN333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                        93333333333333",
"3333333333333(                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#7333NC3333333333H>EEEE>N3333333333CN3330!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                        53333333333333",
"33333333333335                                         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.33333333333333NAAAAAAAB333333333333333<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         4333333333333",
"333333333333?                                          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.3333O>H33333333LAAAAAA>33333333O>H3333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         ,333333333333",
"333333333333I                                          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%333FAAL3333333333HHHH3333333333EAAF333<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                         9333333333333",
"33333333333(                                           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%333CEF=333333333333333333333333=>LN333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                          (33333333333",
"3333333333(                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%33333333333333333333333333333333333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           +3333333333",
"33333333339                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333333333333HHHHHH=333333333333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           93333333333",
"33333333339                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333333NG>EAAAAAAAAAAML>C3333333333%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           93333333333",
"3333333333+                                            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%3333333=F@AAAAAAAAAAAAAAAAAAB>=3333333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                           +3333333333",
"3333333333,9999$                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!<33333NKAAAAAAAAAAAAAAAAAAAAAAAB>=33333;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                       $999,3333333333",
"333333333333333332,(-                                  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!;3333FAAAAAAAAAAAAAAAAAAAAAAAAAAABG3333&!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                  9+*?3333333333333333",
"333333333333333333333*(                                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#333NAAAAAAAAABEL>>>>>>EEBAAAAAAAAA3333!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                               9,433333333333333333333",
"33333333333333333333333?9                              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!733HAAAAAALGN333333333333NGEAAAAAA3336!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                             9*33333333333333333333333",
"3333333333333333333333333J                             !!!!!!!!!!!!!!!!!!!!&;;;;;;#!!!!!!!033=AAAB>N333333333333333333NKBAAB3331!!!!!!!#;;;;;;#!!!!!!!!!!!!!!!!!!!!!                            J3333333333333333333333333",
"33333333333333333333333333*                            !!!!!!!!!!!!!!!8%0D3333333333300%%;:333OEF=3333333333333333333333=>EO333%;%%0033333333333D/%8!!!!!!!!!!!!!!!!                           ,33333333333333333333333333",
"333333333333333333333333333,                           !!!!!!!!!!!!!%D3333333333333333333333333333333333333333333333333333333333333333333333333333336.!!!!!!!!!!!!!!                          ,333333333333333333333333333",
"3333333333333333333333333333(                          !!!!!!!!!!!;D333333333333333333333333333333333333333333333333333333333333333333333333333333333368!!!!!!!!!!!!                         I3333333333333333333333333333",
"33333333333333333333333333334$                         !!!!!!!!!!.333333333333333333333333333333333333333333333333333333333333333333333333333333333333378!!!!!!!!!!!                         ?3333333333333333333333333333",
"33333333333333333333333333333(                         !!!!!!!!!;3333333333333333333333333333333333333333333333333333333333333333333333333333333333333337&!!!!!!!!!!                        933333333333333333333333333333",
"33333333333333333333333333333)                         !!!!!!!!!D33333333333333333333333333333333333333333333333333333333333333333333333333333333333333330!!!!!!!!!!                        J33333333333333333333333333333",
"333333333333333333333333333334                         !!!!!!!!<3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333338!!!!!!!!!                        *33333333333333333333333333333",
"333333333333333333333333333333                         !!!!!!!!13333333333333333333333333333333333333336%;8!!!!&;%6333333333333333333333333333333333333333%!!!!!!!!!                        333333333333333333333333333333",
"333333333333333333333333333333                         !!!!!!!!633333333333333333333333333333333333330;!!!!!!!!!!!!;033333333333333333333333333333333333330!!!!!!!!!                        333333333333333333333333333333",
"******************************                         !!!!!!!!3333333333333333333333333333333333333%!!!!!!!!!!!!!!!!%3333333333333333333333333333333333336!!!!!!!!!                        ******************************",
"                                                       !!!!!!!!333333333333333333333333333333333333%!!!!!!!!!!!!!!!!!!:333333333333333333333333333333333333!!!!!!!!!                                                      ",
"                                                       !!!!!!!!333333333333333333333333333333333330!!!!!!!!!!!!!!!!!!!!63333333333333333333333333333333333D!!!!!!!!!                                                      ",
"                                                       !!!!!!!!D33333336/%%;;;#!!!!!!!!!!!8;;;;;%%&!!!!!!!!!!!!!!!!!!!!8%%;;;;;8!!!!!!!!!!!&;;;%%0633333330!!!!!!!!!                                                      ",
"                                                       !!!!!!!!/337/%8!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!;%/333:!!!!!!!!!                                                      ",
"                                                       !!!!!!!!./<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.0;!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"                                                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                                                      ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                  ()?33?)(                                                   ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                9?33333333?9                                                 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                               (333333333333(                                                ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              (33333333333333(                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             -3333333333333333-                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             ,3333333333333333,                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             333333333333333334                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                            93333333333333333335                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                            93333333333333333339                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                            53333333333333333339                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             ?33333333333333333$                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             (3333333333333333*                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              23333333333333335                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              +33333333333333(                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                            9)3333333333333332-                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                          5*3333333333333333334+$                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                        $,33333333333333333333334,I$                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                     $(*33333333333333333333333333332+-                                      ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                   $+4333333333333333333333333333333333,9                                    ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                  +43333333333333333333333333333333333333)-                                  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                $,33333333333333333333333333333333333333334(                                 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                               $23333333333333333333333333333333333333333333(                                ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                               )333333333333333333333333333333333333333333333(                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                              +333333333333333333333333333333333333333333333335                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                             -433333333333333333333333333333333333333333333333*                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                             ,33333333333333333333333333333333333333333333333335                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            $33333333333333333333333333333333333333333333333333J                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            +333333333333333333333333333333333333333333333333332                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            *333333333333333333333333333333333333333333333333333                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            4333333333333333333333333333333333333333333333333333                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            3333333333333333333333333333333333333333333333333333                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            ******************3333333333333334******************                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              333333333333333*                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              333333333333333*                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              333333333333333*                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              333333333333333*                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              333333333333333*                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                              333333333333333*                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             -333333333333333?                                               ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                             ,3333333333333333(                                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                            -333333333333333334$                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                            J333333333333333333+                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           $43333333333333333334$                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           +33333333333333333333+                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           ?33333333333333333333?                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                          93333333333333333333333(                                           ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                          ,3333333333333333333333)                                           ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                          433333333333333333333333-                                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         9333333333333333333333333+                                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         +333333333333333333333333)                                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         *3333333333333333333333333                                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         333333333333333333333333335                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                        $33333333333333333333333333I                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                        933333333333333333333333333+                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                        933333333333333333333333333+                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                        933333333333333333333333333+                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                        533333333333333333333333333(                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         433333333333333333333333335                                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         ,333333333333333333333333?                                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                         9333333333333333333333333I                                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                          (3333333333333333333333(                                           ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           +33333333333333333333(                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           9333333333333333333339                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           9333333333333333333339                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                           +33333333333333333333+                                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                       $999,33333333333333333333,9999$                                       ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                  9+*?3333333333333333333333333333333332,(-                                  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                               9,433333333333333333333333333333333333333333*(                                ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                             9*3333333333333333333333333333333333333333333333?9                              ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                            J33333333333333333333333333333333333333333333333333J                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                           ,3333333333333333333333333333333333333333333333333333*                            ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                          ,333333333333333333333333333333333333333333333333333333,                           ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                         I33333333333333333333333333333333333333333333333333333333(                          ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                         ?333333333333333333333333333333333333333333333333333333334$                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        93333333333333333333333333333333333333333333333333333333333(                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        J3333333333333333333333333333333333333333333333333333333333)                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        *33333333333333333333333333333333333333333333333333333333334                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        333333333333333333333333333333333333333333333333333333333333                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        333333333333333333333333333333333333333333333333333333333333                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        ************************************************************                         ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                                                                                                             ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
};

#endif
#if 1
    #include "bitmaps/myicons.xpm"
    //#include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/paste.xpm"
    #include "bitmaps/undo.xpm"
    #include "bitmaps/redo.xpm"

#else
    #include "bitmaps24/sample.xpm"
    #include "bitmaps24/new.xpm"
    #include "bitmaps24/open.xpm"
    #include "bitmaps24/save.xpm"
    #include "bitmaps24/copy.xpm"
    #include "bitmaps24/cut.xpm"
    #include "bitmaps24/preview.xpm"  // paste XPM
    #include "bitmaps24/print.xpm"
    #include "bitmaps24/help.xpm"
#endif

using namespace std;
using namespace thc;

Objects objs;

// Should really be a little more sophisticated about this
#define TIMER_ID 2001

static void OpenShellFile( std::string &filename_from_shell )
{
	if( filename_from_shell != "" )
	{
		wxCommandEvent *p = new wxCommandEvent(wxEVT_MENU, ID_FILE_OPEN_SHELL);
		objs.gl->filename_from_shell = filename_from_shell;
		objs.frame->GetEventHandler()->QueueEvent(p);
	}
}

static void OpenShellFileAsk( std::string &filename_from_shell )
{
	if( filename_from_shell != "" )
    {
		wxCommandEvent *p = new wxCommandEvent(wxEVT_MENU, ID_FILE_OPEN_SHELL_ASK);
		objs.gl->filename_from_shell = filename_from_shell;
		objs.frame->GetEventHandler()->QueueEvent(p);
    }
}

// Some IPC stuff to support proper instance handling
int DialogDetect::counter=0;	              // Don't accept shell commands if we are running a dialog
void (*DialogDetect::func)(std::string &);    // Run them when the dialog exits
std::string DialogDetect::parm;
class InterProcessCommunicationConnection : public wxConnection
{
protected:
	bool OnExec(const wxString& topic, const wxString& data)
	{
		if( topic == "Activate" )
		{
			cprintf("Activated by another instance starting\n");
			if( objs.frame && objs.gl )
			{
				objs.frame->Raise();
				std::string filename_from_shell = std::string(data.c_str());
				if( DialogDetect::IsOpen() )
                {
				    DialogDetect::OnClose( OpenShellFileAsk, filename_from_shell );
                }
                else
				{
                    OpenShellFile( filename_from_shell );
				}
			}
		}
		return true;
	}
};

class InterProcessCommunicationServer : public wxServer
{
public:
    wxConnectionBase *OnAcceptConnection( const wxString& WXUNUSED(topic) )
	{
        return new InterProcessCommunicationConnection;
    }
};


// ----------------------------------------------------------------------------
// application class
// ----------------------------------------------------------------------------

class ChessApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int  OnExit();
private:
    wxSingleInstanceChecker m_one;
	InterProcessCommunicationServer *m_server=NULL;
};

CtrlBoxBookMoves *gbl_book_moves;

class PanelNotebook: public wxWindow
{
public:
    PanelNotebook( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size, int book_moves_width_ );
#ifdef AUI_NOTEBOOK
    void OnTabSelected( wxAuiNotebookEvent& event );
    void OnTabClose( wxAuiNotebookEvent& event );
    wxAuiNotebook	*notebook;
#else
    void OnTabSelected( wxBookCtrlEvent& event );
    wxNotebook *notebook;
#endif
	void OnTabNew( wxCommandEvent& event );
    void OnSize( wxSizeEvent &evt );
    int book_moves_width;
    int new_tab_button_width;
	wxButton *new_tab_button;
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(PanelNotebook, wxWindow)
    EVT_SIZE( PanelNotebook::OnSize )
#ifdef AUI_NOTEBOOK
    EVT_AUINOTEBOOK_PAGE_CHANGED( wxID_ANY, PanelNotebook::OnTabSelected)   //user selects a tab
    EVT_AUINOTEBOOK_PAGE_CLOSE( wxID_ANY, PanelNotebook::OnTabClose)        //user closes a tab
#else
    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, PanelNotebook::OnTabSelected)   //user selects a tab
#endif
    EVT_BUTTON( ID_BUTTON_TAB_NEW, PanelNotebook::OnTabNew )
END_EVENT_TABLE()


// Constructor
PanelNotebook::PanelNotebook
(
    wxWindow *parent, 
    wxWindowID id,
    const wxPoint &point,
    const wxSize &siz,
    int book_moves_width_
)
    : wxWindow( parent, id, point, siz, wxNO_BORDER )
{
    // Note that the wxNotebook is twice as high as the panel it's in. If the wxNotebook is very short,
    // a weird erasure effect occurs on the first tab header when creating subsequent tabs. Making the
    // wxNotebook think it's less short fixes this - and it still appears nice and short through the
    // short panel parenting it. [Removed this when changing to wxAuiNotebook]
    book_moves_width = book_moves_width_;
	new_tab_button = new wxButton( this, ID_BUTTON_TAB_NEW, "New Tab" );
    wxClientDC dc(new_tab_button);
    wxCoord width, height, descent, external_leading;
    dc.GetTextExtent( "New Tab", &width, &height, &descent, &external_leading );
    new_tab_button_width = (width*130) / 100;
	//wxSize butt_sz = new_tab_button->GetSize();
	//new_tab_button_width = butt_sz.x;
#ifdef AUI_NOTEBOOK
    notebook = new wxAuiNotebook(this, wxID_ANY, wxPoint(5,0),  wxSize(siz.x-book_moves_width-10-new_tab_button_width-10,siz.y*5),
        wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | /* wxAUI_NB_WINDOWLIST_BUTTON | /*wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS |*/ wxAUI_NB_CLOSE_ON_ACTIVE_TAB );


#else
    notebook = new wxNotebook(this, wxID_ANY, wxPoint(5,0), wxSize(siz.x-book_moves_width-10-new_tab_button_width-10,siz.y*5) );
#endif
    NotebookPage *notebook_page1 = new NotebookPage(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize );
    notebook->AddPage(notebook_page1,"New Game",true);

    wxPoint pt(siz.x-book_moves_width-2,8);
    wxSize  sz(book_moves_width,siz.y-8);
    gbl_book_moves = new CtrlBoxBookMoves( this,
                          wxID_ANY,
                          pt,
                          sz );
	pt.x -= (new_tab_button_width+10);
	sz.x  =  new_tab_button_width;
	new_tab_button->SetPosition(pt);
	new_tab_button->SetSize(sz);
}

void PanelNotebook::OnSize( wxSizeEvent &evt )
{
    wxSize siz = evt.GetSize();
    wxSize sz1;
    sz1.x = siz.x-book_moves_width-10-new_tab_button_width-10;
    sz1.y = siz.y*5;
    notebook->SetSize(sz1);
    wxSize  sz(book_moves_width,siz.y-8);
    wxPoint pt(siz.x-book_moves_width-2,8);
    gbl_book_moves->SetPosition(pt);
	pt.x -= (new_tab_button_width+10);
	sz.x  =  new_tab_button_width;
	new_tab_button->SetPosition(pt);
	new_tab_button->SetSize(sz);
}

void PanelNotebook::OnTabNew( wxCommandEvent& WXUNUSED(event) )
{
	objs.gl->CmdTabNew();
}

#ifdef AUI_NOTEBOOK
void PanelNotebook::OnTabClose( wxAuiNotebookEvent& event )
{
	if(	objs.tabs->GetNbrTabs() > 1 )
	{
#if 1
		event.Veto();				// stop system closing tab
		objs.gl->CmdTabClose();		// Okay if always close active tab
#else
		int idx = event.GetSelection();
        objs.gl->OnTabClose(idx);
#endif
	}
	else
	{
		event.Veto();			// never close last tab
		objs.frame->Close();	// same as exit from main menu
	}
}

void PanelNotebook::OnTabSelected( wxAuiNotebookEvent& event )
#else
void PanelNotebook::OnTabSelected( wxBookCtrlEvent& event )
#endif
{
    int idx = event.GetSelection();
    if( objs.gl )
        objs.gl->OnTabSelected(idx);
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

//  We also optionally prepend the time - to prepend the time instantiate a DebugPrintfTime object
//  on the stack - no need to use it
static int dbg_printf_prepend_time=0;
static FILE *teefile;   // private debugging log
#ifdef DURING_DEVELOPMENT
static bool dbg_console_enabled = true;     // set this to false except during development
#else
static bool dbg_console_enabled = false;    // set this to false except during development
#endif
DebugPrintfTime::DebugPrintfTime()  { dbg_printf_prepend_time++; }
DebugPrintfTime::~DebugPrintfTime() { dbg_printf_prepend_time--; if(dbg_printf_prepend_time<0) dbg_printf_prepend_time=0; }
int AutoTimer::instance_cnt;
AutoTimer *AutoTimer::instance_ptr;

#ifndef KILL_DEBUG_COMPLETELY
int core_printf( const char *fmt, ... )
{
    if( !dbg_console_enabled )
        return 0;
    int ret=0;
	va_list args;
	va_start( args, fmt );
    char buf[1000];
    char *p = buf;
    if( dbg_printf_prepend_time )
    {
        AutoTimer *at = AutoTimer::instance_ptr;
        if( at )
        {
            sprintf( p, "%fms ", at->Elapsed() );
            p = strchr(buf,'\0');
        }
        else
        {
            time_t rawtime;
            struct tm * timeinfo;
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            strcpy( p, asctime(timeinfo) );
            p = strchr(buf,'\n');
        }
    }
    vsnprintf( p, sizeof(buf)-2-(p-buf), fmt, args ); 
    fputs(buf,stdout);
    if( teefile )
        fputs(buf,teefile);
    va_end(args);
    return ret;
}
#endif

// New approach to logging - use a console window to simulate Mac behaviour on Windows
#ifdef THC_WINDOWS
static const WORD MAX_CONSOLE_LINES = 500;      // maximum mumber of lines the output console should have
void RedirectIOToConsole()
{
    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    // allocate a console for this app
    AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(handle, &coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(handle, coninfo.dwSize);

	int height = 60;
	int width = 80;

	_SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = height - 1;
	rect.Right = width - 1;

	SetConsoleWindowInfo(handle, TRUE, &rect);            // Set Window Size

#if 1
	// Redirect the CRT standard input, output, and error handles to the console
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//Clear the error state for each of the C++ standard stream objects. We need to do this, as
	//attempts to access the standard streams before they refer to a valid target will cause the
	//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
	//to always occur during startup regardless of whether anything has been read from or written to
	//the console or not.
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
#else


    // redirect unbuffered STDOUT to the console
    int hConHandle;
    long lStdHandle;
    FILE *fp;
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
	/*
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    ios::sync_with_stdio(); */
#endif
}
#endif


class ChessFrame: public wxFrame
{
public:
    ChessFrame(const wxString& title, const wxPoint& pos, const wxSize& size, bool pos_siz_restored );
    ~ChessFrame()
    {
        context->resize_ready = false;   // stops a bogus resize during shutdown on mac
    }  
    void OnIdle(wxIdleEvent& event);
    void OnMove       (wxMoveEvent &event);
    void OnMouseWheel( wxMouseEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnTimeout    (wxTimerEvent& event);
    void OnQuit       (wxCommandEvent &);
    void OnClose      (wxCloseEvent &);
    void OnAbout      (wxCommandEvent &);
//  void OnUnimplemented    (wxCommandEvent &);
    void OnHelp             (wxCommandEvent &);
    void OnCredits          (wxCommandEvent &);
    void OnFlip             (wxCommandEvent &);
    void OnButtonUp         (wxCommandEvent &);
    void OnButtonDown       (wxCommandEvent &);
    void OnButtonLeft       (wxCommandEvent &);
    void OnButtonRight      (wxCommandEvent &);

    void OnSetPosition(wxCommandEvent &);
    void OnNewGame    (wxCommandEvent &);
    void OnTakeback   (wxCommandEvent &);
        void OnUpdateTakeback(wxUpdateUIEvent &);
    void OnMoveNow    (wxCommandEvent &);
        void OnUpdateMoveNow(wxUpdateUIEvent &);
    void OnDraw       (wxCommandEvent &);
        void OnUpdateDraw(wxUpdateUIEvent &);
    void OnWhiteResigns(wxCommandEvent &);
        void OnUpdateWhiteResigns(wxUpdateUIEvent &);
    void OnBlackResigns(wxCommandEvent &);
        void OnUpdateBlackResigns(wxUpdateUIEvent &);
    void OnPlayWhite  (wxCommandEvent &);
        void OnUpdatePlayWhite(wxUpdateUIEvent &);
    void OnPlayBlack  (wxCommandEvent &);
        void OnUpdatePlayBlack(wxUpdateUIEvent &);
    void OnSwapSides  (wxCommandEvent &);
        void OnUpdateSwapSides(wxUpdateUIEvent &);
    void OnTabNew  (wxCommandEvent &);
        void OnUpdateTabNew(wxUpdateUIEvent &);
    void OnTabClose  (wxCommandEvent &);
        void OnUpdateTabClose(wxUpdateUIEvent &);
    void OnTabInclude (wxCommandEvent &);
        void OnUpdateTabInclude(wxUpdateUIEvent &);
    void OnKibitz    (wxCommandEvent &);
        void OnUpdateKibitz(wxUpdateUIEvent &);
    void OnClearKibitz    (wxCommandEvent &);
        void OnUpdateClearKibitz(wxUpdateUIEvent &);
    void OnPlayers    (wxCommandEvent &);
        void OnUpdatePlayers(wxUpdateUIEvent &);
    void OnClocks     (wxCommandEvent &);
        void OnUpdateClocks(wxUpdateUIEvent &);
    void OnOptionsReset(wxCommandEvent &);
        void OnUpdateOptionsReset(wxUpdateUIEvent &);
    void OnBook       (wxCommandEvent &);
        void OnUpdateBook(wxUpdateUIEvent &);
    void OnLog        (wxCommandEvent &);
        void OnUpdateLog(wxUpdateUIEvent &);
    void OnEngine     (wxCommandEvent &);
        void OnUpdateEngine(wxUpdateUIEvent &);
    void OnDatabaseMaintenance(wxCommandEvent &);
        void OnUpdateDatabaseMaintenance(wxUpdateUIEvent &);
    void OnDatabaseSearch(wxCommandEvent &);
        void OnUpdateDatabaseSearch(wxUpdateUIEvent &);
    void OnDatabaseShowAll(wxCommandEvent &);
        void OnUpdateDatabaseShowAll(wxUpdateUIEvent &);
    void OnDatabasePlayers(wxCommandEvent &);
        void OnUpdateDatabasePlayers(wxUpdateUIEvent &);
    void OnDatabaseSelect(wxCommandEvent &);
        void OnUpdateDatabaseSelect(wxUpdateUIEvent &);
    void OnDatabaseCreate(wxCommandEvent &);
        void OnUpdateDatabaseCreate(wxUpdateUIEvent &);
    void OnDatabaseAppend(wxCommandEvent &);
        void OnUpdateDatabaseAppend(wxUpdateUIEvent &);
    void OnDatabasePattern(wxCommandEvent &);
        void OnUpdateDatabasePattern(wxUpdateUIEvent &);
    void OnDatabaseMaterial(wxCommandEvent &);
        void OnUpdateDatabaseMaterial(wxUpdateUIEvent &);
    void OnTraining   (wxCommandEvent &);
        void OnUpdateTraining(wxUpdateUIEvent &);
    void OnGeneral    (wxCommandEvent &);
        void OnUpdateGeneral (wxUpdateUIEvent &);
    void RefreshLanguageFont( const char *from, int before_font_size, bool before_no_italics,
                              const char *to,   int after_font_size,  bool after_no_italics );

    void OnFileNew (wxCommandEvent &);
        void OnUpdateFileNew( wxUpdateUIEvent &);
    void OnFileOpen (wxCommandEvent &);
        void OnUpdateFileOpen(wxUpdateUIEvent &);
    void OnFileOpenShell (wxCommandEvent &);
    void OnFileOpenShellAsk (wxCommandEvent &);
	void OnFileOpenMru(wxCommandEvent&);
    void OnFileOpenLog (wxCommandEvent &);
        void OnUpdateFileOpenLog(wxUpdateUIEvent &);
    void OnFileSave (wxCommandEvent &);
        void OnUpdateFileSave( wxUpdateUIEvent &);
    void OnFileSaveAs (wxCommandEvent &);
        void OnUpdateFileSaveAs( wxUpdateUIEvent &);
    void OnFileSaveGameAs (wxCommandEvent &);
        void OnUpdateFileSaveGameAs( wxUpdateUIEvent &);
    void OnGamesCurrent (wxCommandEvent &);
        void OnUpdateGamesCurrent( wxUpdateUIEvent &);
    void OnGamesDatabase (wxCommandEvent &);
        void OnUpdateGamesDatabase( wxUpdateUIEvent &);
    void OnGamesSession (wxCommandEvent &);
        void OnUpdateGamesSession( wxUpdateUIEvent &);
    void OnGamesClipboard (wxCommandEvent &);
        void OnUpdateGamesClipboard( wxUpdateUIEvent &);
    void OnNextGame (wxCommandEvent &);
        void OnUpdateNextGame( wxUpdateUIEvent &);
    void OnPreviousGame (wxCommandEvent &);
        void OnUpdatePreviousGame( wxUpdateUIEvent &);

    void OnEditCopy (wxCommandEvent &);
        void OnUpdateEditCopy( wxUpdateUIEvent &);
    void OnEditCut (wxCommandEvent &);
        void OnUpdateEditCut( wxUpdateUIEvent &);
    void OnEditPaste (wxCommandEvent &);
        void OnUpdateEditPaste( wxUpdateUIEvent &);
    void OnEditUndo (wxCommandEvent &);
        void OnUpdateEditUndo( wxUpdateUIEvent &);
    void OnEditRedo (wxCommandEvent &);
        void OnUpdateEditRedo( wxUpdateUIEvent &);
    void OnEditDelete (wxCommandEvent &);
        void OnUpdateEditDelete( wxUpdateUIEvent &);
    void OnEditGameDetails (wxCommandEvent &);
        void OnUpdateEditGameDetails( wxUpdateUIEvent &);
    void OnEditGamePrefix (wxCommandEvent &);
        void OnUpdateEditGamePrefix( wxUpdateUIEvent &);
    void OnEditCopyGamePGNToClipboard (wxCommandEvent &);
        void OnUpdateEditCopyGamePGNToClipboard(wxUpdateUIEvent &);
    void OnEditCopyFENToClipboard (wxCommandEvent &);
        void OnUpdateEditCopyFENToClipboard(wxUpdateUIEvent &);
    void OnEditDeleteVariation (wxCommandEvent &);
        void OnUpdateEditDeleteVariation( wxUpdateUIEvent &);
    void OnEditPromote (wxCommandEvent &);
        void OnUpdateEditPromote( wxUpdateUIEvent &);
    void OnEditDemote (wxCommandEvent &);
        void OnUpdateEditDemote( wxUpdateUIEvent &);
    void OnEditDemoteToComment (wxCommandEvent &);
        void OnUpdateEditDemoteToComment( wxUpdateUIEvent &);
    void OnEditPromoteToVariation (wxCommandEvent &);
        void OnUpdateEditPromoteToVariation( wxUpdateUIEvent &);
    void OnEditPromoteRestToVariation (wxCommandEvent &);
        void OnUpdateEditPromoteRestToVariation( wxUpdateUIEvent &);


    DECLARE_EVENT_TABLE()
private:
    PanelContext *context;
    wxTimer m_timer;
    void SetFocusOnList() { if(context) context->SetFocusOnList(); }
};

//-----------------------------------------------------------------------------
// Application class
//-----------------------------------------------------------------------------

IMPLEMENT_APP(ChessApp)

extern void JobBegin();
extern void JobEnd();

wxString argv1;  // can pass a .pgn file on command line
bool gbl_spelling_us;			// True for US spelling
const char *gbl_spell_colour;	// "colour" or "color"

bool ChessApp::OnInit()
{
    // Check if there is another process running.
    if( m_one.IsAnotherRunning() ) {

		// If we have a valid filename as a parameter, open it in other instance
		if( argc > 1 )
		{
			wxFileName fn(argv[1]);
			std::string name      = std::string(fn.GetFullName().c_str());
			std::string dir       = std::string(fn.GetPath().c_str());
			std::string full_path = std::string(fn.GetFullPath().c_str());
			if( dir == "" )
			{
				fn.Assign(wxFileName::GetCwd(),argv[1]);
				name      = std::string(fn.GetFullName().c_str());
				dir       = std::string(fn.GetPath().c_str());
				full_path = std::string(fn.GetFullPath().c_str());
			}
			bool ok            = fn.IsOk();
			bool exists        = fn.FileExists();
			if( ok && exists )
			{
				// Create a IPC client and use it to ask the existing process to show itself.
				wxClient *client = new wxClient;
				wxConnectionBase *conn = client->MakeConnection("localhost", "/tmp/a_socket" /* or a port number */, "Activate");
				conn->Execute( full_path.c_str() );
				delete conn;
				delete client;
				// Don't enter the message loop.
				return false;
			}
		}
    }

    //_CrtSetBreakAlloc(1050656);
    srand(time(NULL));
    //_CrtSetBreakAlloc( 198300 ); //563242 );
    //_CrtSetBreakAlloc( 195274 );
    //_CrtSetBreakAlloc( 21007 );
    JobBegin();
    if( argc == 2 )
    {
        argv1 = argv[1];
        if( argv1[0] == '-' )   // any switch is interpreted as turn on console
        {
            argv1 = "";
            dbg_console_enabled = true;
        }
    }
    #ifndef KILL_DEBUG_COMPLETELY
    if( dbg_console_enabled )
    {
        #ifdef THC_WINDOWS
        #ifdef DURING_DEVELOPMENT
        teefile = fopen( "/users/bill/documents/tarrasch/debuglog.txt", "wt" );
        #endif
        RedirectIOToConsole();
        #endif
    }
    #endif
    //thc::ChessRules cr;
    //cr.TestInternals();
	int lang = wxLocale::GetSystemLanguage();
	gbl_spelling_us = (lang==wxLANGUAGE_ENGLISH_US);
	gbl_spell_colour = gbl_spelling_us ? "color" : "colour";
	cprintf( "Spelling uses US English? %s\n", gbl_spelling_us?"Yes":"No" );
    wxString error_msg;
    int disp_width, disp_height;
    wxDisplaySize(&disp_width, &disp_height);
    cprintf( "Display size = %d x %d\n", disp_width, disp_height );
    objs.repository = new Repository;

    // Use these as fallbacks/defaults
    int xx = (disp_width * 10) /100;
    int yy = (disp_height * 10) /100;
    int ww = (disp_width * 80) /100;
    int hh = (disp_height * 80) /100;
    wxPoint pt(xx,yy);
    wxSize  sz(ww,hh);

    // Restore size position from repository
    cprintf( "Repository sizes/positions x=%d, y=%d, w =%d, h=%d\n",
        objs.repository->nv.m_x,
        objs.repository->nv.m_y,
        objs.repository->nv.m_w,
        objs.repository->nv.m_h );
	bool pos_siz_not_saved = ( objs.repository->nv.m_x==-1 &&
        objs.repository->nv.m_y==-1 &&
        objs.repository->nv.m_w==-1 &&
        objs.repository->nv.m_h==-1 );
    bool pos_siz_restored = !pos_siz_not_saved;
    if( pos_siz_restored )
    {
        pt.x = objs.repository->nv.m_x;
        pt.y = objs.repository->nv.m_y;
        sz.x = objs.repository->nv.m_w;
        sz.y = objs.repository->nv.m_h;
    }

    // Improved detect MAXIMISED frame logic (single display initial x,y slightly negative), multiple
	//  display one might be very negative)
    bool  maximize = false;
    objs.repository->nv.m_x = pt.x;
    objs.repository->nv.m_y = pt.y;
    objs.repository->nv.m_w = sz.x;
    objs.repository->nv.m_h = sz.y;
	if( pos_siz_restored && pt.y<0 && pt.x<0 )
		maximize = true;
	char buf[80];
	snprintf(buf,sizeof(buf),"Tarrasch Chess GUI %s",MASTER_VERSION_BASE);
    ChessFrame *frame = new ChessFrame (buf,
                                  pt, sz, pos_siz_restored );
    if( maximize )
        frame->Maximize();
    objs.frame = frame;
    SetTopWindow (frame);
    frame->Show(true);

    // Start a IPC server.
    m_server = new InterProcessCommunicationServer;
    m_server->Create("/tmp/a_socket" /* or the same port number */);

    bool another_instance_running = m_one.IsAnotherRunning();
    cprintf( "Another Instance running = %s\n", another_instance_running?"true":"false" );
    objs.db  = new Database( objs.repository->database.m_file.c_str(), another_instance_running );
	objs.canvas->SetBoardTitle( "Initial Position" );
    if( objs.gl )
        objs.gl->StatusUpdate();
    return true;
}


int ChessApp::OnExit()
{
	if( m_server )
		delete m_server;
	if (teefile)
	{
		fclose(teefile);
		teefile = NULL;
	}
    cprintf( "ChessApp::OnExit(): May wait for tiny database load here...\n" );
    extern wxMutex *KillWorkerThread();
    wxMutex *ptr_mutex_tiny_database = KillWorkerThread();
    wxMutexLocker lock(*ptr_mutex_tiny_database);
    cprintf( "ChessApp::OnExit() if we did wait, that wait is now over\n" );
    if( objs.uci_interface )
    {
        delete objs.uci_interface;
        objs.uci_interface = NULL;
    }
    JobEnd();
    if( objs.gl )
    {
        delete objs.gl;
        objs.gl = NULL;
    }
    if( objs.tabs )
    {
        delete objs.tabs;
        objs.tabs = NULL;
    }
    if( objs.db )
    {
        delete objs.db;
        objs.db = NULL;
    }
    if( objs.cws )
    {
        delete objs.cws;
        objs.cws = NULL;
    }
    if( objs.repository )
    {
        delete objs.repository;
        objs.repository = NULL;
    }
    if( objs.book )
    {
        delete objs.book;
        objs.book = NULL;
    }
    if( objs.log )
    {
        delete objs.log;
        objs.log = NULL;
    }
    if( objs.session )
    {
        delete objs.session;
        objs.session = NULL;
    }
    return wxApp::OnExit();
}

//-----------------------------------------------------------------------------
// Main frame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ChessFrame, wxFrame)
    EVT_CLOSE(                     ChessFrame::OnClose )
    EVT_MENU (wxID_EXIT,           ChessFrame::OnQuit)
    EVT_MENU (ID_CMD_ABOUT,        ChessFrame::OnAbout)
    EVT_MENU (ID_HELP_HELP,        ChessFrame::OnHelp)
    EVT_MENU (ID_HELP_CREDITS,     ChessFrame::OnCredits)
    EVT_MENU (ID_CMD_FLIP,         ChessFrame::OnFlip)
    EVT_MENU (ID_CMD_KIBITZ,       ChessFrame::OnKibitz)
        EVT_UPDATE_UI (ID_CMD_KIBITZ,      ChessFrame::OnUpdateKibitz)
    EVT_MENU (ID_CMD_CLEAR_KIBITZ,      ChessFrame::OnClearKibitz)
        EVT_UPDATE_UI (ID_CMD_CLEAR_KIBITZ,      ChessFrame::OnUpdateClearKibitz)
    EVT_MENU (ID_CMD_SET_POSITION, ChessFrame::OnSetPosition)
    EVT_MENU (ID_CMD_NEW_GAME,     ChessFrame::OnNewGame)
    EVT_MENU (wxID_UNDO,           ChessFrame::OnEditUndo)
        EVT_UPDATE_UI (wxID_UNDO,           ChessFrame::OnUpdateEditUndo)
    EVT_MENU (wxID_REDO,           ChessFrame::OnEditRedo)
        EVT_UPDATE_UI (wxID_REDO,           ChessFrame::OnUpdateEditRedo)
    EVT_MENU (ID_CMD_TAKEBACK,     ChessFrame::OnTakeback)
        EVT_UPDATE_UI (ID_CMD_TAKEBACK,     ChessFrame::OnUpdateTakeback)
    EVT_MENU (ID_CMD_MOVENOW,      ChessFrame::OnMoveNow)
        EVT_UPDATE_UI (ID_CMD_MOVENOW,      ChessFrame::OnUpdateMoveNow)
    EVT_MENU (ID_CMD_DRAW,         ChessFrame::OnDraw)
        EVT_UPDATE_UI (ID_CMD_DRAW,         ChessFrame::OnUpdateDraw)
    EVT_MENU (ID_CMD_PLAY_WHITE,   ChessFrame::OnPlayWhite)
        EVT_UPDATE_UI (ID_CMD_PLAY_WHITE,   ChessFrame::OnUpdatePlayWhite)
    EVT_MENU (ID_CMD_PLAY_BLACK,   ChessFrame::OnPlayBlack)
        EVT_UPDATE_UI (ID_CMD_PLAY_BLACK,   ChessFrame::OnUpdatePlayBlack)
    EVT_MENU (ID_CMD_WHITE_RESIGNS,ChessFrame::OnWhiteResigns)
        EVT_UPDATE_UI (ID_CMD_WHITE_RESIGNS,ChessFrame::OnUpdateWhiteResigns)
    EVT_MENU (ID_CMD_BLACK_RESIGNS,ChessFrame::OnBlackResigns)
        EVT_UPDATE_UI (ID_CMD_BLACK_RESIGNS,ChessFrame::OnUpdateBlackResigns)
    EVT_MENU (ID_CMD_SWAP_SIDES,   ChessFrame::OnSwapSides)
        EVT_UPDATE_UI (ID_CMD_SWAP_SIDES,   ChessFrame::OnUpdateSwapSides)
    EVT_MENU (ID_FILE_TAB_NEW,   ChessFrame::OnTabNew)
        EVT_UPDATE_UI (ID_FILE_TAB_NEW,   ChessFrame::OnUpdateTabNew)
    EVT_MENU (ID_FILE_TAB_CLOSE,   ChessFrame::OnTabClose)
        EVT_UPDATE_UI (ID_FILE_TAB_CLOSE,   ChessFrame::OnUpdateTabClose)
    EVT_MENU (ID_FILE_TAB_INCLUDE,   ChessFrame::OnTabInclude)
        EVT_UPDATE_UI (ID_FILE_TAB_INCLUDE,   ChessFrame::OnUpdateTabInclude)

    EVT_MENU (wxID_NEW,                     ChessFrame::OnFileNew)
        EVT_UPDATE_UI (wxID_NEW,                ChessFrame::OnUpdateFileNew)
    EVT_MENU (wxID_OPEN,                    ChessFrame::OnFileOpen)
        EVT_UPDATE_UI (wxID_OPEN,               ChessFrame::OnUpdateFileOpen)
	EVT_MENU_RANGE(wxID_FILE1,wxID_FILE1+19, ChessFrame::OnFileOpenMru)
    EVT_MENU (ID_FILE_OPEN_LOG,             ChessFrame::OnFileOpenLog)
        EVT_UPDATE_UI (ID_FILE_OPEN_LOG,        ChessFrame::OnUpdateFileOpenLog)
    EVT_MENU (wxID_SAVE,                    ChessFrame::OnFileSave)
        EVT_UPDATE_UI (wxID_SAVE,               ChessFrame::OnUpdateFileSave)
    EVT_MENU (wxID_SAVEAS,                  ChessFrame::OnFileSaveAs)
        EVT_UPDATE_UI (wxID_SAVEAS,             ChessFrame::OnUpdateFileSaveAs)
    EVT_MENU (ID_FILE_SAVE_GAME_AS,         ChessFrame::OnFileSaveGameAs)
        EVT_UPDATE_UI (ID_FILE_SAVE_GAME_AS,    ChessFrame::OnUpdateFileSaveGameAs)
    EVT_MENU (ID_GAMES_CURRENT,             ChessFrame::OnGamesCurrent)
        EVT_UPDATE_UI (ID_GAMES_CURRENT,        ChessFrame::OnUpdateGamesCurrent)
    EVT_MENU (ID_GAMES_DATABASE,            ChessFrame::OnGamesDatabase)
        EVT_UPDATE_UI (ID_GAMES_DATABASE,       ChessFrame::OnUpdateGamesDatabase)
    EVT_MENU (ID_GAMES_SESSION,             ChessFrame::OnGamesSession)
        EVT_UPDATE_UI (ID_GAMES_SESSION,        ChessFrame::OnUpdateGamesSession)
    EVT_MENU (ID_GAMES_CLIPBOARD,           ChessFrame::OnGamesClipboard)
        EVT_UPDATE_UI (ID_GAMES_CLIPBOARD,      ChessFrame::OnUpdateGamesClipboard)
    EVT_MENU (ID_CMD_NEXT_GAME,             ChessFrame::OnNextGame)
        EVT_UPDATE_UI (ID_CMD_NEXT_GAME,        ChessFrame::OnUpdateNextGame)
    EVT_MENU (ID_CMD_PREVIOUS_GAME,         ChessFrame::OnPreviousGame)
        EVT_UPDATE_UI (ID_CMD_PREVIOUS_GAME,    ChessFrame::OnUpdatePreviousGame)

    EVT_MENU (wxID_COPY,                    ChessFrame::OnEditCopy)
        EVT_UPDATE_UI (wxID_COPY,                       ChessFrame::OnUpdateEditCopy)
    EVT_MENU (wxID_CUT,                     ChessFrame::OnEditCut)
        EVT_UPDATE_UI (wxID_CUT,                        ChessFrame::OnUpdateEditCut)
    EVT_MENU (wxID_PASTE,                   ChessFrame::OnEditPaste)
        EVT_UPDATE_UI (wxID_PASTE,                      ChessFrame::OnUpdateEditPaste)
    EVT_MENU (wxID_DELETE,                  ChessFrame::OnEditDelete)
        EVT_UPDATE_UI (wxID_DELETE,                     ChessFrame::OnUpdateEditDelete)
    EVT_MENU (ID_EDIT_GAME_DETAILS,         ChessFrame::OnEditGameDetails)
        EVT_UPDATE_UI (ID_EDIT_GAME_DETAILS,            ChessFrame::OnUpdateEditGameDetails)
    EVT_MENU (ID_EDIT_GAME_PREFIX,          ChessFrame::OnEditGamePrefix)
        EVT_UPDATE_UI (ID_EDIT_GAME_PREFIX,             ChessFrame::OnUpdateEditGamePrefix)
    EVT_MENU(ID_COPY_GAME_PGN_TO_CLIPBOARD, ChessFrame::OnEditCopyGamePGNToClipboard)
        EVT_UPDATE_UI(ID_COPY_GAME_PGN_TO_CLIPBOARD,    ChessFrame::OnUpdateEditCopyGamePGNToClipboard)
    EVT_MENU(ID_COPY_FEN_TO_CLIPBOARD,      ChessFrame::OnEditCopyFENToClipboard)
        EVT_UPDATE_UI(ID_COPY_FEN_TO_CLIPBOARD,         ChessFrame::OnUpdateEditCopyFENToClipboard)
    EVT_MENU (ID_EDIT_DELETE_VARIATION,     ChessFrame::OnEditDeleteVariation)
        EVT_UPDATE_UI (ID_EDIT_DELETE_VARIATION,        ChessFrame::OnUpdateEditDeleteVariation)
    EVT_MENU (ID_EDIT_PROMOTE,              ChessFrame::OnEditPromote)
        EVT_UPDATE_UI (ID_EDIT_PROMOTE,                 ChessFrame::OnUpdateEditPromote)
    EVT_MENU (ID_EDIT_DEMOTE,               ChessFrame::OnEditDemote)
        EVT_UPDATE_UI (ID_EDIT_DEMOTE,                  ChessFrame::OnUpdateEditDemote)
    EVT_MENU (ID_EDIT_DEMOTE_TO_COMMENT,    ChessFrame::OnEditDemoteToComment)
        EVT_UPDATE_UI (ID_EDIT_DEMOTE_TO_COMMENT,       ChessFrame::OnUpdateEditDemoteToComment)
    EVT_MENU (ID_EDIT_PROMOTE_TO_VARIATION, ChessFrame::OnEditPromoteToVariation)
        EVT_UPDATE_UI (ID_EDIT_PROMOTE_TO_VARIATION,    ChessFrame::OnUpdateEditPromoteToVariation)
    EVT_MENU (ID_EDIT_PROMOTE_REST_TO_VARIATION, ChessFrame::OnEditPromoteRestToVariation)
        EVT_UPDATE_UI (ID_EDIT_PROMOTE_REST_TO_VARIATION,    ChessFrame::OnUpdateEditPromoteRestToVariation)

    EVT_MENU (ID_OPTIONS_PLAYERS,  ChessFrame::OnPlayers)
        EVT_UPDATE_UI (ID_OPTIONS_PLAYERS,   ChessFrame::OnUpdatePlayers)
    EVT_MENU (ID_OPTIONS_CLOCKS,   ChessFrame::OnClocks)
        EVT_UPDATE_UI (ID_OPTIONS_CLOCKS,   ChessFrame::OnUpdateClocks)
    EVT_MENU (ID_OPTIONS_BOOK,     ChessFrame::OnBook)
        EVT_UPDATE_UI (ID_OPTIONS_BOOK,     ChessFrame::OnUpdateBook)
    EVT_MENU (ID_OPTIONS_LOG,      ChessFrame::OnLog)
        EVT_UPDATE_UI (ID_OPTIONS_LOG,      ChessFrame::OnUpdateLog)
    EVT_MENU (ID_OPTIONS_ENGINE,   ChessFrame::OnEngine)
        EVT_UPDATE_UI (ID_OPTIONS_ENGINE,   ChessFrame::OnUpdateEngine)
    EVT_MENU (ID_OPTIONS_TRAINING, ChessFrame::OnTraining)
        EVT_UPDATE_UI (ID_OPTIONS_TRAINING, ChessFrame::OnUpdateTraining)
    EVT_MENU (ID_OPTIONS_GENERAL,  ChessFrame::OnGeneral)
        EVT_UPDATE_UI (ID_OPTIONS_GENERAL,  ChessFrame::OnUpdateGeneral)
    EVT_MENU (ID_OPTIONS_RESET,    ChessFrame::OnOptionsReset)
        EVT_UPDATE_UI (ID_OPTIONS_RESET,    ChessFrame::OnUpdateOptionsReset)

    EVT_MENU (ID_DATABASE_SEARCH,                   ChessFrame::OnDatabaseSearch)
        EVT_UPDATE_UI (ID_DATABASE_SEARCH,               ChessFrame::OnUpdateDatabaseSearch)
    EVT_MENU (ID_DATABASE_SHOW_ALL,                 ChessFrame::OnDatabaseShowAll)
        EVT_UPDATE_UI (ID_DATABASE_SHOW_ALL,             ChessFrame::OnUpdateDatabaseShowAll)
    EVT_MENU (ID_DATABASE_PLAYERS,                  ChessFrame::OnDatabasePlayers)
        EVT_UPDATE_UI (ID_DATABASE_PLAYERS,              ChessFrame::OnUpdateDatabasePlayers)
    EVT_MENU (ID_DATABASE_SELECT,                   ChessFrame::OnDatabaseSelect)
        EVT_UPDATE_UI (ID_DATABASE_SELECT,              ChessFrame::OnUpdateDatabaseSelect)
    EVT_MENU (ID_DATABASE_CREATE,                   ChessFrame::OnDatabaseCreate)
        EVT_UPDATE_UI (ID_DATABASE_APPEND,              ChessFrame::OnUpdateDatabaseCreate)
    EVT_MENU (ID_DATABASE_APPEND,                   ChessFrame::OnDatabaseAppend)
        EVT_UPDATE_UI (ID_DATABASE_APPEND,              ChessFrame::OnUpdateDatabaseAppend)
    EVT_MENU (ID_DATABASE_PATTERN,                  ChessFrame::OnDatabasePattern)
        EVT_UPDATE_UI (ID_DATABASE_PATTERN,             ChessFrame::OnUpdateDatabasePattern)
    EVT_MENU (ID_DATABASE_MATERIAL,                 ChessFrame::OnDatabaseMaterial)
        EVT_UPDATE_UI (ID_DATABASE_MATERIAL,            ChessFrame::OnUpdateDatabaseMaterial)
    EVT_MENU (ID_DATABASE_MAINTENANCE,              ChessFrame::OnDatabaseMaintenance)
        EVT_UPDATE_UI (ID_DATABASE_MAINTENANCE,          ChessFrame::OnUpdateDatabaseMaintenance)
    EVT_MENU (ID_FILE_OPEN_SHELL,                    ChessFrame::OnFileOpenShell)	// Doesn't appear in any actual menu, used to open files from Windows Explorer
    EVT_MENU (ID_FILE_OPEN_SHELL_ASK,                ChessFrame::OnFileOpenShellAsk)	// Doesn't appear in any actual menu, used to open files from Windows Explorer

    EVT_TOOL (ID_CMD_FLIP,         ChessFrame::OnFlip)
    EVT_TOOL (ID_CMD_NEXT_GAME,    ChessFrame::OnNextGame)
    EVT_TOOL (ID_CMD_PREVIOUS_GAME,ChessFrame::OnPreviousGame)
    EVT_TOOL (ID_HELP_HELP,        ChessFrame::OnHelp)
    EVT_TOOL (ID_BUTTON_UP,        ChessFrame::OnButtonUp)
    EVT_TOOL (ID_BUTTON_DOWN,      ChessFrame::OnButtonDown)
    EVT_TOOL (ID_BUTTON_LEFT,      ChessFrame::OnButtonLeft)
    EVT_TOOL (ID_BUTTON_RIGHT,     ChessFrame::OnButtonRight)
    EVT_IDLE (ChessFrame::OnIdle)
    EVT_TIMER( TIMER_ID, ChessFrame::OnTimeout)
    EVT_MOVE (ChessFrame::OnMove)
    EVT_CHAR( ChessFrame::OnChar )
    EVT_MOUSEWHEEL( ChessFrame::OnMouseWheel )
END_EVENT_TABLE()



ChessFrame::ChessFrame(const wxString& title, const wxPoint& pos, const wxSize& size, bool pos_siz_restored )
    : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size ) //, wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN|
                                                            //        wxNO_FULL_REPAINT_ON_RESIZE )
{
    SetIcon(wxICON(bbbbbbbb));      // for explanation of 'bbbbbbbb' see Tarrasch.rc

    // Timer
    m_timer.SetOwner(this,TIMER_ID);

    // Menu - File
    wxMenu *menu_file     = new wxMenu;
    menu_file->Append (wxID_NEW,                    "New\tCtrl+N", "Start afresh with a new .pgn file with no games");
    menu_file->Append (wxID_OPEN,                   "Open\tCtrl+O", "Open an existing .pgn file" );
	wxMenu *menu_recent = new wxMenu;
	menu_file->AppendSubMenu( menu_recent, "Open Recent" );
    menu_file->Append (ID_FILE_OPEN_LOG,            "Open log file", "Open the file in which Tarrasch logs games");
    menu_file->Append (wxID_SAVE,                   "Save\tCtrl+S", "Save the current .pgn file" );
    menu_file->Append (wxID_SAVEAS,                 "Save as", "Save the current .pgn file with a different name");
    menu_file->Append (ID_FILE_SAVE_GAME_AS,        "Save game as", "Save the current game to a new or existing .pgn file");
    menu_file->AppendSeparator();
    menu_file->Append (ID_FILE_TAB_NEW,              "New tab\tCtrl+T");
    menu_file->Append (ID_FILE_TAB_CLOSE,            "Close tab\tCtrl+D");
    menu_file->Append (ID_FILE_TAB_INCLUDE,          "Add tab to file", "Add this game to the current file");
    menu_file->AppendSeparator();
    menu_file->Append (wxID_EXIT,                   "E&xit\tAlt-X");

    // Menu - Edit
    wxMenu *menu_edit     = new wxMenu;
    menu_edit->Append (wxID_COPY,                    "Copy\tCtrl+C");
    menu_edit->Append (wxID_CUT,                     "Cut\tCtrl+X");
    menu_edit->Append (wxID_PASTE,                   "Paste\tCtrl+V");
    menu_edit->Append (wxID_UNDO,                    "Undo\tCtrl+Z");
    menu_edit->Append (wxID_REDO,                    "Redo\tCtrl+Y");
    menu_edit->Append (wxID_DELETE,                  "Delete comment text or remainder of variation\tDel", "Delete selection or rest of variation");
    menu_edit->Append (ID_EDIT_GAME_DETAILS,         "Edit game details", "Modify player names, tournament names, or other details");
//    menu_edit->Append (ID_EDIT_GAME_PREFIX,          "Edit game prefix");
    menu_edit->Append (ID_COPY_GAME_PGN_TO_CLIPBOARD,"Copy game to system clipboard (PGN)");
    menu_edit->Append (ID_COPY_FEN_TO_CLIPBOARD,     "Copy position to system clipboard (FEN)");
    menu_edit->Append (ID_EDIT_DELETE_VARIATION,     "Delete variation", "Delete the whole variation");
    menu_edit->Append (ID_EDIT_PROMOTE,              "Promote variation", "The current variation is elevated higher in the hierarchy of variations");
    menu_edit->Append (ID_EDIT_DEMOTE,               "Demote variation", "The current variation is lowered in the hierarchy of variations");
    menu_edit->Append (ID_EDIT_DEMOTE_TO_COMMENT,    "Demote rest of variation to comment\tAlt-D", "Convert moves to plain text which can be edited");
    menu_edit->Append (ID_EDIT_PROMOTE_TO_VARIATION, "Promote comment to moves","Convert plain text to live moves");
    menu_edit->Append (ID_EDIT_PROMOTE_REST_TO_VARIATION, "Promote rest of comment to moves\tAlt-P","Convert plain text (from cursor to end of comment) to live moves");

    // Menu - Games
    wxMenu *menu_games   = new wxMenu;
    menu_games->Append (ID_GAMES_CURRENT,        "Current file\tCtrl+L", "Show the current file as a list of games");
    menu_games->Append (ID_GAMES_DATABASE,       "Database", "Do a database search, revealing a list of found games");
    menu_games->Append (ID_GAMES_SESSION,        "Session", "Show activity in this session as a list of games");
    menu_games->Append (ID_GAMES_CLIPBOARD,      "Clipboard", "Show the copied games as a list of games");
    menu_games->AppendSeparator();
    menu_games->Append (ID_CMD_NEXT_GAME,       "Next game from file");
    menu_games->Append (ID_CMD_PREVIOUS_GAME,   "Previous game from file");

    // Menu - Commands
    wxMenu *menu_commands = new wxMenu;
    menu_commands->Append (ID_CMD_NEW_GAME,     "New Game", "Start a new game, but leave existing .pgn file open" );
    menu_commands->Append (ID_CMD_FLIP,         "Flip board\tCtrl+F", "Reverse the current orientation, applies to Setup position and Games menu boards as well");
    menu_commands->Append (ID_CMD_TAKEBACK,     "Take back\tCtrl+B");
    menu_commands->Append (ID_CMD_SET_POSITION, "Set position\tCtrl+P", "Setup a new position");
    menu_commands->Append (ID_CMD_KIBITZ,       "Start kibitzer\tCtrl+K", "Turn computer analysis on or off");
    #if 1 // auto_kibitz_clear (if auto kibitz clear, comment out manual clear)
//  menu_commands->Append (ID_CMD_CLEAR_KIBITZ, "Clear kibitz text");
    #else // manual_kibitz_clear
    menu_commands->Append (ID_CMD_CLEAR_KIBITZ, "Clear kibitz text");
    #endif

    menu_commands->Append (ID_CMD_DRAW,             "Draw", "Indicate game result, or claim draw when playing against engine");
    menu_commands->Append (ID_CMD_WHITE_RESIGNS,    "White resigns", "Indicate game result, or concede when playing White against engine");
    menu_commands->Append (ID_CMD_BLACK_RESIGNS,    "Black resigns", "Indicate game result, or concede when playing Black against engine");
    menu_commands->Append (ID_CMD_PLAY_WHITE,       "Play white", "Play White against engine, starting from current position");
    menu_commands->Append (ID_CMD_PLAY_BLACK,       "Play black", "Play Black against engine, starting from current position");
    menu_commands->Append (ID_CMD_SWAP_SIDES,       "Swap sides\tAlt-S", "Reverse roles in game against engine" );
    menu_commands->Append (ID_CMD_MOVENOW,          "Move now\tAlt-M", "Force engine to move" );

    // Options
    wxMenu *menu_options = new wxMenu;
    menu_options->Append (ID_OPTIONS_ENGINE,       "Engine", "Specify the UCI engine to use, along with Engine options");
    menu_options->Append (ID_OPTIONS_GENERAL,      "General", "Miscellaneous options");
    menu_options->Append (ID_OPTIONS_PLAYERS,      "Player names", "Label the board with player names (also available by clicking on name)");
    menu_options->Append (ID_OPTIONS_CLOCKS,       "Clocks", "Control all aspects of the on-screen game timer");
    menu_options->Append (ID_OPTIONS_LOG,          "Log file", "Setup a file to automatically log all games");
    menu_options->Append (ID_OPTIONS_BOOK,         "Opening book", "Control a simple .pgn based opening book facility");
    menu_options->Append (ID_OPTIONS_TRAINING,     "Training", "Control some simple training modes to improve visualization skills");
    menu_options->Append (ID_OPTIONS_RESET,        "Reset to factory defaults", "Reset all options to their original values");

    // Database
    wxMenu *menu_database = new wxMenu;
    menu_database->Append (ID_DATABASE_SEARCH,              "Position search", "Search the database for the current position");
    menu_database->Append (ID_DATABASE_PATTERN,             "Pattern search", "Search the database for situations where a group of pieces are at specific locations");
    menu_database->Append (ID_DATABASE_MATERIAL,            "Material balance search", "Search the database for a specific material balance, with optional locked down squares" );
    menu_database->Append (ID_DATABASE_SHOW_ALL,            "Show all games", "Show all database games - equivalent to searching for the standard starting position");
    menu_database->Append (ID_DATABASE_PLAYERS,             "Show all ordered by player", "Show all games ordered by White player, useful for searching for players");
    menu_database->Append (ID_DATABASE_SELECT,              "Select current database", "Specify which database file to use for searches");
    menu_database->Append (ID_DATABASE_CREATE,              "Create new database", "Create a new database file from scratch");
    menu_database->Append (ID_DATABASE_APPEND,              "Append to database", "Append games to an existing database");
    // menu_database->Append (ID_DATABASE_MAINTENANCE,         "INTERNAL TEST - REMOVE SOON - Maintain database");

    // Help
    wxMenu *menu_help     = new wxMenu;
    menu_help->Append (ID_CMD_ABOUT,                "About", "Version and publisher information");
    menu_help->Append (ID_HELP_HELP,                "Help", "An overview of the program");
    menu_help->Append (ID_HELP_CREDITS,             "Credits", "Thank you to numerous people");

    // Menu bar
    wxMenuBar *menu = new wxMenuBar;
    menu->Append (menu_file,     "&File");
    menu->Append (menu_edit,     "&Edit");
    menu->Append (menu_games,    "&Games");
    menu->Append (menu_commands, "&Commands");
    menu->Append (menu_options,  "&Options");
    menu->Append (menu_database, "Database");
    menu->Append (menu_help,     "&Help");
    SetMenuBar( menu );

    // Create a status bar
    CreateStatusBar(4);
    int widths[4] = {-350,-90,-90,-120};
    SetStatusWidths(4,widths);

#ifdef THC_WINDOWS
    wxToolBar *toolbar = new wxToolBar(this, wxID_ANY,
                               wxDefaultPosition, wxDefaultSize,
                               wxTB_HORIZONTAL|wxNO_BORDER);
#else
    wxToolBar *toolbar = CreateToolBar(); //new wxToolBar(this, wxID_ANY,
                               //wxDefaultPosition, wxDefaultSize,
                               //wxTB_HORIZONTAL|wxNO_BORDER);
#endif

//  wxBitmap bmpNew    (new_xpm);
    wxBitmap bmpOpen   (open_xpm);
    wxBitmap bmpSave   (save_xpm);
    wxBitmap bmpCopy   (copy_xpm);
    wxBitmap bmpCut    (cut_xpm);
    wxBitmap bmpPaste  (paste_xpm);
    wxBitmap bmp_undo  (undo_xpm); //(bill_undo_xpm);
    wxBitmap bmp_redo  (redo_xpm); //(bill_redo_xpm);
    wxBitmap bmp_left  (left_xpm);
    wxBitmap bmp_right (right_xpm);
    wxBitmap bmp_up    (up_xpm);
    wxBitmap bmp_down  (down_xpm);
    wxBitmap bmp_flip  (flip_xpm);
    wxBitmap bmp_database  (database_xpm);
    wxBitmap bmp_help  (bill_help_xpm);
//  wxBitmap bmpNew    (bill_file_new_xpm);
//  wxBitmap bmp_game_new   (bill_game_new_xpm);
    wxBitmap bmpNew         (bill_doc_single_xpm);
    wxBitmap bmp_game_new   (bill_doc_stack_xpm);
    wxBitmap bmp_kibitzer   (bill_kibitzer_xpm);
    wxBitmap bmp_next       (bill_next_game_xpm);
    wxBitmap bmp_previous   (bill_previous_game_xpm);

    // I have modified all the icons to be 16 high (not 15) since a couple
    //  of the Reptor icons I brought across really needed 16.
    wxSize our_size(16,16);
    toolbar->SetToolBitmapSize(our_size);

    // Avoid an annoying deprecation warning
    #define ADD_TOOL(id,bm,txt) toolbar->AddTool( id,  wxEmptyString, bm, wxNullBitmap, wxITEM_NORMAL,txt, wxEmptyString, NULL)
    /*toolbar->AddTool( wxID_NEW,  wxEmptyString, bmpNew,
                        wxNullBitmap, wxITEM_NORMAL,
                        wxT("New file"), wxEmptyString, NULL); */


    ADD_TOOL( wxID_NEW,  bmpNew,  wxT("New file") );
    ADD_TOOL( ID_CMD_NEW_GAME,  bmp_game_new,  wxT("New game") );
    ADD_TOOL( wxID_OPEN, bmpOpen, wxT("Open") );
    ADD_TOOL( wxID_SAVE, bmpSave, wxT("Save") );
    int nbr_separators = 0;
#ifdef THC_WINDOWS
    toolbar->AddSeparator();
    nbr_separators++;
#endif
    toolbar->AddSeparator();
    nbr_separators++;
    ADD_TOOL( wxID_COPY, bmpCopy, "Copy" ); //, toolBarBitmaps[Tool_copy], _T("Toggle button 2"), wxITEM_CHECK);
    ADD_TOOL( wxID_CUT,  bmpCut,  "Cut"  ); //, toolBarBitmaps[Tool_cut], _T("Toggle/Untoggle help button"));
    ADD_TOOL( wxID_PASTE,bmpPaste,"Paste"); // , toolBarBitmaps[Tool_paste], _T("Paste"));
    ADD_TOOL( wxID_UNDO, bmp_undo, "Undo");
    ADD_TOOL( wxID_REDO, bmp_redo, "Redo");
#ifdef THC_WINDOWS
    toolbar->AddSeparator();
    nbr_separators++;
#endif
    toolbar->AddSeparator();
    nbr_separators++;
    ADD_TOOL( ID_CMD_FLIP,      bmp_flip, "Flip board");
    ADD_TOOL( ID_GAMES_DATABASE, bmp_database, "Database");
    ADD_TOOL( ID_CMD_KIBITZ,    bmp_kibitzer,  "Kibitzer start/stop");
    ADD_TOOL( ID_BUTTON_DOWN,   bmp_down,  "Forward (main line)");
    ADD_TOOL( ID_BUTTON_UP,     bmp_up,    "Backward (main line)");
    ADD_TOOL( ID_BUTTON_RIGHT,  bmp_right, "Forward" );
    ADD_TOOL( ID_BUTTON_LEFT,   bmp_left,  "Backward" );
#ifdef THC_WINDOWS
    toolbar->AddSeparator();
    nbr_separators++;
#endif
    toolbar->AddSeparator();
    nbr_separators++;
    ADD_TOOL( ID_CMD_NEXT_GAME,     bmp_next,      "Next game from file"     );
    ADD_TOOL( ID_CMD_PREVIOUS_GAME, bmp_previous,  "Previous game from file" );
#ifdef THC_WINDOWS
    toolbar->AddSeparator();
    nbr_separators++;
#endif
    toolbar->AddSeparator();
    nbr_separators++;
    ADD_TOOL( ID_HELP_HELP,  bmp_help, "Help");
    toolbar->Realize();
    SetToolBar(toolbar);

    wxSize csz = GetClientSize();
    int hh = csz.y;
    int ww = csz.x;

    // Some dynamic sizing code
    wxButton dummy( this, wxID_ANY, "Dummy" );
    wxFont font_temp( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,   false );
    dummy.SetFont(font_temp);
    wxClientDC dc(&dummy);
    wxCoord width, height, descent, external_leading;
    dc.GetTextExtent( "1000 Book moves", &width, &height, &descent, &external_leading );
    int min_book_moves_width = (width*110) / 100;
    cprintf( "Calculated book moves width (was 105)=%d\n", min_book_moves_width );
    wxSize sz_dummy = dummy.GetSize();
    int min_notebook_height = (sz_dummy.y*115) / 100;
    cprintf( "min_notebook_height (was 29) =%d\n", min_notebook_height );
    int height_context  = 120 < hh/2 ? 120 : hh/2;
    int height_notebook = min_notebook_height  < hh/4 ? min_notebook_height  : hh/4;
    int height_board    = hh - height_context - height_notebook;

    PanelNotebook *skinny = new PanelNotebook( this, -1, wxDefaultPosition, wxSize(ww,height_notebook),min_book_moves_width );

    PanelBoard *pb = new PanelBoard( this,
                            wxID_ANY,
                            wxDefaultPosition, wxSize(ww/2,height_board) );

    CtrlChessTxt *lb = new CtrlChessTxt( this, -1, /*ID_LIST_CTRL*/ wxDefaultPosition, wxSize(ww/2,height_board) );

    context = new PanelContext( this, -1, /*wxID_ANY*/ wxDefaultPosition, wxSize(ww,height_context), pb, lb );
    context->notebook = skinny->notebook;
    objs.canvas = context;

    // Init game logic only after everything setup
    objs.gl         = NULL;
    objs.session    = new Session;
    objs.log        = new Log;
    objs.book       = new Book;
    objs.cws        = new CentralWorkSaver;
    objs.tabs       = new Tabs;
    GameLogic *gl   = new GameLogic( context, lb, menu_recent );

    // Hook up connections to GameLogic
    objs.gl         = gl;
    gl->gd.gl       = gl;
    gl->gd.gv.gl    = gl;
    lb->gl          = gl;
    objs.tabs->gl   = gl;
    GameDocument    blank;
    objs.tabs->TabNew( blank );
    objs.cws->Init( &objs.gl->undo, &objs.gl->gd, &objs.gl->gc_pgn, &objs.gl->gc_clipboard ); 
    context->SetPlayers( "", "" );
    context->resize_ready = true;
	context->AuiBegin( this, skinny, pb, lb, context, pos_siz_restored );
}


void ChessFrame::OnClose( wxCloseEvent& WXUNUSED(event) )
{
    bool okay = objs.gl->OnExit();
    if( okay )
    {
		context->AuiEnd();
        wxSize sz = GetSize();
        wxPoint pt = GetPosition();
        if( objs.repository->nv.m_x == -1 &&
			objs.repository->nv.m_y == -1 &&
			objs.repository->nv.m_w == -1 &&
			objs.repository->nv.m_h == -1)
		{
			// Allow user to manually set factory defaults and retain this setting until next session
		}
		else
		{
			objs.repository->nv.m_x = pt.x;
			objs.repository->nv.m_y = pt.y;
			objs.repository->nv.m_w = sz.x;
			objs.repository->nv.m_h = sz.y;
		}
        Destroy();  // only exit if OnExit() worked okay (eg, if it wasn't cancelled)
    }
}

void ChessFrame::OnQuit (wxCommandEvent &)
{
    Close (true);
}

void ChessFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf(
        "\nThis is the Tarrasch Chess GUI V3. Tarrasch is "
        "an easy to use yet powerful system for playing and "
        "analysing with UCI standard chess engines. With V3 Tarrasch now adds "
        "fast and powerful database features."
        "\n\n"
        "Tarrasch includes the state of the art Stockfish engine, some other "
        "strong engines and the weak Tarrasch toy engine. The toy engine is "
        "particularly suited to Tarrasch's blindfold, partial blindfold, and N move lag training modes."
        "\n\n"
        "Visit the publisher's website www.triplehappy.com for Tarrasch "
        "Chess GUI news and updates."
    );
    DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
    wxMessageBox(msg, "About the Tarrasch Chess GUI " MASTER_VERSION, wxOK|wxICON_INFORMATION|wxCENTRE, this);
}

/*
void ChessFrame::OnUnimplemented(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf(
        "Sorry in this pre-beta version of Tarrasch V2, redo is not "
        "not yet implemented, and undo is simply takeback."
        "\n\n"
        "You can check whether a more complete version of the program "
        "is available at the publisher's main site triplehappy.com or "
        "on his blog at triplehappy.wordpress.com."
    );
    wxMessageBox(msg, "Sorry, unimplemented feature", wxOK|wxICON_INFORMATION|wxCENTRE, this);
}  */


void ChessFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf(
        "\nTarrasch is firstly an electronic chessboard. You can start "
        "from the standard starting position or set up any (legal) "
        "position easily. You can move the pieces around freely. A "
        "built in chess clock can be turned on, useful if you want to "
        "play an old fashioned normal game of chess with a friend."
        "\n\n"
        "Tarrasch has an opening book, you can easily see the \"book\" "
        "moves early in a game. Tarrasch uses the %s green when "
        "listing or suggesting book moves. The book can be turned off "
        "using menu \"Options\". That menu allows control over all "
        "optional features, including blindfold and delayed display "
        "training modes."
        "\n\n"
        "Your moves are recorded in a moves window as a main line "
        "plus variations. It is easy to promote or demote variations. "
        "You can freely type (and edit) comments to go with your moves. "
        "Just click in the moves "
        "window and start typing. Comments can be converted to moves "
        "and vice-versa."
        "\n\n"
        "At any time you can save your work in a new or existing .pgn "
        "file (a standard chess file format). You can load games from "
        "any .pgn file. Open a .pgn file in the \"File\" menu and "
        "select a game, later on the \"Games\" menu allows you to pick "
        "another game from the file."
        "\n\n"
        "You can use a UCI chess engine (UCI is another chess "
        "computing standard) either as an opponent, or to provide "
        "expert commentary."
        "\n\n"
        "The Tarrasch package includes one or more engines and one of "
        "these engines is used by default. Other "
        "engines can be separately purchased or downloaded. Use menu "
        "\"Options\" to select an alternative engine if you have one. "
        "You can start a game with the engine at any time, in any "
        "position, as either white or black. By default you will get "
        "a time handicap. You can change the clock settings by "
        "clicking on the clock, or using menu \"Options\". You can "
        "change player names by clicking on them or by using menu "
        "\"Options\"."
        "\n\n"
        "To turn on commentary (kibitzing in chess parlance), use "
        "the \"Commands\" menu or the Robot button or select the Engine Analysis tab at the bottom. You can even "
        "get the chess engine to provide commentary when you are "
        "playing against it."
        "\n\n"
		"Tarrasch now includes database facilities for easily finding "
		"games and move frequency statistics for openings you are interested in. You can search "
		"for positions, patterns and material balances and conveniently "
		"play through the games found."
		, gbl_spell_colour
    );
    DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
    wxMessageBox(msg, "Tarrasch Chess GUI Help", wxOK|wxICON_INFORMATION|wxCENTRE, this);
}

void ChessFrame::OnCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf(
        wxString::FromUTF8("\nProgram design and implementation: Bill Forster."
        "\n\n"
        "Foundation supporter: Yves Catineau."
        "\n\n"
        "Foundation donors: Paul Cook, Gary Baydo, Ransith Fernando, "
        "Lukasz Berezowski, Eric Ziegler, Laurence Dayton, Albrecht Schmidt, "
        "Lloyd Standish and David Beagan."
        "\n\n"
        "Testers: Iliya Kristoff, Claude Tamplenizza, Ian Sellen."
        "\n\n"
        "Thanks to David L Brown and the Good Companions for the chess "
        "graphics."
        "\n\n"
		"Thanks to Yusuke Kamiyamane (Fugue icons) for some of the toolbar icons."
        "\n\n"
        "Special thanks to Mark Crowther for permission to use TWIC to build tarrasch-base the default database, TWIC "
        "underlies all modern chess database curation, the chess community owes Mark a massive debt."
        "\n\n"
        "Thanks to Ed Schr" "\xC3\xB6" "der" " for Millionbase."
        "\n\n"
        "Thanks to the engine authors who provided explicit permission to "
        "include their engines. In chronological order, Vasik Rajlich (Rybka), "
        "the late Don Dailey and Larry Kaufman (Komodo), and Robert Houdart (Houdini)."
        "\n\n"
        "Thanks to the Stockfish team, Stockfish is now the default engine. "
        "Permission to include Stockfish is inherent in its licence, as long "
        "as the location of the Stockfish source code is provided. The "
        "location is https://stockfishchess.org."
        "\n\n"
        "Thanks to Inno Setup from Jordan Russell (jrsoftware.org), for "
        "the setup program."
        "\n\n"
        "Thanks to Julian Smart, Vadim Zeitlin and the wxWidgets community "
        "for the GUI library."
        "\n\n"
		"Thanks to Garry Peek and the Radium team (Norman Bartholomew, John Cocks, Jaron Peek, Tim McEwan, Brian Peach, Dr Garry Brown, Corwin Newall, Kate Hyndman and James Herbison) "
		"for providing office space and good company through much of the V3 development grind."
        "\n\n"
        "Thanks to my dear wife Maria Champion-Forster, for not only tolerating me "
		"spending (wasting?) thousands of hours on this project, but actually encouraging it."
        "\n\n"
        "Dedicated to the memory of John Victor Forster 1949-2001. We "
        "miss him every day.")
    );
    DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
    wxMessageBox(msg, "Credits", wxOK|wxICON_INFORMATION|wxCENTRE, this);
}



#if 0
void CustomLog( const char *txt )
{
    static FILE *log;
    if( log == NULL )
        log = fopen( "C:\\Documents and Settings\\Bill\\My Documents\\Chess\\Engines\\custom-log.txt", "wt" );
    if( log )
    {
        static char buf[1024];
        time_t t = time(NULL);
        struct tm *p = localtime(&t);
        const char *s = asctime(p);
        strcpy( buf, s );
        if( strchr(buf,'\n') )
            *strchr(buf,'\n') = '\0';
        fprintf( log, "[%s] %s", buf, txt );
        fflush(log);
    }
}
#endif

void ChessFrame::OnMove( wxMoveEvent &WXUNUSED(event) )
{
 //   if( objs.canvas )
 //       objs.canvas->OnMove();
}

void ChessFrame::OnIdle( wxIdleEvent& event )
{
    //CustomLog( "OnIdle()\n" );
    //if( objs.gl && objs.gl->OnIdleNeeded() )
    //    event.RequestMore();    // more intense idle calls required
    event.Skip();
    if( objs.gl )
    {
        objs.gl->OnIdle();
        int millisecs = 500;
        millisecs = objs.gl->MillisecsToNextSecond();
        if( millisecs < 100 )
            millisecs = 100;
        //char buf[80];
        //sprintf( buf, "Idle: %d milliseconds\n", millisecs );
        //CustomLog( buf );
        m_timer.Start( millisecs, true );
    }
}


void ChessFrame::OnTimeout( wxTimerEvent& WXUNUSED(event) )
{
    //CustomLog( "OnTimeout()\n" );
    if( objs.gl )
    {
        objs.gl->OnIdle();
        int millisecs = 1000;
        millisecs = objs.gl->MillisecsToNextSecond();
        if( millisecs < 100 )
            millisecs = 100;
        //char buf[80];
        //sprintf( buf, "Timeout: %d milliseconds\n", millisecs );
        //CustomLog( buf );
        m_timer.Start( millisecs, true );
    }
}

void ChessFrame::OnFlip (wxCommandEvent &)
{
    objs.gl->CmdFlip();
}

void ChessFrame::OnButtonUp (wxCommandEvent &)
{
    Atomic begin(false);
    objs.canvas->lb->NavigationKey( NK_UP );
}

void ChessFrame::OnButtonDown (wxCommandEvent &)
{
    Atomic begin(false);
    objs.canvas->lb->NavigationKey( NK_DOWN );
}

void ChessFrame::OnButtonLeft (wxCommandEvent &)
{
    Atomic begin(false);
    objs.canvas->lb->NavigationKey( NK_LEFT );
}

void ChessFrame::OnButtonRight (wxCommandEvent &)
{
    Atomic begin(false);
    objs.canvas->lb->NavigationKey( NK_RIGHT );
}

void ChessFrame::OnSetPosition (wxCommandEvent &)
{
    objs.gl->CmdSetPosition();
}

void ChessFrame::OnNewGame (wxCommandEvent &)
{
    objs.gl->CmdNewGame();
}

void ChessFrame::OnEditUndo (wxCommandEvent &)
{
    objs.gl->CmdEditUndo();
}

void ChessFrame::OnEditRedo (wxCommandEvent &)
{
    objs.gl->CmdEditRedo();
}

void ChessFrame::OnTakeback (wxCommandEvent &)
{
    objs.gl->CmdTakeback();
}

void ChessFrame::OnMoveNow (wxCommandEvent &)
{
    objs.gl->CmdMoveNow();
}

void ChessFrame::OnFileNew (wxCommandEvent &)
{
    objs.gl->CmdFileNew();
}

void ChessFrame::OnFileOpen (wxCommandEvent &)
{
    objs.gl->CmdFileOpen();
}

void ChessFrame::OnFileOpenShell(wxCommandEvent &)
{
    objs.gl->CmdFileOpenShell(objs.gl->filename_from_shell);
}

void ChessFrame::OnFileOpenShellAsk(wxCommandEvent &)
{
    objs.gl->CmdFileOpenShellAsk(objs.gl->filename_from_shell);
}

void ChessFrame::OnFileOpenMru(wxCommandEvent& event)
{
	int mru_idx = (event.GetId() - wxID_FILE1);
    objs.gl->CmdFileOpenMru( mru_idx );
}

void ChessFrame::OnFileOpenLog (wxCommandEvent &)
{
    objs.gl->CmdFileOpenLog();
}

void ChessFrame::OnFileSave (wxCommandEvent &)
{
    objs.gl->CmdFileSave();
}

void ChessFrame::OnFileSaveAs (wxCommandEvent &)
{
    objs.gl->CmdFileSaveAs();
}

void ChessFrame::OnFileSaveGameAs (wxCommandEvent &)
{
    objs.gl->CmdFileSaveGameAs();
}

void ChessFrame::OnGamesCurrent (wxCommandEvent &)
{
    objs.gl->CmdGamesCurrent();
}

void ChessFrame::OnGamesSession (wxCommandEvent &)
{
    objs.gl->CmdGamesSession();
}

void ChessFrame::OnGamesClipboard (wxCommandEvent &)
{
    objs.gl->CmdGamesClipboard();
}

void ChessFrame::OnNextGame (wxCommandEvent &)
{
    objs.gl->CmdNextGame();
}

void ChessFrame::OnUpdateNextGame( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateNextGame() : false;
    event.Enable(enabled);
}

void ChessFrame::OnPreviousGame (wxCommandEvent &)
{
    objs.gl->CmdPreviousGame();
}

void ChessFrame::OnUpdatePreviousGame( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdatePreviousGame() : false;
    event.Enable(enabled);
}

void ChessFrame::OnEditCopy (wxCommandEvent &)
{
    objs.canvas->lb->Copy();
}

void ChessFrame::OnEditCut (wxCommandEvent &)
{
    objs.canvas->lb->Cut();
}

void ChessFrame::OnEditPaste (wxCommandEvent &)
{
    objs.canvas->lb->Paste();
}

void ChessFrame::OnEditDelete (wxCommandEvent &)
{
    objs.canvas->lb->Delete();
}

void ChessFrame::OnEditGameDetails (wxCommandEvent &)
{
    objs.gl->CmdEditGameDetails();
}

void ChessFrame::OnEditGamePrefix (wxCommandEvent &)
{
    objs.gl->CmdEditGamePrefix();
}

void ChessFrame::OnEditCopyGamePGNToClipboard(wxCommandEvent &)
{
    objs.gl->CmdEditCopyGamePGNToClipboard();
}

void ChessFrame::OnEditCopyFENToClipboard(wxCommandEvent &)
{
    objs.gl->CmdEditCopyFENToClipboard();
}

void ChessFrame::OnEditDeleteVariation (wxCommandEvent &)
{
    objs.gl->CmdEditDeleteVariation();
}

void ChessFrame::OnEditPromote (wxCommandEvent &)
{
    objs.gl->CmdEditPromote();
}

void ChessFrame::OnEditDemote (wxCommandEvent &)
{
    objs.gl->CmdEditDemote();
}

void ChessFrame::OnEditDemoteToComment (wxCommandEvent &)
{
    objs.gl->CmdEditDemoteToComment();
}

void ChessFrame::OnEditPromoteToVariation (wxCommandEvent &)
{
    objs.gl->CmdEditPromoteToVariation();
}

void ChessFrame::OnEditPromoteRestToVariation (wxCommandEvent &)
{
    objs.gl->CmdEditPromoteRestToVariation();
}

void ChessFrame::OnUpdateFileNew( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateFileOpen( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateFileOpen() : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateFileOpenLog( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateFileOpenLog()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateGamesCurrent( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateGamesCurrent()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateFileSave( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateFileSaveAs( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateFileSaveGameAs( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateGamesDatabase( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateGamesSession( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateGamesClipboard( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateGamesClipboard()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditCopy( wxUpdateUIEvent &event )
{
    bool enabled = objs.canvas ? (objs.canvas->lb->GetStringSelection().Length() > 0) : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditCut( wxUpdateUIEvent &event )
{
    bool enabled = objs.canvas ? (objs.canvas->lb->GetStringSelection().Length() > 0) : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditPaste( wxUpdateUIEvent &event )
{
#ifdef WINDOWS_FIX_LATER    
    bool enabled = (wxTheClipboard->Open() && wxTheClipboard->IsSupported( wxDF_TEXT ));
    event.Enable(enabled);
#endif
}

void ChessFrame::OnUpdateEditUndo( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateEditUndo()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditRedo( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateEditRedo()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditDelete( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditGameDetails( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditGamePrefix( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditCopyGamePGNToClipboard(wxUpdateUIEvent &event)
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditCopyFENToClipboard(wxUpdateUIEvent &event)
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditDeleteVariation( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditPromote( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditDemote( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditDemoteToComment( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}


void ChessFrame::OnUpdateEditPromoteToVariation( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateEditPromoteRestToVariation( wxUpdateUIEvent &event )
{
    bool enabled = true;
    event.Enable(enabled);
}

void ChessFrame::OnDraw(wxCommandEvent &)
{
    objs.gl->CmdDraw();
}

void ChessFrame::OnWhiteResigns(wxCommandEvent &)
{
    objs.gl->CmdWhiteResigns();
}

void ChessFrame::OnUpdateWhiteResigns( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateWhiteResigns()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnBlackResigns(wxCommandEvent &)
{
    objs.gl->CmdBlackResigns();
}

void ChessFrame::OnUpdateBlackResigns( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateBlackResigns()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateTakeback( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateTakeback()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateMoveNow( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateMoveNow()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateDraw( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateDraw()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdatePlayWhite( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdatePlayWhite()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdatePlayBlack( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdatePlayBlack()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateSwapSides( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateSwapSides()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateTabNew( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateTabNew()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateTabClose( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateTabClose() : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateTabInclude( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateTabInclude() : false;
    event.Enable(enabled);
}

void ChessFrame::OnUpdateKibitz( wxUpdateUIEvent &event )
{
    wxString txt;
    bool enabled = objs.gl ? objs.gl->CmdUpdateKibitz( txt ) : false;
    event.SetText( txt );
    event.Enable(enabled);
}

void ChessFrame::OnUpdateClearKibitz( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->CmdUpdateClearKibitz() : false;
    event.Enable(enabled);
}

void ChessFrame::OnPlayWhite (wxCommandEvent &)
{
    objs.gl->CmdPlayWhite();
}

void ChessFrame::OnPlayBlack (wxCommandEvent &)
{
    objs.gl->CmdPlayBlack();
}

void ChessFrame::OnSwapSides (wxCommandEvent &)
{
    objs.gl->CmdSwapSides();
}

void ChessFrame::OnTabNew (wxCommandEvent &)
{
    objs.gl->CmdTabNew();
}

void ChessFrame::OnTabClose (wxCommandEvent &)
{
    objs.gl->CmdTabClose();
}

void ChessFrame::OnTabInclude (wxCommandEvent &)
{
    objs.gl->CmdTabInclude();
}

void ChessFrame::OnKibitz (wxCommandEvent &)
{
    objs.gl->CmdKibitz();
}

void ChessFrame::OnClearKibitz (wxCommandEvent &)
{
    objs.gl->CmdClearKibitz( true );
}

void ChessFrame::OnPlayers(wxCommandEvent &)
{
    objs.gl->CmdPlayers();
}

void ChessFrame::OnUpdatePlayers(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions() : false;
    event.Enable(enabled);
}

void ChessFrame::OnClocks(wxCommandEvent &)
{
    objs.gl->CmdClocks();
}

void ChessFrame::OnUpdateClocks(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions() : false;
    event.Enable(enabled);
}

void ChessFrame::OnOptionsReset(wxCommandEvent &)
{
    bool ingame=false;
    bool     old_enabled = objs.repository->book.m_enabled;
    wxString old_file    = objs.repository->book.m_file;
    wxString old_engine_file  = objs.repository->engine.m_file;
    const char *from = LangCheckDiffBegin();
    bool before_heading_above_board = objs.repository->general.m_heading_above_board;
    int  before_font_size = objs.repository->general.m_font_size;
    bool before_no_italics = objs.repository->general.m_no_italics;
    delete objs.repository;
    objs.repository = new Repository(true);
    LangSet( objs.repository->general.m_notation_language );
    if( objs.repository->engine.m_file != old_engine_file )
        ingame = objs.gl->EngineChanged();
    if( objs.repository->book.m_enabled != old_enabled ||
        objs.repository->book.m_file    != old_file )
    {
        if( objs.repository->book.m_enabled )
        {
            wxString error_msg;
            bool error = objs.book->Load( error_msg, objs.repository->book.m_file );
            if( error )
            {
                DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
                wxMessageBox( error_msg, "Error loading book", wxOK|wxICON_ERROR );
            }
            objs.canvas->BookUpdate( false );
        }
    }
    if( ingame )
    {
        objs.gl->chess_clock.Clocks2Repository();
        objs.gl->chess_clock.GameStart( objs.gl->gd.master_position.WhiteToPlay() );
    }
    else
        objs.gl->chess_clock.Repository2Clocks();
    context->RedrawClocks();
    const char *to = LangCheckDiffEnd();
    bool after_heading_above_board = objs.repository->general.m_heading_above_board;
    int  after_font_size = objs.repository->general.m_font_size;
    bool after_no_italics = objs.repository->general.m_no_italics;
    RefreshLanguageFont( from, before_font_size, before_no_italics,
                           to,  after_font_size,  after_no_italics );
    if( before_heading_above_board != after_heading_above_board )
	{
		context->AuiRefresh();
		context->pb->SetBoardTitle();
	}
	context->pb->gb->Redraw();
    SetFocusOnList();
}

void ChessFrame::OnUpdateOptionsReset(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions() : false;
    event.Enable(enabled);
}

void ChessFrame::OnBook(wxCommandEvent &)
{
    BookDialog dialog( objs.repository->book, this );
    bool     old_enabled = objs.repository->book.m_enabled;
    wxString old_file    = objs.repository->book.m_file;
    if( wxID_OK == dialog.ShowModal() )
    {
        objs.repository->book = dialog.dat;
        if( objs.repository->book.m_enabled != old_enabled ||
            objs.repository->book.m_file    != old_file )
        {
            if( objs.repository->book.m_enabled )
            {
                wxString error_msg;
                bool error = objs.book->Load( error_msg, objs.repository->book.m_file );
                if( error )
                {
                    DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
                    wxMessageBox( error_msg, "Error loading book", wxOK|wxICON_ERROR );
                }
                objs.canvas->BookUpdate( false );
            }
            objs.gl->Refresh();
        }
    }
    SetFocusOnList();
}

void ChessFrame::OnUpdateBook(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions() : false;
    event.Enable(enabled);
}

void ChessFrame::OnLog(wxCommandEvent &)
{
    LogDialog dialog( objs.repository->log, this );
    if( wxID_OK == dialog.ShowModal() )
    {
        objs.repository->log = dialog.dat;
    }
    SetFocusOnList();
}

void ChessFrame::OnUpdateLog(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnEngine(wxCommandEvent &)
{
    wxString old_file     = objs.repository->engine.m_file;
	bool old_normal_priority = objs.repository->engine.m_normal_priority;
	bool old_low_priority = objs.repository->engine.m_low_priority;
	bool old_idle_priority = objs.repository->engine.m_idle_priority;
    EngineDialog dialog( objs.repository->engine, this );
    if( wxID_OK == dialog.ShowModal() )
    {
        objs.repository->engine = dialog.dat;
        const char *s = dialog.dat.m_file.c_str();
        cprintf( "file=%s\n", s );
        if( old_file != objs.repository->engine.m_file ||
			old_normal_priority != objs.repository->engine.m_normal_priority ||
			old_low_priority != objs.repository->engine.m_low_priority ||
			old_idle_priority != objs.repository->engine.m_idle_priority )
		{
            objs.gl->EngineChanged();
        }
    }
    SetFocusOnList();
}

void ChessFrame::OnGamesDatabase (wxCommandEvent &)
{
    objs.gl->CmdGamesDatabase();
}

void ChessFrame::OnDatabaseSearch(wxCommandEvent &)
{
    objs.gl->CmdDatabaseSearch();
}

void ChessFrame::OnDatabaseShowAll(wxCommandEvent &)
{
    objs.gl->CmdDatabaseShowAll();
}

void ChessFrame::OnDatabasePlayers(wxCommandEvent &)
{
    objs.gl->CmdDatabasePlayers();
}

void ChessFrame::OnDatabaseSelect(wxCommandEvent &)
{
    objs.gl->CmdDatabaseSelect();
}

void ChessFrame::OnDatabaseCreate(wxCommandEvent &)
{
    objs.gl->CmdDatabaseCreate();
}

void ChessFrame::OnDatabaseAppend(wxCommandEvent &)
{
    objs.gl->CmdDatabaseAppend();
}

void ChessFrame::OnDatabasePattern(wxCommandEvent &)
{
    objs.gl->CmdDatabasePattern();
}

void ChessFrame::OnDatabaseMaterial(wxCommandEvent &)
{
    objs.gl->CmdDatabaseMaterial();
}

void ChessFrame::OnUpdateDatabaseSearch(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabaseShowAll(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabasePlayers(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabaseSelect(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabaseCreate(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabaseAppend(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabasePattern(wxUpdateUIEvent &)
{
}

void ChessFrame::OnUpdateDatabaseMaterial(wxUpdateUIEvent &)
{
}

void ChessFrame::OnDatabaseMaintenance(wxCommandEvent &)
{
    wxString old_file    = objs.repository->engine.m_file;
    MaintenanceDialog dialog( objs.repository->engine, this );
    if( wxID_OK == dialog.ShowModal() )
    {
        objs.repository->engine = dialog.dat;
        if( old_file != objs.repository->engine.m_file )
            objs.gl->EngineChanged();
    }
    SetFocusOnList();
}

void ChessFrame::OnUpdateDatabaseMaintenance( wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions()  : false;
    event.Enable(enabled);
}


void ChessFrame::OnUpdateEngine(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnGeneral(wxCommandEvent &)
{
    const char *from = LangCheckDiffBegin();
    bool before_heading_above_board = objs.repository->general.m_heading_above_board;
    int  before_font_size = objs.repository->general.m_font_size;
    bool before_no_italics = objs.repository->general.m_no_italics;
    GeneralDialog dialog( this, &objs.repository->general );
    if( wxID_OK == dialog.ShowModal() )
    {
        //bool changed = ( 0 != memcmp(&dialog.dat,&objs.repository->training,sizeof(dialog.dat))  );
        objs.repository->general = dialog.dat;
        //if( changed )
        //    objs.gl->SetGroomedPosition();
        const char *s=dialog.dat.m_notation_language.c_str();
        cprintf( "notation language=%s, no italics=%d, straight to game=%d\n",
                                 s, dialog.dat.m_no_italics,
                                 dialog.dat.m_straight_to_game );
        const char *to = LangCheckDiffEnd();
		bool after_heading_above_board = objs.repository->general.m_heading_above_board;
        int  after_font_size = objs.repository->general.m_font_size;
        bool after_no_italics = objs.repository->general.m_no_italics;
        RefreshLanguageFont( from, before_font_size, before_no_italics,
                             to,   after_font_size,  after_no_italics );

		// Change board colours
		if( before_heading_above_board != after_heading_above_board )
		{
			context->AuiRefresh();
			context->pb->SetBoardTitle();
		}
		objs.canvas->pb->gb->Redraw();
    }
    SetFocusOnList();
}

void ChessFrame::RefreshLanguageFont( const char *UNUSED(from), int before_font_size, bool before_no_italics,
                                      const char *to,   int after_font_size,  bool after_no_italics )
{
	if( before_font_size != after_font_size )
		objs.gl->lb->SetFontSize();
    bool redisplayed = false;
    if( to )
    {

        // If the language has changed, redisplay
        long pos = objs.gl->lb->GetInsertionPoint();
        objs.gl->gd.Rebuild();
        objs.gl->gd.Redisplay(pos);
        redisplayed = true;
    }

    // If the italics setting has changed, redisplay
    else if( before_no_italics != after_no_italics )
    {
        long pos = objs.gl->lb->GetInsertionPoint();
        objs.gl->gd.Rebuild();
        objs.gl->gd.Redisplay(pos);
        redisplayed = true;
    }

    // If no redisplay yet, and font change, redisplay
    if(before_font_size!=after_font_size && !redisplayed )
    {
        long pos = objs.gl->lb->GetInsertionPoint();
        objs.gl->gd.Rebuild();
        objs.gl->gd.Redisplay(pos);
    }
    objs.gl->Refresh();
}

void ChessFrame::OnUpdateGeneral(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnTraining(wxCommandEvent &)
{
    TrainingDialog dialog( this );
    dialog.dat = objs.repository->training;
    if( wxID_OK == dialog.ShowModal() )
    {
        //bool changed = ( 0 != memcmp(&dialog.dat,&objs.repository->training,sizeof(dialog.dat))  );
        objs.repository->training = dialog.dat;
        //if( changed )
        //    objs.gl->SetGroomedPosition();
        dbg_printf( "nbr_half=%d, hide wpawn=%d, hide wpiece=%d,"
                                 " hide bpawn=%d, hide bpiece=%d\n",
                                 dialog.dat.m_nbr_half_moves_behind,
                                 dialog.dat.m_blindfold_hide_white_pawns,
                                 dialog.dat.m_blindfold_hide_white_pieces,
                                 dialog.dat.m_blindfold_hide_black_pawns,
                                 dialog.dat.m_blindfold_hide_black_pieces );
    }
    objs.gl->Refresh();
    SetFocusOnList();
}

void ChessFrame::OnUpdateTraining(wxUpdateUIEvent &event )
{
    bool enabled = objs.gl ? objs.gl->UpdateOptions()  : false;
    event.Enable(enabled);
}

void ChessFrame::OnMouseWheel( wxMouseEvent &event )
{
    if( objs.canvas )
        objs.canvas->lb->GetEventHandler()->ProcessEvent(event);
}

void ChessFrame::OnChar( wxKeyEvent &event )
{
    if( objs.canvas )
        objs.canvas->lb->GetEventHandler()->ProcessEvent(event);
}

