/****************************************************************************
 * Generate a list of all possible moves in a position
 ****************************************************************************/
void ChessRules::GenMoveList( MOVELIST *l )
{
    Square square;

    // Convenient spot for some asserts
    //  Have a look at TestInternals() for this,
    //   A ChessPositionRaw should finish with 32 bits of detail information
    //   (see DETAIL macros, this assert() checks that)
    assert( sizeof(ChessPositionRaw) ==
               (offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL))
          );

    // We also rely on Moves being 32 bits for the implementation of Move
    //  bitwise == and != operators
    assert( sizeof(Move) == sizeof(int32_t) );

    // Clear move list
    l->count  = 0;   // set each field for each move

    // Loop through all squares
    for( square=a8; square<=h1; ++square )
    {

        // If square occupied by a piece of the right colour
        char piece=squares[square];
        if( (white&&IsWhite(piece)) || (!white&&IsBlack(piece)) )
        {

            // Generate moves according to the occupying piece
            switch( piece )
            {
                case 'P':
                {
                    WhitePawnMoves( l, square );
                    break;
                }
                case 'p':
                {
                    BlackPawnMoves( l, square );
                    break;
                }
                case 'N':
                case 'n':
                {
                    const lte *ptr = knight_lookup[square];
                    ShortMoves( l, square, ptr, NOT_SPECIAL );
                    break;
                }
                case 'B':
                case 'b':
                {
                    const lte *ptr = bishop_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'R':
                case 'r':
                {
                    const lte *ptr = rook_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'Q':
                case 'q':
                {
                    const lte *ptr = queen_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'K':
                case 'k':
                {
                    KingMoves( l, square );
                    break;
                }
            }
        }
    }
}

/****************************************************************************
 * Generate moves for pieces that move along multi-move rays (B,R,Q)
 ****************************************************************************/
void ChessRules::LongMoves( MOVELIST *l, Square square, const lte *ptr )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_rays = *ptr++;
    while( nbr_rays-- )
    {
        lte ray_len = *ptr++;
        while( ray_len-- )
        {
            dst = (Square)*ptr++;
            char piece=squares[dst];

            // If square not occupied (empty), add move to list
            if( IsEmptySquare(piece) )
            {
                m->src     = square;
                m->dst     = dst;
                m->capture = ' ';
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }

            // Else must move to end of ray
            else
            {
                ptr += ray_len;
                ray_len = 0;

                // If not occupied by our man add a capture
                if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
                {
                    m->src     = square;
                    m->dst     = dst;
                    m->special = NOT_SPECIAL;
                    m->capture = piece;
                    l->count++;
                    m++;
                }
            }
        }
    }
}

/****************************************************************************
 * Generate moves for pieces that move along single move rays (N,K)
 ****************************************************************************/
void ChessRules::ShortMoves( MOVELIST *l, Square square,
                                         const lte *ptr, SPECIAL special  )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        dst = (Square)*ptr++;
        char piece = squares[dst];

        // If square not occupied (empty), add move to list
        if( IsEmptySquare(piece) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Else if occupied by enemy man, add move to list as a capture
        else if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = piece;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of king moves
 ****************************************************************************/
void ChessRules::KingMoves( MOVELIST *l, Square square )
{
    const lte *ptr = king_lookup[square];
    ShortMoves( l, square, ptr, SPECIAL_KING_MOVE );

    // Generate castling king moves
    Move *m;
    m = &l->moves[l->count];

    // White castling
    if( square == e1 )   // king on e1 ?
    {

        // King side castling
        if(
            squares[g1] == ' '   &&
            squares[f1] == ' '   &&
            squares[h1] == 'R'   &&
            (wking)            &&
            !AttackedSquare(e1,false) &&
            !AttackedSquare(f1,false) &&
            !AttackedSquare(g1,false)
          )
        {
            m->src     = e1;
            m->dst     = g1;
            m->special = SPECIAL_WK_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Queen side castling
        if(
            squares[b1] == ' '         &&
            squares[c1] == ' '         &&
            squares[d1] == ' '         &&
            squares[a1] == 'R'         &&
            (wqueen)                 &&
            !AttackedSquare(e1,false)  &&
            !AttackedSquare(d1,false)  &&
            !AttackedSquare(c1,false)
          )
        {
            m->src     = e1;
            m->dst     = c1;
            m->special = SPECIAL_WQ_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }
    }

    // Black castling
    if( square == e8 )   // king on e8 ?
    {

        // King side castling
        if(
            squares[g8] == ' '         &&
            squares[f8] == ' '         &&
            squares[h8] == 'r'         &&
            (bking)                  &&
            !AttackedSquare(e8,true) &&
            !AttackedSquare(f8,true) &&
            !AttackedSquare(g8,true)
          )
        {
            m->src     = e8;
            m->dst     = g8;
            m->special = SPECIAL_BK_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Queen side castling
        if(
            squares[b8] == ' '         &&
            squares[c8] == ' '         &&
            squares[d8] == ' '         &&
            squares[a8] == 'r'         &&
            (bqueen)                 &&
            !AttackedSquare(e8,true) &&
            !AttackedSquare(d8,true) &&
            !AttackedSquare(c8,true)
          )
        {
            m->src     = e8;
            m->dst     = c8;
            m->special = SPECIAL_BQ_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of white pawn moves
 ****************************************************************************/
void ChessRules::WhitePawnMoves( MOVELIST *l,  Square square )
{
    Move *m = &l->moves[l->count];
    const lte *ptr = pawn_white_lookup[square];
    bool promotion = (RANK(square) == '7');

    // Capture ray
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        Square dst = (Square)*ptr++;
        if( dst == enpassant_target )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = SPECIAL_WEN_PASSANT;
            m->capture = 'p';
            m++;
            l->count++;
        }
        else if( IsBlack(squares[dst]) )
        {
            m->src    = square;
            m->dst    = dst;
            m->capture = squares[dst];
            if( !promotion )
            {
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }
            else
            {

                // Generate (under)promotions in the order (Q),N,B,R
                //  but we no longer rely on this elsewhere as it
                //  stops us reordering moves
                m->special   = SPECIAL_PROMOTION_QUEEN;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_KNIGHT;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_BISHOP;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_ROOK;
                m++;
                l->count++;
            }
        }
    }

    // Advance ray
    nbr_moves = *ptr++;
    for( lte i=0; i<nbr_moves; i++ )
    {
        Square dst = (Square)*ptr++;

        // If square occupied, end now
        if( !IsEmptySquare(squares[dst]) )
            break;
        m->src     = square;
        m->dst     = dst;
        m->capture = ' ';
        if( !promotion )
        {
            m->special  =  (i==0 ? NOT_SPECIAL : SPECIAL_WPAWN_2SQUARES);
            m++;
            l->count++;
        }
        else
        {

            // Generate (under)promotions in the order (Q),N,B,R
            //  but we no longer rely on this elsewhere as it
            //  stops us reordering moves
            m->special   = SPECIAL_PROMOTION_QUEEN;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_KNIGHT;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_BISHOP;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_ROOK;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of black pawn moves
 ****************************************************************************/
void ChessRules::BlackPawnMoves( MOVELIST *l, Square square )
{
    Move *m = &l->moves[l->count];
    const lte *ptr = pawn_black_lookup[square];
    bool promotion = (RANK(square) == '2');

    // Capture ray
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        Square dst = (Square)*ptr++;
        if( dst == enpassant_target )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = SPECIAL_BEN_PASSANT;
            m->capture = 'P';
            m++;
            l->count++;
        }
        else if( IsWhite(squares[dst]) )
        {
            m->src  = square;
            m->dst    = dst;
            m->capture = squares[dst];
            if( !promotion )
            {
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }
            else
            {

                // Generate (under)promotions in the order (Q),N,B,R
                //  but we no longer rely on this elsewhere as it
                //  stops us reordering moves
                m->special   = SPECIAL_PROMOTION_QUEEN;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_KNIGHT;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_BISHOP;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_ROOK;
                m++;
                l->count++;
            }
        }
    }

    // Advance ray
    nbr_moves = *ptr++;
    for( int i=0; i<nbr_moves; i++ )
    {
        Square dst = (Square)*ptr++;

        // If square occupied, end now
        if( !IsEmptySquare(squares[dst]) )
            break;
        m->src  = square;
        m->dst  = dst;
        m->capture = ' ';
        if( !promotion )
        {
            m->special  =  (i==0 ? NOT_SPECIAL : SPECIAL_BPAWN_2SQUARES);
            m++;
            l->count++;
        }
        else
        {

            // Generate (under)promotions in the order (Q),N,B,R
            //  but we no longer rely on this elsewhere as it
            //  stops us reordering moves
            m->special   = SPECIAL_PROMOTION_QUEEN;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_KNIGHT;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_BISHOP;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_ROOK;
            m++;
            l->count++;
        }
    }
}



/****************************************************************************
 * Create a list of all legal moves in this position, with extra info
 ****************************************************************************/
void ChessRules::GenLegalMoveList( MOVELIST *list, bool check[MAXMOVES],
                                                    bool mate[MAXMOVES],
                                                    bool stalemate[MAXMOVES] )
{
    int i, j;
    bool okay;
    TERMINAL terminal_score;
    MOVELIST list2;

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate(terminal_score);
        Square king_to_move = (Square)(white ? wking_square : bking_square );
        bool bcheck = false;
        if( AttackedPiece(king_to_move) )
            bcheck = true;
        PopMove( list2.moves[i] );
        if( okay )
        {
            stalemate[j] = (terminal_score==TERMINAL_WSTALEMATE ||
                            terminal_score==TERMINAL_BSTALEMATE);
            mate[j]      = (terminal_score==TERMINAL_WCHECKMATE ||
                            terminal_score==TERMINAL_BCHECKMATE);
            check[j]     = mate[j] ? false : bcheck;
            list->moves[j++] = list2.moves[i];
        }
    }
    list->count  = j;
}

/****************************************************************************
 * Create a list of all legal moves in this position, with extra info
 ****************************************************************************/
void ChessRules::GenLegalMoveList( MOVELIST *list, bool check[MAXMOVES],
                                                    bool mate[MAXMOVES],
                                                    bool stalemate[MAXMOVES] )
{


/****************************************************************************
 * Generate moves for pieces that move along single move rays (N,K)
 ****************************************************************************/
void ChessRules::ShortMoves( MOVELIST *l, Square square,
                                         const lte *ptr, SPECIAL special  )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        dst = (Square)*ptr++;
        char piece = squares[dst];

        // If square not occupied (empty), add move to list
        if( IsEmptySquare(piece) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Else if occupied by enemy man, add move to list as a capture
        else if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = piece;
            m++;
            l->count++;
        }
    }
}



/*

efficiently calculate list of all legal moves
 king moves
  for each knight ray
   if its an enemy knight increment check count
  for each king ray
   clear possible pin flag
   for each square
    if enemy
     it may be giving check, or it may be pinning, or neither, then we are done
      checker or pinner based on whether friend previously set possible pin flag
      horizontal and vertical rays - checkers and pinners are from rooks and queens
      diagonal rays - checkers and pinners are from bishops and queens, sometimes pawns check
      if checker increment check count
      if pinner save pinned piece and two pin rays
       a pinned piece can move along the two pin rays (one towards king the other towards pinner) only
       note that because there is only one king per side, a piece cannot be pinned in two or more directions at once
       (Euclid says only one line intersects two points - comprising pinned piece and king).
       diagonnally pinned bishops, queens, and pawns can move on the two diagonal pinning rays
       orthogonally pinned rooks, queens and pawns can move on the two orthogonal pinning rays
    if friend
     if possible pin flag already set we are done, else set possible pinned flag
    if empty and first square
     if the square is not attacked save this as a valid move

 if check > 1
  we are done because if double check only king moves are possible

 if check == 0
  add castling moves if possible

 normal moves
  if enpassant target, generate up to two enpassant moves
   ignore normal pinning logic, apply simple legality check - is king attacked after move?
  for each square on board
   if friend and not king
    for each ray or if pinned for each pin ray
     for each square
      if friend we are done
      if empty or enemy conditionally save move, then if enemy we are done
       conditionally save move
        if check save move only if destination is on ray from king to checker
        otherwise just save move



*/
