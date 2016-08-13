#include <stdio.h>

// Define for new numbering system without holes; a8=0 -> h1=63
#define T2

void queen()
{
    printf( "// Queen, up to 8 rays\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte queen_lookup_%c%c[] =\n", file, rank );
            printf( "{\n" );
            char files[8][8];
            char ranks[8][8];
            char lens [8];
            int idx=0, nbr_non_zero_rays=0;
            for( int ray=0; ray<8; ray++ )
            {
                idx = 0;
                switch( ray )
                {
                    case 0:
                    {
                        char temp=file;
                        for(;;)
                        {
                            temp--;
                            if( temp < 'a' )
                                break;
                            files[ray][idx] = temp;
                            ranks[ray][idx] = rank;
                            idx++;
                        }    
                        break;
                    }
                    case 1:
                    {
                        char temp=file;
                        for(;;)
                        {
                            temp++;
                            if( temp > 'h' )
                                break;
                            files[ray][idx] = temp;
                            ranks[ray][idx] = rank;
                            idx++;
                        }    
                        break;
                    }
                    case 2:
                    {
                        char temp=rank;
                        for(;;)
                        {
                            temp--;
                            if( temp < '1' )
                                break;
                            files[ray][idx] = file;
                            ranks[ray][idx] = temp;
                            idx++;
                        }    
                        break;
                    }
                    case 3:
                    {
                        char temp=rank;
                        for(;;)
                        {
                            temp++;
                            if( temp > '8' )
                                break;
                            files[ray][idx] = file;
                            ranks[ray][idx] = temp;
                            idx++;
                        }    
                        break;
                    }
                    case 4:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf--;
                            tempr--;
                            if( tempf<'a' || tempr<'1' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 5:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf--;
                            tempr++;
                            if( tempf<'a' || tempr>'8' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 6:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf++;
                            tempr++;
                            if( tempf>'h' || tempr>'8' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 7:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf++;
                            tempr--;
                            if( tempf>'h' || tempr<'1' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                }
                lens[ray] = idx;
                if( idx > 0 )
                    nbr_non_zero_rays++;
            }
            printf( "(lte)%d\n", nbr_non_zero_rays );
            for( int ray=0; ray<8; ray++ )
            {
                if( lens[ray] )
                {
                    printf( "    ,(lte)%d ", lens[ray] );
                    for( int i=0; i<lens[ray]; i++ )
                        printf( ",(lte)%c%c ", files[ray][i], ranks[ray][i] );
                    printf( "\n" );
                }
            }
            printf( "};\n" );
        }
    }
}


void rook()
{
    printf( "\n// Rook, up to 4 rays\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte rook_lookup_%c%c[] =\n", file, rank );
            printf( "{\n" );
            char files[4][8];
            char ranks[4][8];
            char lens [4];
            int idx=0, nbr_non_zero_rays=0;
            for( int ray=0; ray<4; ray++ )
            {
                idx = 0;
                switch( ray )
                {
                    case 0:
                    {
                        char temp=file;
                        for(;;)
                        {
                            temp--;
                            if( temp < 'a' )
                                break;
                            files[ray][idx] = temp;
                            ranks[ray][idx] = rank;
                            idx++;
                        }    
                        break;
                    }
                    case 1:
                    {
                        char temp=file;
                        for(;;)
                        {
                            temp++;
                            if( temp > 'h' )
                                break;
                            files[ray][idx] = temp;
                            ranks[ray][idx] = rank;
                            idx++;
                        }    
                        break;
                    }
                    case 2:
                    {
                        char temp=rank;
                        for(;;)
                        {
                            temp--;
                            if( temp < '1' )
                                break;
                            files[ray][idx] = file;
                            ranks[ray][idx] = temp;
                            idx++;
                        }    
                        break;
                    }
                    case 3:
                    {
                        char temp=rank;
                        for(;;)
                        {
                            temp++;
                            if( temp > '8' )
                                break;
                            files[ray][idx] = file;
                            ranks[ray][idx] = temp;
                            idx++;
                        }    
                        break;
                    }
                }
                lens[ray] = idx;
                if( idx > 0 )
                    nbr_non_zero_rays++;
            }
            printf( "(lte)%d\n", nbr_non_zero_rays );
            for( int ray=0; ray<4; ray++ )
            {
                if( lens[ray] )
                {
                    printf( "    ,(lte)%d ", lens[ray] );
                    for( int i=0; i<lens[ray]; i++ )
                        printf( ",(lte)%c%c ", files[ray][i], ranks[ray][i] );
                    printf( "\n" );
                }
            }
            printf( "};\n" );
        }
    }
}

void bishop()
{
    printf( "\n// Bishop, up to 4 rays\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte bishop_lookup_%c%c[] =\n", file, rank );
            printf( "{\n" );
            char files[4][8];
            char ranks[4][8];
            char lens [4];
            int idx=0, nbr_non_zero_rays=0;
            for( int ray=0; ray<4; ray++ )
            {
                idx = 0;
                switch( ray )
                {
                    case 0:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf--;
                            tempr--;
                            if( tempf<'a' || tempr<'1' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 1:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf--;
                            tempr++;
                            if( tempf<'a' || tempr>'8' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 2:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf++;
                            tempr++;
                            if( tempf>'h' || tempr>'8' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 3:
                    {
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf++;
                            tempr--;
                            if( tempf>'h' || tempr<'1' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                }
                lens[ray] = idx;
                if( idx > 0 )
                    nbr_non_zero_rays++;
            }
            printf( "(lte)%d\n", nbr_non_zero_rays );
            for( int ray=0; ray<4; ray++ )
            {
                if( lens[ray] )
                {
                    printf( "    ,(lte)%d ", lens[ray] );
                    for( int i=0; i<lens[ray]; i++ )
                        printf( ",(lte)%c%c ", files[ray][i], ranks[ray][i] );
                    printf( "\n" );
                }
            }
            printf( "};\n" );
        }
    }
}

void pawn_white()
{
    char files[2];
    char ranks[2];
    char tempf;
    char tempr;
    int idx;
    printf( "\n// White pawn, capture ray followed by advance ray\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte pawn_white_lookup_%c%c[] =\n{\n", file, rank );

            // Captures
            tempf=file;
            tempr=rank;
            idx=0;
            tempf--;
            tempr++;
            if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
            {
                files[idx] = tempf;
                ranks[idx] = tempr;
                idx++;
            }
            tempf+=2;
            if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
            {
                files[idx] = tempf;
                ranks[idx] = tempr;
                idx++;
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( ",\n" );

            // Advances
            idx = 0;
            tempf=file;
            tempr=rank;
            tempr++;
            if( tempr<='8' )
            {
                files[idx] = tempf;
                ranks[idx] = tempr;
                idx++;
                if( rank == '2' )
                {
                    tempr++;
                    files[idx] = tempf;
                    ranks[idx] = tempr;
                    idx++;
                }
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( "\n" );
            printf( "};\n" );
        }
    }
}

void pawn_black()
{
    char files[2];
    char ranks[2];
    char tempf;
    char tempr;
    int idx;
    printf( "\n// Black pawn, capture ray followed by advance ray\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte pawn_black_lookup_%c%c[] =\n{\n", file, rank );

            // Captures
            tempf=file;
            tempr=rank;
            idx=0;
            tempf--;
            tempr--;
            if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
            {
                files[idx] = tempf;
                ranks[idx] = tempr;
                idx++;
            }
            tempf+=2;
            if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
            {
                files[idx] = tempf;
                ranks[idx] = tempr;
                idx++;
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( ",\n" );

            // Advances
            idx = 0;
            tempf=file;
            tempr=rank;
            tempr--;
            if( tempr>='1' )
            {
                files[idx] = tempf;
                ranks[idx] = tempr;
                idx++;
                if( rank == '7' )
                {
                    tempr--;
                    files[idx] = tempf;
                    ranks[idx] = tempr;
                    idx++;
                }
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( "\n" );
            printf( "};\n" );
        }
    }
}

void knight()
{
    printf( "\n// Knight, up to 8 squares\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte knight_lookup_%c%c[] =\n", file, rank );
            printf( "{\n" );
            char files[8];
            char ranks[8];
            int idx=0;

            for( int i=0; i<8; i++ )
            {
                char tempf=file;
                char tempr=rank;
                switch(i)
                {
                    case 0: tempf-=2; tempr-=1; break;
                    case 1: tempf-=2; tempr+=1; break;
                    case 2: tempf-=1; tempr-=2; break;
                    case 3: tempf-=1; tempr+=2; break;
                    case 4: tempf+=2; tempr-=1; break;
                    case 5: tempf+=2; tempr+=1; break;
                    case 6: tempf+=1; tempr-=2; break;
                    case 7: tempf+=1; tempr+=2; break;
                }
                if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
                {
                    files[idx] = tempf;
                    ranks[idx] = tempr;
                    idx++;
                }
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( "\n" );
            printf( "};\n" );
        }
    }
}

void king()
{
    printf( "\n// King, up to 8 squares\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte king_lookup_%c%c[] =\n", file, rank );
            printf( "{\n" );
            char files[8];
            char ranks[8];
            int idx=0;

            for( int i=0; i<8; i++ )
            {
                char tempf=file;
                char tempr=rank;
                switch(i)
                {
                    case 0: tempf-=1; tempr-=1; break;
                    case 1: tempf-=1; tempr+=1; break;
                    case 2: tempf+=1; tempr-=1; break;
                    case 3: tempf+=1; tempr+=1; break;
                    case 4: tempf-=1;           break;
                    case 5: tempf+=1;           break;
                    case 6:           tempr-=1; break;
                    case 7:           tempr+=1; break;
                }
                if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
                {
                    files[idx] = tempf;
                    ranks[idx] = tempr;
                    idx++;
                }
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( "\n" );
            printf( "};\n" );
        }
    }
}

void attacks( bool white )
{
    char ray_type[8];
    printf( "\n// Attack from up to 8 rays on a %s piece\n", white?"white":"black" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte attacks_%s_lookup_%c%c[] =\n", white?"white":"black", file, rank );
            printf( "{\n" );
            char files[8][8];
            char ranks[8][8];
            char lens [8];
            int idx=0, nbr_non_zero_rays=0;
            for( int ray=0; ray<8; ray++ )
            {
                idx = 0;
                switch( ray )
                {
                    case 0:
                    {
                        ray_type[ray] = 'R';
                        char temp=file;
                        for(;;)
                        {
                            temp--;
                            if( temp < 'a' )
                                break;
                            files[ray][idx] = temp;
                            ranks[ray][idx] = rank;
                            idx++;
                        }    
                        break;
                    }
                    case 1:
                    {
                        ray_type[ray] = 'R';
                        char temp=file;
                        for(;;)
                        {
                            temp++;
                            if( temp > 'h' )
                                break;
                            files[ray][idx] = temp;
                            ranks[ray][idx] = rank;
                            idx++;
                        }    
                        break;
                    }
                    case 2:
                    {
                        ray_type[ray] = 'R';
                        char temp=rank;
                        for(;;)
                        {
                            temp--;
                            if( temp < '1' )
                                break;
                            files[ray][idx] = file;
                            ranks[ray][idx] = temp;
                            idx++;
                        }    
                        break;
                    }
                    case 3:
                    {
                        ray_type[ray] = 'R';
                        char temp=rank;
                        for(;;)
                        {
                            temp++;
                            if( temp > '8' )
                                break;
                            files[ray][idx] = file;
                            ranks[ray][idx] = temp;
                            idx++;
                        }    
                        break;
                    }
                    case 4:
                    {
                        ray_type[ray] = white?'B':'P';
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf--;
                            tempr--;
                            if( tempf<'a' || tempr<'1' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 5:
                    {
                        ray_type[ray] = white?'P':'B';
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf--;
                            tempr++;
                            if( tempf<'a' || tempr>'8' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 6:
                    {
                        ray_type[ray] = white?'P':'B';
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf++;
                            tempr++;
                            if( tempf>'h' || tempr>'8' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                    case 7:
                    {
                        ray_type[ray] = white?'B':'P';
                        char tempf=file;
                        char tempr=rank;
                        for(;;)
                        {
                            tempf++;
                            tempr--;
                            if( tempf>'h' || tempr<'1' )
                                break;
                            files[ray][idx] = tempf;
                            ranks[ray][idx] = tempr;
                            idx++;
                        }    
                        break;
                    }
                }
                lens[ray] = idx;
                if( idx > 0 )
                    nbr_non_zero_rays++;
            }
            printf( "(lte)%d\n", nbr_non_zero_rays );
            for( int ray=0; ray<8; ray++ )
            {
                if( lens[ray] )
                {
                    printf( "    ,(lte)%d ", lens[ray] );
                    for( int i=0; i<lens[ray]; i++ )
                    {
                        printf( ",(lte)%c%c,(lte)(", files[ray][i], ranks[ray][i] );
                        if( i == 0 )
                            printf("K|");
                        if( i==0 && ray_type[ray]=='P' )
                            printf("P|B|Q) ");
                        else if( ray_type[ray] == 'R' )
                        {
                            printf("R|Q) ");
                            if( i==0 )
                                printf("  ");
                        }
                        else  // ('P' && i>0) || 'B')
                        {
                            printf("B|Q) ");
                            if( i==0 )
                                printf("  ");
                        }
                    }
                    printf( "\n" );
                }
            }
            printf( "};\n" );
        }
    }
}

void pawn_attacks( bool white )
{
    printf( "\n// Attack from up to 2 %s pawns on a %s piece\n", white?"black":"white", white?"white":"black" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte pawn_attacks_%s_lookup_%c%c[] =\n", white?"white":"black", file, rank );
            printf( "{\n" );
            char files[2];
            char ranks[2];
            int idx=0;
            char tempf=file;
            char tempr=rank;
            tempf--;
            if( white )
                tempr++;
            else
                tempr--;
            for( int i=0; i<2; i++ )
            {
                if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
                {
                    files[idx] = tempf;
                    ranks[idx] = tempr;
                    idx++;
                }    
                tempf += 2;
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( "\n};\n" );
        }
    }
}

void good_king_position()
{
    // Sort of an extended knight move
    /*   (uc)  0x21,     // NNE
         (uc)  0x20,     // NN 
         (uc)  0x12,     // NEE
         (uc)  0x02,     //  EE
         (uc) -0x0e,     // SEE
         (uc) -0x1f,     // SSE
         (uc) -0x21,     // SSW
         (uc) -0x20,     // SS 
         (uc) -0x12,     // SWW
         (uc) -0x02,     //  WW
         (uc)  0x0e,     // NWW
         (uc)  0x1f      // NNW */

    printf( "\n// Good king positions in the ending are often a knight move (2-1) or" );
    printf( "\n//  a (2-0) move towards the centre\n" );
    for( char file='a'; file<='h'; file++ )
    {
        for( char rank='1'; rank<='8'; rank++ )
        {
            printf( "static const lte good_king_position_lookup_%c%c[] =\n", file, rank );
            printf( "{\n" );
            char files[12];
            char ranks[12];
            int idx=0;

            for( int i=0; i<12; i++ )
            {
                char tempf=file;
                char tempr=rank;
                switch(i)
                {
                    case 0:  tempf-=2; tempr-=1; break;
                    case 1:  tempf-=2;           break;
                    case 2:  tempf-=2; tempr+=1; break;
                    case 3:  tempf+=2; tempr-=1; break;
                    case 4:  tempf+=2;           break;
                    case 5:  tempf+=2; tempr+=1; break;
                    case 6:  tempr-=2; tempf-=1; break;
                    case 7:  tempr-=2;           break;
                    case 8:  tempr-=2; tempf+=1; break;
                    case 9:  tempr+=2; tempf-=1; break;
                    case 10: tempr+=2;           break;
                    case 11: tempr+=2; tempf+=1; break;
                }
                if( 'a'<=tempf && tempf<='h' && '1'<=tempr && tempr<='8' )
                {
                    files[idx] = tempf;
                    ranks[idx] = tempr;
                    idx++;
                }
            }
            printf( "    (lte)%d", idx );
            for( int i=0; i<idx; i++ )
                printf( ", (lte)%c%c", files[i], ranks[i] );
            printf( "\n" );
            printf( "};\n" );
        }
    }
}

void lookup( const char *txt )
{
    printf( "\n// %s\n", txt );
    printf( "static const lte *%s[] =\n", txt );
    printf("{\n");
    #ifdef T2
    for( char rank='8'; rank>='1'; rank-- )
    #else
    for( char rank='1'; rank<='8'; rank++ )
    #endif
    {
        for( char file='a'; file<='h'; file++ )
        {
            printf( "    %s_%c%c", txt, file, rank );
            #ifdef T2
            if( file=='h' && rank=='1' )
            #else
            if( file=='h' && rank=='8' )
            #endif
                printf("\n");
            else
                printf(",\n");
        }
        #ifndef T2
        if( rank != '8' )
            printf( "    0,0,0,0,0,0,0,0,\n" );
        #endif
    }
    printf("};\n");
}

int main()
{
    printf("//\n");
    printf("// These lookup tables are machine generated\n");
    printf("// They require prior definitions of;\n");
    printf("//  squares (a1,a2..h8)\n");
    printf("//  pieces (P,N,B,N,R,Q,K)\n");
    printf("//  lte (=lookup table element, a type for the lookup tables, eg int or unsigned char)\n" );
    printf("//\n");
    printf("\n");
    queen();
    lookup( "queen_lookup" );
    rook();
    lookup( "rook_lookup" );
    bishop();
    lookup( "bishop_lookup" );
    knight();
    lookup( "knight_lookup" );
    king();
    lookup( "king_lookup" );
    pawn_white();
    lookup( "pawn_white_lookup" );
    pawn_black();
    lookup( "pawn_black_lookup" );
    good_king_position();
    lookup( "good_king_position_lookup" );
    pawn_attacks(true);
    lookup( "pawn_attacks_white_lookup" );
    pawn_attacks(false);
    lookup( "pawn_attacks_black_lookup" );
    attacks(true);
    lookup( "attacks_white_lookup" );
    attacks(false);
    lookup( "attacks_black_lookup" );
    return 0;
}
