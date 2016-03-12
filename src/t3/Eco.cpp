/****************************************************************************
 *  ECO opening codes
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <map>
#include "thc.h"
#include "Eco.h"
#include "GameDocument.h"
#include "DebugPrintf.h"
#include "CompressMoves.h"

struct ECO_CODE
{
    const char *eco_code;
    const char *moves_txt;
    std::string compressed_moves;
    uint64_t    hash;
    thc::ChessPosition position;
};


// eco4.txt is based on exeterchessclub.org.uk/content/eco-codes
// eco5.txt is based on chessgames.com/chessecohelp.html
ECO_CODE eco_codes[] =
    {
{ "A00", "" },
{ "A01", "1. b3" },
{ "A02", "1. f4" },
{ "A03", "1. f4 d5" },
{ "A04", "1. Nf3" },
{ "A05", "1. Nf3 Nf6" },
{ "A06", "1. Nf3 d5" },
{ "A07", "1. Nf3 d5 2. g3" },
{ "A08", "1. Nf3 d5 2. g3 c5 3. Bg2" },
{ "A09", "1. Nf3 d5 2. c4" },
{ "A10", "1. c4" },
{ "A11", "1. c4 c6" },
{ "A12", "1. c4 c6 2. Nf3 d5 3. b3" },
{ "A13", "1. c4 e6" },
{ "A14", "1. c4 e6 2. Nf3 d5 3. g3 Nf6 4. Bg2 Be7 5. O-O" },
{ "A15", "1. c4 Nf6" },
{ "A16", "1. c4 Nf6 2. Nc3" },
{ "A17", "1. c4 Nf6 2. Nc3 e6" },
{ "A18", "1. c4 Nf6 2. Nc3 e6 3. e4" },
{ "A19", "1. c4 Nf6 2. Nc3 e6 3. e4 c5" },
{ "A20", "1. c4 e5" },
{ "A21", "1. c4 e5 2. Nc3" },
{ "A22", "1. c4 e5 2. Nc3 Nf6" },
{ "A23", "1. c4 e5 2. Nc3 Nf6 3. g3 c6" },
{ "A24", "1. c4 e5 2. Nc3 Nf6 3. g3 g6" },
{ "A25", "1. c4 e5 2. Nc3 Nc6" },
{ "A26", "1. c4 e5 2. Nc3 Nc6 3. g3 g6 4. Bg2 Bg7 5. d3 d6" },
{ "A27", "1. c4 e5 2. Nc3 Nc6 3. Nf3" },
{ "A28", "1. c4 e5 2. Nc3 Nc6 3. Nf3 Nf6" },
{ "A29", "1. c4 e5 2. Nc3 Nc6 3. Nf3 Nf6 4. g3" },
{ "A30", "1. c4 c5" },
{ "A31", "1. c4 c5 2. Nf3 Nf6 3. d4" },
{ "A32", "1. c4 c5 2. Nf3 Nf6 3. d4 cxd4 4. Nxd4 e6" },
{ "A33", "1. c4 c5 2. Nf3 Nf6 3. d4 cxd4 4. Nxd4 e6 5. Nc3 Nc6" },
{ "A34", "1. c4 c5 2. Nc3" },
{ "A35", "1. c4 c5 2. Nc3 Nc6" },
{ "A36", "1. c4 c5 2. Nc3 Nc6 3. g3" },
{ "A37", "1. c4 c5 2. Nc3 Nc6 3. g3 g6 4. Bg2 Bg7 5. Nf3" },
{ "A38", "1. c4 c5 2. Nc3 Nc6 3. g3 g6 4. Bg2 Bg7 5. Nf3 Nf6" },
{ "A39", "1. c4 c5 2. Nc3 Nc6 3. g3 g6 4. Bg2 Bg7 5. Nf3 Nf6 6. O-O O-O 7. d4" },
{ "A40", "1. d4" },
{ "A41", "1. d4 d6" },
{ "A42", "1. d4 d6 2. c4 g6 3. Nc3 Bg7 4. e4" },
{ "A43", "1. d4 c5" },
{ "A44", "1. d4 c5 2. d5 e5" },
{ "A45", "1. d4 Nf6" },
{ "A46", "1. d4 Nf6 2. Nf3" },
{ "A47", "1. d4 Nf6 2. Nf3 b6" },
{ "A48", "1. d4 Nf6 2. Nf3 g6" },
{ "A49", "1. d4 Nf6 2. Nf3 g6 3. g3" },
{ "A50", "1. d4 Nf6 2. c4" },
{ "A51", "1. d4 Nf6 2. c4 e5" },
{ "A52", "1. d4 Nf6 2. c4 e5 3. dxe5 Ng4" },
{ "A53", "1. d4 Nf6 2. c4 d6" },
{ "A54", "1. d4 Nf6 2. c4 d6 3. Nc3 e5 4. Nf3" },
{ "A55", "1. d4 Nf6 2. c4 d6 3. Nc3 e5 4. Nf3 Nbd7 5. e4" },
{ "A56", "1. d4 Nf6 2. c4 c5" },
{ "A57", "1. d4 Nf6 2. c4 c5 3. d5 b5" },
{ "A58", "1. d4 Nf6 2. c4 c5 3. d5 b5 4. cxb5 a6 5. bxa6" },
{ "A59", "1. d4 Nf6 2. c4 c5 3. d5 b5 4. cxb5 a6 5. bxa6 Bxa6 6. Nc3 d6 7. e4" },
{ "A60", "1. d4 Nf6 2. c4 c5 3. d5 e6" },
{ "A61", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. Nf3 g6" },
{ "A62", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. Nf3 g6 7. g3 Bg7 8. Bg2 O-O" },
{ "A63", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. Nf3 g6 7. g3 Bg7 8. Bg2 O-O 9. O-O Nbd7" },
{ "A64", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. Nf3 g6 7. g3 Bg7 8. Bg2 O-O 9. O-O Nbd7 10. Nd2 a6 11. a4 Re8" },
{ "A65", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4" },
{ "A66", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. f4" },
{ "A67", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. f4 Bg7 8. Bb5+" },
{ "A68", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. f4 Bg7 8. Nf3 O-O" },
{ "A69", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. f4 Bg7 8. Nf3 O-O 9. Be2 Re8" },
{ "A70", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3" },
{ "A71", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Bg5" },
{ "A72", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O" },
{ "A73", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O" },
{ "A74", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O a6" },
{ "A75", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O a6 10. a4 Bg4" },
{ "A76", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O Re8" },
{ "A77", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O Re8 10. Nd2" },
{ "A78", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O Re8 10. Nd2 Na6" },
{ "A79", "1. d4 Nf6 2. c4 c5 3. d5 e6 4. Nc3 exd5 5. cxd5 d6 6. e4 g6 7. Nf3 Bg7 8. Be2 O-O 9. O-O Re8 10. Nd2 Na6 11. f3 Nc7" },
{ "A80", "1. d4 f5" },
{ "A81", "1. d4 f5 2. g3" },
{ "A82", "1. d4 f5 2. e4" },
{ "A83", "1. d4 f5 2. e4 fxe4 3. Nc3 Nf6 4. Bg5" },
{ "A84", "1. d4 f5 2. c4" },
{ "A85", "1. d4 f5 2. c4 Nf6 3. Nc3" },
{ "A86", "1. d4 f5 2. c4 Nf6 3. g3" },
{ "A87", "1. d4 f5 2. c4 Nf6 3. g3 g6 4. Bg2 Bg7 5. Nf3" },
{ "A88", "1. d4 f5 2. c4 Nf6 3. g3 g6 4. Bg2 Bg7 5. Nf3 O-O 6. O-O d6 7. Nc3 c6" },
{ "A89", "1. d4 f5 2. c4 Nf6 3. g3 g6 4. Bg2 Bg7 5. Nf3 O-O 6. O-O d6 7. Nc3 Nc6" },
{ "A90", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2" },
{ "A91", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7" },
{ "A92", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O" },
{ "A93", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d5 7. b3" },
{ "A94", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d5 7. b3 c6 8. Ba3" },
{ "A95", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d5 7. Nc3 c6" },
{ "A96", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d6" },
{ "A97", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d6 7. Nc3 Qe8" },
{ "A98", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d6 7. Nc3 Qe8 8. Qc2" },
{ "A99", "1. d4 f5 2. c4 Nf6 3. g3 e6 4. Bg2 Be7 5. Nf3 O-O 6. O-O d6 7. Nc3 Qe8 8. b3" },
{ "B00", "1. e4" },
{ "B01", "1. e4 d5" },
{ "B02", "1. e4 Nf6" },
{ "B03", "1. e4 Nf6 2. e5 Nd5 3. d4" },
{ "B04", "1. e4 Nf6 2. e5 Nd5 3. d4 d6 4. Nf3" },
{ "B05", "1. e4 Nf6 2. e5 Nd5 3. d4 d6 4. Nf3 Bg4" },
{ "B06", "1. e4 g6" },
{ "B07", "1. e4 d6 2. d4 Nf6" },
{ "B08", "1. e4 d6 2. d4 Nf6 3. Nc3 g6 4. Nf3" },
{ "B09", "1. e4 d6 2. d4 Nf6 3. Nc3 g6 4. f4" },
{ "B10", "1. e4 c6" },
{ "B11", "1. e4 c6 2. Nc3 d5 3. Nf3 Bg4" },
{ "B12", "1. e4 c6 2. d4" },
{ "B13", "1. e4 c6 2. d4 d5 3. exd5 cxd5" },
{ "B14", "1. e4 c6 2. d4 d5 3. exd5 cxd5 4. c4 Nf6 5. Nc3 e6" },
{ "B15", "1. e4 c6 2. d4 d5 3. Nc3" },
{ "B16", "1. e4 c6 2. d4 d5 3. Nc3 dxe4 4. Nxe4 Nf6 5. Nxf6+ gxf6" },
{ "B17", "1. e4 c6 2. d4 d5 3. Nc3 dxe4 4. Nxe4 Nd7" },
{ "B18", "1. e4 c6 2. d4 d5 3. Nc3 dxe4 4. Nxe4 Bf5" },
{ "B19", "1. e4 c6 2. d4 d5 3. Nc3 dxe4 4. Nxe4 Bf5 5. Ng3 Bg6 6. h4 h6 7. Nf3 Nd7" },
{ "B20", "1. e4 c5" },
{ "B21", "1. e4 c5 2. f4" },
{ "B22", "1. e4 c5 2. c3" },
{ "B23", "1. e4 c5 2. Nc3" },
{ "B24", "1. e4 c5 2. Nc3 Nc6 3. g3" },
{ "B25", "1. e4 c5 2. Nc3 Nc6 3. g3 g6 4. Bg2 Bg7 5. d3 d6" },
{ "B26", "1. e4 c5 2. Nc3 Nc6 3. g3 g6 4. Bg2 Bg7 5. d3 d6 6. Be3" },
{ "B27", "1. e4 c5 2. Nf3" },
{ "B28", "1. e4 c5 2. Nf3 a6" },
{ "B29", "1. e4 c5 2. Nf3 Nf6" },
{ "B30", "1. e4 c5 2. Nf3 Nc6" },
{ "B31", "1. e4 c5 2. Nf3 Nc6 3. Bb5 g6" },
{ "B32", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 e5" },
{ "B33", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4" },
{ "B34", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. Nxc6" },
{ "B35", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. Nc3 Bg7 6. Be3 Nf6 7. Bc4" },
{ "B36", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. c4" },
{ "B37", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. c4 Bg7" },
{ "B38", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. c4 Bg7 6. Be3" },
{ "B39", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. c4 Bg7 6. Be3 Nf6 7. Nc3 Ng4" },
{ "B40", "1. e4 c5 2. Nf3 e6" },
{ "B41", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 a6" },
{ "B42", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 a6 5. Bd3" },
{ "B43", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 a6 5. Nc3" },
{ "B44", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 Nc6" },
{ "B45", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 Nc6 5. Nc3" },
{ "B46", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 Nc6 5. Nc3 a6" },
{ "B47", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 Nc6 5. Nc3 Qc7" },
{ "B48", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 Nc6 5. Nc3 Qc7 6. Be3" },
{ "B49", "1. e4 c5 2. Nf3 e6 3. d4 cxd4 4. Nxd4 Nc6 5. Nc3 Qc7 6. Be3 a6 7. Be2" },
{ "B50", "1. e4 c5 2. Nf3 d6" },
{ "B51", "1. e4 c5 2. Nf3 d6 3. Bb5+" },
{ "B52", "1. e4 c5 2. Nf3 d6 3. Bb5+ Bd7" },
{ "B53", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Qxd4" },
{ "B54", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4" },
{ "B55", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. f3 e5 6. Bb5+" },
{ "B56", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3" },
{ "B57", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bc4" },
{ "B58", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 d6 6. Be2" },
{ "B59", "1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 d6 6. Be2 e5 7. Nb3" },
{ "B60", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5" },
{ "B61", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 Bd7 7. Qd2" },
{ "B62", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6" },
{ "B63", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2" },
{ "B64", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2 Be7 8. O-O-O O-O 9. f4" },
{ "B65", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2 Be7 8. O-O-O O-O 9. f4 Nxd4 10. Qxd4" },
{ "B66", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2 a6" },
{ "B67", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2 a6 8. O-O-O Bd7" },
{ "B68", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2 a6 8. O-O-O Bd7 9. f4 Be7" },
{ "B69", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 Nc6 6. Bg5 e6 7. Qd2 a6 8. O-O-O Bd7 9. f4 Be7 10. Nf3 b5 11. Bxf6" },
{ "B70", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6" },
{ "B71", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. f4" },
{ "B72", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3" },
{ "B73", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. Be2 Nc6 8. O-O" },
{ "B74", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. Be2 Nc6 8. O-O O-O 9. Nb3" },
{ "B75", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. f3" },
{ "B76", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. f3 O-O" },
{ "B77", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. f3 O-O 8. Qd2 Nc6 9. Bc4" },
{ "B78", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. f3 O-O 8. Qd2 Nc6 9. Bc4 Bd7 10. O-O-O" },
{ "B79", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 g6 6. Be3 Bg7 7. f3 O-O 8. Qd2 Nc6 9. Bc4 Bd7 10. O-O-O Qa5 11. Bb3 Rfc8 12. h4" },
{ "B80", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6" },
{ "B81", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. g4" },
{ "B82", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. f4" },
{ "B83", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Be2" },
{ "B84", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Be2 a6" },
{ "B85", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Be2 a6 7. O-O Qc7 8. f4 Nc6" },
{ "B86", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Bc4" },
{ "B87", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Bc4 a6 7. Bb3 b5" },
{ "B88", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Bc4 Nc6" },
{ "B89", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 e6 6. Bc4 Nc6 7. Be3" },
{ "B90", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6" },
{ "B91", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. g3" },
{ "B92", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Be2" },
{ "B93", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. f4" },
{ "B94", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Bg5" },
{ "B95", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Bg5 e6" },
{ "B96", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Bg5 e6 7. f4" },
{ "B97", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Bg5 e6 7. f4 Qb6" },
{ "B98", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Bg5 e6 7. f4 Be7" },
{ "B99", "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Bg5 e6 7. f4 Be7 8. Qf3 Qc7 9. O-O-O Nbd7" },
{ "C00", "1. e4 e6" },
{ "C01", "1. e4 e6 2. d4 d5 3. exd5 exd5 4. Nc3 Nf6 5. Bg5" },
{ "C02", "1. e4 e6 2. d4 d5 3. e5" },
{ "C03", "1. e4 e6 2. d4 d5 3. Nd2" },
{ "C04", "1. e4 e6 2. d4 d5 3. Nd2 Nc6 4. Ngf3 Nf6" },
{ "C05", "1. e4 e6 2. d4 d5 3. Nd2 Nf6" },
{ "C06", "1. e4 e6 2. d4 d5 3. Nd2 Nf6 4. e5 Nfd7 5. Bd3 c5 6. c3 Nc6 7. Ne2 cxd4 8. cxd4" },
{ "C07", "1. e4 e6 2. d4 d5 3. Nd2 c5" },
{ "C08", "1. e4 e6 2. d4 d5 3. Nd2 c5 4. exd5 exd5" },
{ "C09", "1. e4 e6 2. d4 d5 3. Nd2 c5 4. exd5 exd5 5. Ngf3 Nc6" },
{ "C10", "1. e4 e6 2. d4 d5 3. Nc3" },
{ "C11", "1. e4 e6 2. d4 d5 3. Nc3 Nf6" },
{ "C12", "1. e4 e6 2. d4 d5 3. Nc3 Nf6 4. Bg5 Bb4" },
{ "C13", "1. e4 e6 2. d4 d5 3. Nc3 Nf6 4. Bg5 Be7" },
{ "C14", "1. e4 e6 2. d4 d5 3. Nc3 Nf6 4. Bg5 Be7 5. e5 Nfd7 6. Bxe7 Qxe7" },
{ "C15", "1. e4 e6 2. d4 d5 3. Nc3 Bb4" },
{ "C16", "1. e4 e6 2. d4 d5 3. Nc3 Bb4 4. e5" },
{ "C17", "1. e4 e6 2. d4 d5 3. Nc3 Bb4 4. e5 c5" },
{ "C18", "1. e4 e6 2. d4 d5 3. Nc3 Bb4 4. e5 c5 5. a3 Bxc3+ 6. bxc3" },
{ "C19", "1. e4 e6 2. d4 d5 3. Nc3 Bb4 4. e5 c5 5. a3 Bxc3+ 6. bxc3 Ne7" },
{ "C20", "1. e4 e5" },
{ "C21", "1. e4 e5 2. d4 exd4" },
{ "C22", "1. e4 e5 2. d4 exd4 3. Qxd4 Nc6" },
{ "C23", "1. e4 e5 2. Bc4" },
{ "C24", "1. e4 e5 2. Bc4 Nf6" },
{ "C25", "1. e4 e5 2. Nc3" },
{ "C26", "1. e4 e5 2. Nc3 Nf6" },
{ "C27", "1. e4 e5 2. Nc3 Nf6 3. Bc4 Nxe4" },
{ "C28", "1. e4 e5 2. Nc3 Nf6 3. Bc4 Nc6" },
{ "C29", "1. e4 e5 2. Nc3 Nf6 3. f4" },
{ "C30", "1. e4 e5 2. f4" },
{ "C31", "1. e4 e5 2. f4 d5" },
{ "C32", "1. e4 e5 2. f4 d5 3. exd5 e4 4. d3 Nf6" },
{ "C33", "1. e4 e5 2. f4 exf4" },
{ "C34", "1. e4 e5 2. f4 exf4 3. Nf3" },
{ "C35", "1. e4 e5 2. f4 exf4 3. Nf3 Be7" },
{ "C36", "1. e4 e5 2. f4 exf4 3. Nf3 d5" },
{ "C37", "1. e4 e5 2. f4 exf4 3. Nf3 g5 4. Nc3" },
{ "C38", "1. e4 e5 2. f4 exf4 3. Nf3 g5 4. Bc4 Bg7" },
{ "C39", "1. e4 e5 2. f4 exf4 3. Nf3 g5 4. h4" },
{ "C40", "1. e4 e5 2. Nf3" },
{ "C41", "1. e4 e5 2. Nf3 d6" },
{ "C42", "1. e4 e5 2. Nf3 Nf6" },
{ "C43", "1. e4 e5 2. Nf3 Nf6 3. d4 exd4 4. e5 Ne4 5. Qxd4" },
{ "C44", "1. e4 e5 2. Nf3 Nc6" },
{ "C45", "1. e4 e5 2. Nf3 Nc6 3. d4 exd4 4. Nxd4" },
{ "C46", "1. e4 e5 2. Nf3 Nc6 3. Nc3" },
{ "C47", "1. e4 e5 2. Nf3 Nc6 3. Nc3 Nf6" },
{ "C48", "1. e4 e5 2. Nf3 Nc6 3. Nc3 Nf6 4. Bb5" },
{ "C49", "1. e4 e5 2. Nf3 Nc6 3. Nc3 Nf6 4. Bb5 Bb4" },
{ "C50", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5" },
{ "C51", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5 4. b4" },
{ "C52", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5 4. b4 Bxb4 5. c3 Ba5" },
{ "C53", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5 4. c3" },
{ "C54", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5 4. c3 Nf6 5. d4 exd4 6. cxd4" },
{ "C55", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Nf6" },
{ "C56", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Nf6 4. d4 exd4 5. O-O Nxe4" },
{ "C57", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Nf6 4. Ng5" },
{ "C58", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Nf6 4. Ng5 d5 5. exd5 Na5" },
{ "C59", "1. e4 e5 2. Nf3 Nc6 3. Bc4 Nf6 4. Ng5 d5 5. exd5 Na5 6. Bb5+ c6 7. dxc6 bxc6 8. Be2 h6" },
{ "C60", "1. e4 e5 2. Nf3 Nc6 3. Bb5" },
{ "C61", "1. e4 e5 2. Nf3 Nc6 3. Bb5 Nd4" },
{ "C62", "1. e4 e5 2. Nf3 Nc6 3. Bb5 d6" },
{ "C63", "1. e4 e5 2. Nf3 Nc6 3. Bb5 f5" },
{ "C64", "1. e4 e5 2. Nf3 Nc6 3. Bb5 Bc5" },
{ "C65", "1. e4 e5 2. Nf3 Nc6 3. Bb5 Nf6" },
{ "C66", "1. e4 e5 2. Nf3 Nc6 3. Bb5 Nf6 4. O-O d6" },
{ "C67", "1. e4 e5 2. Nf3 Nc6 3. Bb5 Nf6 4. O-O Nxe4" },
{ "C68", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6" },
{ "C69", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 dxc6 5. O-O f6 6. d4" },
{ "C70", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4" },
{ "C71", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 d6" },
{ "C72", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 d6 5. O-O" },
{ "C73", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 d6 5. Bxc6+ bxc6 6. d4" },
{ "C74", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 d6 5. c3" },
{ "C75", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 d6 5. c3 Bd7" },
{ "C76", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 d6 5. c3 Bd7 6. d4 g6" },
{ "C77", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6" },
{ "C78", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O" },
{ "C79", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O d6" },
{ "C80", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Nxe4" },
{ "C81", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Nxe4 6. d4 b5 7. Bb3 d5 8. dxe5 Be6 9. Qe2" },
{ "C82", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Nxe4 6. d4 b5 7. Bb3 d5 8. dxe5 Be6 9. c3" },
{ "C83", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Nxe4 6. d4 b5 7. Bb3 d5 8. dxe5 Be6" },
{ "C84", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7" },
{ "C85", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Bxc6 dxc6" },
{ "C86", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Qe2" },
{ "C87", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 d6" },
{ "C88", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3" },
{ "C89", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d5" },
{ "C90", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6" },
{ "C91", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. d4" },
{ "C92", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3" },
{ "C93", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 h6" },
{ "C94", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 Nb8" },
{ "C95", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 Nb8 10. d4" },
{ "C96", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 Na5 10. Bc2" },
{ "C97", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 Na5 10. Bc2 c5 11. d4 Qc7" },
{ "C98", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 Na5 10. Bc2 c5 11. d4 Qc7 12. Nbd2 Nc6" },
{ "C99", "1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 O-O 8. c3 d6 9. h3 Na5 10. Bc2 c5 11. d4 Qc7 12. Nbd2 cxd4 13. cxd4" },
{ "D00", "1. d4 d5" },
{ "D01", "1. d4 d5 2. Nc3 Nf6 3. Bg5" },
{ "D02", "1. d4 d5 2. Nf3" },
{ "D03", "1. d4 d5 2. Nf3 Nf6 3. Bg5" },
{ "D04", "1. d4 d5 2. Nf3 Nf6 3. e3" },
{ "D05", "1. d4 d5 2. Nf3 Nf6 3. e3 e6" },
{ "D06", "1. d4 d5 2. c4" },
{ "D07", "1. d4 d5 2. c4 Nc6" },
{ "D08", "1. d4 d5 2. c4 e5" },
{ "D09", "1. d4 d5 2. c4 e5 3. dxe5 d4 4. Nf3 Nc6 5. g3" },
{ "D10", "1. d4 d5 2. c4 c6" },
{ "D11", "1. d4 d5 2. c4 c6 3. Nf3" },
{ "D12", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. e3 Bf5" },
{ "D13", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. cxd5 cxd5" },
{ "D14", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. cxd5 cxd5 5. Nc3 Nc6 6. Bf4 Bf5" },
{ "D15", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. Nc3" },
{ "D16", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. Nc3 dxc4 5. a4" },
{ "D17", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. Nc3 dxc4 5. a4 Bf5" },
{ "D18", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. Nc3 dxc4 5. a4 Bf5 6. e3" },
{ "D19", "1. d4 d5 2. c4 c6 3. Nf3 Nf6 4. Nc3 dxc4 5. a4 Bf5 6. e3 e6 7. Bxc4 Bb4 8. O-O O-O 9. Qe2" },
{ "D20", "1. d4 d5 2. c4 dxc4" },
{ "D21", "1. d4 d5 2. c4 dxc4 3. Nf3" },
{ "D22", "1. d4 d5 2. c4 dxc4 3. Nf3 a6 4. e3 Bg4 5. Bxc4 e6 6. d5" },
{ "D23", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6" },
{ "D24", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6 4. Nc3" },
{ "D25", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6 4. e3" },
{ "D26", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6 4. e3 e6" },
{ "D27", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6 4. e3 e6 5. Bxc4 c5 6. O-O a6" },
{ "D28", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6 4. e3 e6 5. Bxc4 c5 6. O-O a6 7. Qe2" },
{ "D29", "1. d4 d5 2. c4 dxc4 3. Nf3 Nf6 4. e3 e6 5. Bxc4 c5 6. O-O a6 7. Qe2 b5 8. Bb3 Bb7" },
{ "D30", "1. d4 d5 2. c4 e6" },
{ "D31", "1. d4 d5 2. c4 e6 3. Nc3" },
{ "D32", "1. d4 d5 2. c4 e6 3. Nc3 c5" },
{ "D33", "1. d4 d5 2. c4 e6 3. Nc3 c5 4. cxd5 exd5 5. Nf3 Nc6 6. g3" },
{ "D34", "1. d4 d5 2. c4 e6 3. Nc3 c5 4. cxd5 exd5 5. Nf3 Nc6 6. g3 Nf6 7. Bg2 Be7" },
{ "D35", "1. d4 d5 2. c4 e6 3. Nc3 Nf6" },
{ "D36", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. cxd5 exd5 5. Bg5 c6 6. Qc2" },
{ "D37", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3" },
{ "D38", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 Bb4" },
{ "D39", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 Bb4 5. Bg5 dxc4" },
{ "D40", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c5" },
{ "D41", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c5 5. cxd5" },
{ "D42", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c5 5. cxd5 Nxd5 6. e3 Nc6 7. Bd3" },
{ "D43", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6" },
{ "D44", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6 5. Bg5 dxc4" },
{ "D45", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6 5. e3" },
{ "D46", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6 5. e3 Nbd7 6. Bd3" },
{ "D47", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6 5. e3 Nbd7 6. Bd3 dxc4 7. Bxc4" },
{ "D48", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6 5. e3 Nbd7 6. Bd3 dxc4 7. Bxc4 b5 8. Bd3 a6" },
{ "D49", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Nf3 c6 5. e3 Nbd7 6. Bd3 dxc4 7. Bxc4 b5 8. Bd3 a6 9. e4 c5 10. e5 cxd4 11. Nxb5" },
{ "D50", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5" },
{ "D51", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Nbd7" },
{ "D52", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Nbd7 5. e3 c6 6. Nf3" },
{ "D53", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7" },
{ "D54", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Rc1" },
{ "D55", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3" },
{ "D56", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 h6 7. Bh4" },
{ "D57", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 h6 7. Bh4 Ne4 8. Bxe7 Qxe7" },
{ "D58", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 h6 7. Bh4 b6" },
{ "D59", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 h6 7. Bh4 b6 8. cxd5 Nxd5" },
{ "D60", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7" },
{ "D61", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Qc2" },
{ "D62", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Qc2 c5 8. cxd5" },
{ "D63", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1" },
{ "D64", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1 c6 8. Qc2" },
{ "D65", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1 c6 8. Qc2 a6 9. cxd5" },
{ "D66", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1 c6 8. Bd3" },
{ "D67", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1 c6 8. Bd3 dxc4 9. Bxc4 Nd5" },
{ "D68", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1 c6 8. Bd3 dxc4 9. Bxc4 Nd5 10. Bxe7 Qxe7 11. O-O Nxc3 12. Rxc3 e5" },
{ "D69", "1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 Be7 5. e3 O-O 6. Nf3 Nbd7 7. Rc1 c6 8. Bd3 dxc4 9. Bxc4 Nd5 10. Bxe7 Qxe7 11. O-O Nxc3 12. Rxc3 e5 13. dxe5 Nxe5 14. Nxe5 Qxe5" },
{ "D70", "1. d4 Nf6 2. c4 g6 3. f3 d5" },
{ "D71", "1. d4 Nf6 2. c4 g6 3. g3 d5" },
{ "D72", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. cxd5 Nxd5 6. e4 Nb6 7. Ne2" },
{ "D73", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3" },
{ "D74", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3 O-O 6. cxd5 Nxd5 7. O-O" },
{ "D75", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3 O-O 6. cxd5 Nxd5 7. O-O c5 8. dxc5" },
{ "D76", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3 O-O 6. cxd5 Nxd5 7. O-O Nb6" },
{ "D77", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3 O-O 6. O-O" },
{ "D78", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3 O-O 6. O-O c6" },
{ "D79", "1. d4 Nf6 2. c4 g6 3. g3 d5 4. Bg2 Bg7 5. Nf3 O-O 6. O-O c6 7. cxd5 cxd5" },
{ "D80", "1. d4 Nf6 2. c4 g6 3. Nc3 d5" },
{ "D81", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Qb3" },
{ "D82", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Bf4" },
{ "D83", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Bf4 Bg7 5. e3 O-O" },
{ "D84", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Bf4 Bg7 5. e3 O-O 6. cxd5 Nxd5 7. Nxd5 Qxd5 8. Bxc7" },
{ "D85", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. cxd5 Nxd5" },
{ "D86", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. cxd5 Nxd5 5. e4 Nxc3 6. bxc3 Bg7 7. Bc4" },
{ "D87", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. cxd5 Nxd5 5. e4 Nxc3 6. bxc3 Bg7 7. Bc4 O-O 8. Ne2 c5" },
{ "D88", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. cxd5 Nxd5 5. e4 Nxc3 6. bxc3 Bg7 7. Bc4 O-O 8. Ne2 c5 9. O-O Nc6 10. Be3 cxd4 11. cxd4" },
{ "D89", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. cxd5 Nxd5 5. e4 Nxc3 6. bxc3 Bg7 7. Bc4 O-O 8. Ne2 c5 9. O-O Nc6 10. Be3 cxd4 11. cxd4 Bg4 12. f3 Na5 13. Bd3" },
{ "D90", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3" },
{ "D91", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Bg5" },
{ "D92", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Bf4" },
{ "D93", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Bf4 O-O 6. e3" },
{ "D94", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. e3" },
{ "D95", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. e3 O-O 6. Qb3" },
{ "D96", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Qb3" },
{ "D97", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Qb3 dxc4 6. Qxc4 O-O 7. e4" },
{ "D98", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Qb3 dxc4 6. Qxc4 O-O 7. e4 Bg4" },
{ "D99", "1. d4 Nf6 2. c4 g6 3. Nc3 d5 4. Nf3 Bg7 5. Qb3 dxc4 6. Qxc4 O-O 7. e4 Bg4 8. Be3" },
{ "E00", "1. d4 Nf6 2. c4 e6" },
{ "E01", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2" },
{ "E02", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 dxc4 5. Qa4+" },
{ "E03", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 dxc4 5. Qa4+ Nbd7 6. Qxc4" },
{ "E04", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 dxc4 5. Nf3" },
{ "E05", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 dxc4 5. Nf3 Be7" },
{ "E06", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 Be7 5. Nf3" },
{ "E07", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 Be7 5. Nf3 O-O 6. O-O Nbd7" },
{ "E08", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 Be7 5. Nf3 O-O 6. O-O Nbd7 7. Qc2" },
{ "E09", "1. d4 Nf6 2. c4 e6 3. g3 d5 4. Bg2 Be7 5. Nf3 O-O 6. O-O Nbd7 7. Qc2 c6 8. Nbd2" },
{ "E10", "1. d4 Nf6 2. c4 e6 3. Nf3" },
{ "E11", "1. d4 Nf6 2. c4 e6 3. Nf3 Bb4+" },
{ "E12", "1. d4 Nf6 2. c4 e6 3. Nf3 b6" },
{ "E13", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. Nc3 Bb7 5. Bg5 h6 6. Bh4 Bb4" },
{ "E14", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. e3" },
{ "E15", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. g3" },
{ "E16", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. g3 Bb7 5. Bg2 Bb4+" },
{ "E17", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. g3 Bb7 5. Bg2 Be7" },
{ "E18", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. g3 Bb7 5. Bg2 Be7 6. O-O O-O 7. Nc3" },
{ "E19", "1. d4 Nf6 2. c4 e6 3. Nf3 b6 4. g3 Bb7 5. Bg2 Be7 6. O-O O-O 7. Nc3 Ne4 8. Qc2 Nxc3" },
{ "E20", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4" },
{ "E21", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Nf3" },
{ "E22", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qb3" },
{ "E23", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qb3 c5 5. dxc5 Nc6" },
{ "E24", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3" },
{ "E25", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3 c5 6. f3 d5 7. cxd5" },
{ "E26", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3 c5 6. e3" },
{ "E27", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3 O-O" },
{ "E28", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3 O-O 6. e3" },
{ "E29", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3 O-O 6. e3 c5 7. Bd3 Nc6" },
{ "E30", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Bg5" },
{ "E31", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Bg5 h6 5. Bh4 c5 6. d5 d6" },
{ "E32", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2" },
{ "E33", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 Nc6" },
{ "E34", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 d5" },
{ "E35", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 d5 5. cxd5 exd5" },
{ "E36", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 d5 5. a3" },
{ "E37", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 d5 5. a3 Bxc3+ 6. Qxc3 Ne4 7. Qc2" },
{ "E38", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 c5" },
{ "E39", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. Qc2 c5 5. dxc5 O-O" },
{ "E40", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3" },
{ "E41", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 c5" },
{ "E42", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 c5 5. Ne2" },
{ "E43", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 b6" },
{ "E44", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 b6 5. Ne2" },
{ "E45", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 b6 5. Ne2 Ba6" },
{ "E46", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O" },
{ "E47", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Bd3" },
{ "E48", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Bd3 d5" },
{ "E49", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Bd3 d5 6. a3 Bxc3+ 7. bxc3" },
{ "E50", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3" },
{ "E51", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5" },
{ "E52", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 b6" },
{ "E53", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5" },
{ "E54", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5 7. O-O dxc4 8. Bxc4" },
{ "E55", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5 7. O-O dxc4 8. Bxc4 Nbd7" },
{ "E56", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5 7. O-O Nc6" },
{ "E57", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5 7. O-O Nc6 8. a3 dxc4 9. Bxc4" },
{ "E58", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5 7. O-O Nc6 8. a3 Bxc3 9. bxc3" },
{ "E59", "1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. e3 O-O 5. Nf3 d5 6. Bd3 c5 7. O-O Nc6 8. a3 Bxc3 9. bxc3 dxc4 10. Bxc4" },
{ "E60", "1. d4 Nf6 2. c4 g6" },
{ "E61", "1. d4 Nf6 2. c4 g6 3. Nc3" },
{ "E62", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3" },
{ "E63", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 Nc6 7. O-O a6" },
{ "E64", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 c5" },
{ "E65", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 c5 7. O-O" },
{ "E66", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 c5 7. O-O Nc6 8. d5" },
{ "E67", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 Nbd7" },
{ "E68", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 Nbd7 7. O-O e5 8. e4" },
{ "E69", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. Nf3 d6 5. g3 O-O 6. Bg2 Nbd7 7. O-O e5 8. e4 c6 9. h3" },
{ "E70", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4" },
{ "E71", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. h3" },
{ "E72", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. g3" },
{ "E73", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Be2" },
{ "E74", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Be2 O-O 6. Bg5 c5" },
{ "E75", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Be2 O-O 6. Bg5 c5 7. d5 e6" },
{ "E76", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f4" },
{ "E77", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f4 O-O 6. Be2" },
{ "E78", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f4 O-O 6. Be2 c5 7. Nf3" },
{ "E79", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f4 O-O 6. Be2 c5 7. Nf3 cxd4 8. Nxd4 Nc6 9. Be3" },
{ "E80", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3" },
{ "E81", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O" },
{ "E82", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 b6" },
{ "E83", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 Nc6" },
{ "E84", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 Nc6 7. Nge2 a6 8. Qd2 Rb8" },
{ "E85", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 e5" },
{ "E86", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 e5 7. Nge2 c6" },
{ "E87", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 e5 7. d5" },
{ "E88", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 e5 7. d5 c6" },
{ "E89", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. f3 O-O 6. Be3 e5 7. d5 c6 8. Nge2 cxd5" },
{ "E90", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3" },
{ "E91", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2" },
{ "E92", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5" },
{ "E93", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. d5 Nbd7" },
{ "E94", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. O-O" },
{ "E95", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. O-O Nbd7 8. Re1" },
{ "E96", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. O-O Nbd7 8. Re1 c6 9. Bf1 a5" },
{ "E97", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. O-O Nc6" },
{ "E98", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. O-O Nc6 8. d5 Ne7 9. Ne1" },
{ "E99", "1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 e5 7. O-O Nc6 8. d5 Ne7 9. Ne1 Nd7 10. f3 f5" }
    };

static std::map<uint64_t,int> lookup;
static void eco_codes_inner();
static void eco_test();
static void eco_regen();
static void eco_begin()
{
    static bool once;
    if( !once )
    {
        once = true;
        //cprintf( "in\n" );
        //unsigned long t1 = time(NULL);
        for( int i=0; i<1; i++ )
            eco_codes_inner();
        //unsigned long t2 = time(NULL);
        //eco_regen();
        //cprintf( "out - elapsed %d\n", (int)(t2-t1) );
        //for( int i=0; i<10; i++ )
        //    cprintf( "%lld\n", eco_codes[i].hash );
        //eco_test();
    }
}

static void eco_regen()
{
    eco_begin();
    FILE *ofile = fopen( "/users/bill/documents/github/tarrasch-chess-gui/eco-regen.txt", "wt" );
    int nbr = sizeof(eco_codes)/(sizeof(eco_codes[0]));
    for( int i=0; ofile && i<nbr; i++ )
    {
        ECO_CODE *p = &eco_codes[i];
        const char *in = p->eco_code;
        std::string blob = p->compressed_moves;
        CompressMoves press;
        std::vector<thc::Move> v = press.Uncompress(blob);
        thc::ChessRules cr;
        std::string txt;
        for( int j=0; j<v.size(); j++ )
        {
            thc::Move mv = v[j];
            char buf[100];
            if( cr.white )
                sprintf( buf, "%s%d. ", j==0?"":" ", cr.full_move_count );
            else
                sprintf( buf, " " );
            txt += std::string(buf);
            txt += mv.NaturalOut(&cr);
            cr.PlayMove(mv);
        }
        fprintf( ofile, "{ \"%s\", \"%s\" }%s\n", in, txt.c_str(), i+1<nbr?",":"" );
    }
    if( ofile )
        fclose( ofile );
}


static void eco_test()
{
    eco_begin();
    int nbr = sizeof(eco_codes)/(sizeof(eco_codes[0]));
    int failures=0;
    for( int i=0; i<nbr; i++ )
    {
        ECO_CODE *p = &eco_codes[i];
        const char *in = p->eco_code;
        std::string s = p->compressed_moves;
        const char *out = eco_calculate(s);
        bool match = (0==strcmp(in,out));
        if( !match )
            failures++;
        if( !match )
            cprintf( "%s: in %s, out %s\n", match?"match":"fail", in, out );
    }
    cprintf( "%d failures\n", failures );
}

const char *eco_ref( const char *eco_in )
{
    eco_begin();
    int nbr = sizeof(eco_codes)/(sizeof(eco_codes[0]));
    for( int i=0; i<nbr; i++ )
    {
        ECO_CODE *p = &eco_codes[i];
        const char *rover = p->eco_code;
        bool match = (0==strcmp(eco_in,rover));
        if( match )
            return( p->moves_txt );
    }
    return("");
}


static void eco_codes_inner()
{
    int nbr = sizeof(eco_codes)/(sizeof(eco_codes[0]));
    for( int i=0; i<nbr; i++ )
    {
        GameDocument gd;
        ECO_CODE *p = &eco_codes[i];
        thc::ChessRules cr;
        int nbr_converted;
        bool ok = gd.PgnParse(true,nbr_converted,p->moves_txt,cr,NULL);
        //cprintf( "ok=%s\n", ok?"true":"false" );
        if( ok )
        {
            std::string s = gd.RefCompressedMoves();
            p->compressed_moves = s;
            //cprintf( "%s: %d moves\n", eco_codes[i].eco_code, s.length() );
        }
    }
    thc::ChessRules cr_temp;
    uint64_t hash_base = cr_temp.Hash64Calculate();
    for( int i=0; i<nbr; i++ )
    {
        ECO_CODE *p = &eco_codes[i];
        CompressMoves press;
        std::vector<thc::Move> v = press.Uncompress(p->compressed_moves);
        #if 1 
        thc::ChessRules cr;
        uint64_t hash = hash_base;
        for( int j=0; j<v.size(); j++ )
        {
            thc::Move mv = v[j];
            hash = cr.Hash64Update(hash, mv );
            cr.PlayMove( mv );
        }
        p->hash = hash;
        p->position = cr;
        #else
        p->hash = press.cr.Hash64Calculate();
        p->position = press.cr;
        #endif
        lookup[hash] = i;
    }
}

const char *eco_calculate( std::string &compressed_moves )
{
    CompressMoves press;
    std::vector<thc::Move> v = press.Uncompress(compressed_moves);
    return eco_calculate( v );
}

const char *eco_calculate( const std::vector<thc::Move> &moves )
{
    eco_begin();
    int best_so_far=0;
    thc::ChessRules cr;
    uint64_t hash = cr.Hash64Calculate();
    for( int i=0; i<30 && i<moves.size(); i++ )
    {
        thc::Move mv = moves[i];
        hash = cr.Hash64Update( hash, mv );
        cr.PlayMove( mv );
        std::map<uint64_t,int>::iterator it = lookup.find(hash);
        if( it != lookup.end() )
        {
            int found = it->second;
            if( eco_codes[found].compressed_moves.length() >= eco_codes[best_so_far].compressed_moves.length() && eco_codes[found].position==cr )
                best_so_far = found;
        }
    }
    return eco_codes[best_so_far].eco_code;
}

