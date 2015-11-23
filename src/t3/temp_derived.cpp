struct TempElement
{
    int idx;
    int transpo;
    std::string blob;
    std::vector<int> counts;
};

static bool compare_blob( const TempElement &e1, const TempElement &e2 )
{
    bool lt = false;
    if( e1.transpo == e2.transpo )
        lt = (e1.blob < e2.blob);
    else
        lt = (e1.transpo > e2.transpo);     // smaller transpo nbr should come first
    return lt;
}

static bool compare_counts( const TempElement &e1, const TempElement &e2 )
{
    bool lt = false;
    if( e1.transpo != e2.transpo )
        lt = (e1.transpo > e2.transpo);     // smaller transpo nbr should come first
    else
    {
        unsigned int len = e1.blob.length();
        if( e2.blob.length() < len )
            len = e2.blob.length();
        for( unsigned int i=0; i<len; i++ )
        {
            if( e1.counts[i] != e2.counts[i] )
            {
                lt = (e1.counts[i] > e2.counts[i]);
                return lt;
            }
        }
        lt = (e1.blob.length() > e2.blob.length());
    }
    return lt;
}

void DbDialog::SmartCompare()
{
    std::vector<TempElement> inter;     // intermediate representation
    
    // Step 1, do a conventional string sort, beginning at our offset into the blob
    unsigned int sz = displayed_games.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        std::string str_blob = displayed_games[i]->RefCompressedMoves();
        TempElement e;
        unsigned int sz2 = transpositions.size();
        for( unsigned int j=0; j<sz2; j++ )
        {
            PATH_TO_POSITION *ptp = &transpositions[j];
            unsigned int offset = ptp->blob.length();
            if( 0 == memcmp( ptp->blob.c_str(), str_blob.c_str(), offset ) )
            {
                e.idx = i;
                e.blob = str_blob.substr(offset);
                e.counts.resize( e.blob.length() );
                e.transpo = transpo_activated ? j+1 : 0;
                inter.push_back(e);
                break;
            }
        }
    }
    std::sort( inter.begin(), inter.end(), compare_blob );
    
    
    // Step 2, work out the nbr of moves in clumps of moves
/*
     // Imagine that the compressed one byte codes sort in the same order as multi-char ascii move
     //  representations (they don't but the key thing is that they are deterministic - the actual order
     //  doesn't matter)
     A)  1.d4 Nf6 2.c4 e6 3.Nc3
     B)  1.d4 Nf6 2.c4 e6 3.Nf3
     C)  1.d4 Nf6 2.c4 e6 3.Nf3
     D)  1.d4 d5 2.c4 e6 3.Nc3
     E)  1.d4 d5 2.c4 e6 3.Nf3
     F)  1.d4 d5 2.c4 e6 3.Nf3
     G)  1.d4 d5 2.c4 c5 3.d5

     // Calculate the counts like this
              j=0 j=1 j=2 j=3 j=4
     i=0  A)  7
     i=1  B)  7
     i=2  C)  7
     i=3  D)  7      j=0 count 7 '1.d4's at offset 0 into each game 
     i=4  E)  7
     i=5  F)  7
     i=6  G)  7

              j=0 j=1 j=2 j=3 j=4
     i=0  A)  7   3
     i=1  B)  7   3
     i=2  C)  7   3
     i=3  D)  7   4    j=1 count 3 '1...Nf6's and 4 '1...d5's 
     i=4  E)  7   4
     i=5  F)  7   4
     i=6  G)  7   4

              j=0 j=1 j=2 j=3 j=4
     i=0  A)  7   3   7   6   1
     i=1  B)  7   3   7   6   2
     i=2  C)  7   3   7   6   2     etc.
     i=3  D)  7   4   7   6   1
     i=4  E)  7   4   7   6   2
     i=5  F)  7   4   7   6   2
     i=6  G)  7   4   7   1   1

     //  Now re-sort based on these counts, bigger counts come first
     E)  7   4   7   6   2
     F)  7   4   7   6   2
     D)  7   4   7   6   1
     G)  7   4   7   1   1
     B)  7   3   7   6   2
     C)  7   3   7   6   2 
     A)  7   3   7   6   1

     // So final ordering is according to line popularity
     E)  1.d4 d5 2.c4 e6 3.Nf3
     F)  1.d4 d5 2.c4 e6 3.Nf3
     D)  1.d4 d5 2.c4 e6 3.Nc3
     G)  1.d4 d5 2.c4 c5 3.d5
     B)  1.d4 Nf6 2.c4 e6 3.Nf3
     C)  1.d4 Nf6 2.c4 e6 3.Nf3
     A)  1.d4 Nf6 2.c4 e6 3.Nc3
*/


    sz = inter.size();
    bool at_least_one = true;
    for( unsigned int j=0; at_least_one; j++ )
    {
        at_least_one = false;  // stop when we've passed end of all strings
        char current='\0';
        unsigned int start=0;
        bool run_in_progress=false;
        for( unsigned int i=0; i<sz; i++ )
        {
            TempElement &e = inter[i];
            
            // A short game stops runs
            if( j >= e.blob.length() )
            {
                if( run_in_progress )
                {
                    run_in_progress = false;
                    int count = i-start;
                    for( int k=start; k<i; k++ )
                    {
                        TempElement &f = inter[k];
                        f.counts[j] = count;
                    }
                }
                continue;
            }
            at_least_one = true;
            char c = e.blob[j];
            
            // First time, get something to start a run
            if( !run_in_progress )
            {
                run_in_progress = true;
                current = c;
                start = i;
            }
            else
            {
                
                // Run can be over because of character change
                if( c != current )
                {
                    int count = i-start;
                    for( int k=start; k<i; k++ )
                    {
                        TempElement &f = inter[k];
                        f.counts[j] = count;
                    }
                    current = c;
                    start = i;
                }
                
                // And/Or because we reach bottom
                if( i+1 == sz )
                {
                    int count = sz - start;
                    for( int k=start; k<sz; k++ )
                    {
                        TempElement &f = inter[k];
                        f.counts[j] = count;
                    }
                }
            }
        }
    }

    // Step 3 sort again using the counts
    std::sort( inter.begin(), inter.end(), compare_counts );
    
    // Step 4 build sorted version of games list
    std::vector< smart_ptr<ListableGame> > temp;
    sz = inter.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        TempElement &e = inter[i];
        temp.push_back( displayed_games[e.idx] );
    }

    // Step 5 replace original games list
    displayed_games = temp;
}

void DbDialog::GdvListColClick( int compare_col )
{
    bool in_memory = (displayed_games.size() > 0);
    if( !in_memory )
    {
        if( nbr_games_in_list_ctrl >= QUERY_LOAD_INTO_MEMORY_THRESHOLD )
        {
            wxString msg( "This column sort requires loading a large number of games into memory, is that okay?" );
            int answer = wxMessageBox( msg, "Press Yes to load games",  wxYES_NO|wxCANCEL, this );
            bool ok = (answer == wxYES);
            if( !ok )
                return;
        }
        LoadGamesIntoMemory();
        StatsCalculate();
    }
    ColumnSort( compare_col, displayed_games );  
#if 0
    static int last_time;
    static int consecutive;
    if( compare_col == last_time )
        consecutive++;
    else
        consecutive=0;
    this->compare_col = compare_col;
    backdoor = this;
    if( compare_col == 10 )
        SmartCompare();
    else
        std::sort( displayed_games.begin(), displayed_games.end(), (consecutive%2==0)?rev_compare:compare );
    nbr_games_in_list_ctrl = displayed_games.size();
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
    int top = list_ctrl->GetTopItem();
    int count = 1 + list_ctrl->GetCountPerPage();
    if( count > nbr_games_in_list_ctrl )
        count = nbr_games_in_list_ctrl;
    for( int i=0; i<count; i++ )
        list_ctrl->RefreshItem(top++);
    Goto(0);
    last_time = compare_col;
#endif
}
