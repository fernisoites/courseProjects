#include "replacement_state.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This file is distributed as part of the Cache Replacement Championship     //
// workshop held in conjunction with ISCA'2010.                               //
//                                                                            //
//                                                                            //
// Everyone is granted permission to copy, modify, and/or re-distribute       //
// this software.                                                             //
//                                                                            //
// Please contact Aamer Jaleel <ajaleel@gmail.com> should you have any        //
// questions                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/*
** This file implements the cache replacement state. Users can enhance the code
** below to develop their cache replacement ideas.
**
*/


////////////////////////////////////////////////////////////////////////////////
// The replacement state constructor:                                         //
// Inputs: number of sets, associativity, and replacement policy to use       //
// Outputs: None                                                              //
//                                                                            //
// DO NOT CHANGE THE CONSTRUCTOR PROTOTYPE                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
CACHE_REPLACEMENT_STATE::CACHE_REPLACEMENT_STATE( UINT32 _sets, UINT32 _assoc, UINT32 _pol )
{

    numsets    = _sets;
    assoc      = _assoc;
    replPolicy = _pol;

    mytimer    = 0;

    InitReplacementState();
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function initializes the replacement policy hardware by creating      //
// storage for the replacement state on a per-line/per-cache basis.           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::InitReplacementState()
{
    // Create the state for sets, then create the state for the ways
    repl  = new LINE_REPLACEMENT_STATE* [ numsets ];

    // ensure that we were able to create replacement state
    assert(repl);

    // Create the state for the sets
    for(UINT32 setIndex=0; setIndex<numsets; setIndex++) 
    {
        repl[ setIndex ]  = new LINE_REPLACEMENT_STATE[ assoc ];

        for(UINT32 way=0; way<assoc; way++) 
        {
            // initialize stack position (for true LRU)
            repl[ setIndex ][ way ].LRUstackposition = way;
        }
    }

    // Contestants:  ADD INITIALIZATION FOR YOUR HARDWARE HERE

/* mod_begin 1103 - Initialization for replacement hardware */
    sysarchInit();
/* mod_end 1103 */

}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function is called by the cache on every cache miss. The input        //
// arguments are the thread id, set index, pointers to ways in current set    //
// and the associativity.  We are also providing the PC, physical address,    //
// and accesstype should you wish to use them at victim selection time.       //
// The return value is the physical way index for the line being replaced.    //
// Return -1 if you wish to bypass LLC.                                       //
//                                                                            //
// vicSet is the current set. You can access the contents of the set by       //
// indexing using the wayID which ranges from 0 to assoc-1 e.g. vicSet[0]     //
// is the first way and vicSet[4] is the 4th physical way of the cache.       //
// Elements of LINE_STATE are defined in crc_cache_defs.h                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::GetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc,
                                               Addr_t PC, Addr_t paddr, UINT32 accessType )
{
    // If no invalid lines, then replace based on replacement policy
    if( replPolicy == CRC_REPL_LRU ) 
    {
        return Get_LRU_Victim( setIndex );
    }
    else if( replPolicy == CRC_REPL_RANDOM )
    {
        return Get_Random_Victim( setIndex );
    }
    else if( replPolicy == CRC_REPL_CONTESTANT )
    {
        // Contestants:  ADD YOUR VICTIM SELECTION FUNCTION HERE
/* mod_begin 1104 - Select victim */
        return sysarchSelectVictim(tid, setIndex, vicSet, assoc, PC, paddr, accessType);
/* mod_end 1104 */
        
    }

    // We should never get here
    assert(0);

    return -1; // Returning -1 bypasses the LLC
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function is called by the cache after every cache hit/miss            //
// The arguments are: the set index, the physical way of the cache,           //
// the pointer to the physical line (should contestants need access           //
// to information of the line filled or hit upon), the thread id              //
// of the request, the PC of the request, the accesstype, and finall          //
// whether the line was a cachehit or not (cacheHit=true implies hit)         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::UpdateReplacementState( 
    UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
    UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit )
{
    // What replacement policy?
    if( replPolicy == CRC_REPL_LRU ) 
    {
        UpdateLRU( setIndex, updateWayID );
    }
    else if( replPolicy == CRC_REPL_RANDOM )
    {
        // Random replacement requires no replacement state update
    }
    else if( replPolicy == CRC_REPL_CONTESTANT )
    {
        // Contestants:  ADD YOUR UPDATE REPLACEMENT STATE FUNCTION HERE
        // Feel free to use any of the input parameters to make
        // updates to your replacement policy

/* mod_begin 1105 - Update replacement state */
        sysarchUpdateState(setIndex, updateWayID, currLine, tid, PC, accessType, cacheHit);
/* mod_end 1105 */
    }
    
    
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//////// HELPER FUNCTIONS FOR REPLACEMENT UPDATE AND VICTIM SELECTION //////////
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function finds the LRU victim in the cache set by returning the       //
// cache block at the bottom of the LRU stack. Top of LRU stack is '0'        //
// while bottom of LRU stack is 'assoc-1'                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::Get_LRU_Victim( UINT32 setIndex )
{
    // Get pointer to replacement state of current set
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];

    INT32   lruWay   = 0;

    // Search for victim whose stack position is assoc-1
    for(UINT32 way=0; way<assoc; way++) 
    {
        if( replSet[way].LRUstackposition == (assoc-1) ) 
        {
            lruWay = way;
            break;
        }
    }

    // return lru way
    return lruWay;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function finds a random victim in the cache set                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::Get_Random_Victim( UINT32 setIndex )
{
    INT32 way = (rand() % assoc);
    
    return way;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function implements the LRU update routine for the traditional        //
// LRU replacement policy. The arguments to the function are the physical     //
// way and set index.                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::UpdateLRU( UINT32 setIndex, INT32 updateWayID )
{
    // Determine current LRU stack position
    UINT32 currLRUstackposition = repl[ setIndex ][ updateWayID ].LRUstackposition;

    // Update the stack position of all lines before the current line
    // Update implies incremeting their stack positions by one
    for(UINT32 way=0; way<assoc; way++) 
    {
        if( repl[setIndex][way].LRUstackposition < currLRUstackposition ) 
        {
            repl[setIndex][way].LRUstackposition++;
        }
    }

    // Set the LRU stack position of new line to be zero
    repl[ setIndex ][ updateWayID ].LRUstackposition = 0;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// The function prints the statistics for the cache                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
ostream & CACHE_REPLACEMENT_STATE::PrintStats(ostream &out)
{

    out<<"=========================================================="<<endl;
    out<<"=========== Replacement Policy Statistics ================"<<endl;
    out<<"=========================================================="<<endl;

    // CONTESTANTS:  Insert your statistics printing here

/* mod_begin 1106 - Print stats */
    sysarchPrintStats(out);
/* mod_end 1106 */

    return out;
    
}

/* mod_begin 1107 - Functions */
void CACHE_REPLACEMENT_STATE::sysarchInit()
{
    // Initial scores
    initialScore = HALF_MAX_LINE_SCORE - INIT_SCORE_STEP;
    initialScoreUp = true;
    
    accessCounter = 0;
    prevMisses = 0;
    currMisses = 0;
    
    // Lines
    for(UINT32 setIndex=0; setIndex<numsets; setIndex++) {
        repl[ setIndex ]  = new LINE_REPLACEMENT_STATE[ assoc ];

        for(UINT32 way=0; way<assoc; way++) {
            repl[ setIndex ][ way ].initScore();
        }
    }
    
    // Statistics
    totalScores = 0;
    updateCounter = 0;
    
    totalScoreRanges = 0;
    totalNumLines = 0;
    totalMinScores = 0;
    totalMaxScores = 0;
    scoreRangeCounter = 0;

    noLinesUnderThreshold = 0;

    numLinesEvicted = 0;
    
    numIntervals = 0;

    totalInitScores = 0;
}

INT32 CACHE_REPLACEMENT_STATE::sysarchSelectVictim(
    UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc,
    Addr_t PC, Addr_t paddr, UINT32 accessType)
{
    INT32 way = 0;
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];


    //----------------------------------------------
    // Selection policy: Randomly select lines with scores under threshold
    //----------------------------------------------
    INT32 threshold_score = THRESHOLD_SCORE;
    
    UINT32 num_lines = 0;
    INT32 maxScore = 0;
    INT32 minScore = MAX_LINE_SCORE;
    bool found_lines = true;
    
    // Get the number of lines with scores under threshold
    INT32 minScoreAll = MAX_LINE_SCORE;
    for (UINT32 i = 0; i < assoc; i++) {
        INT32 score = replSet[i].getScore();
        if (score < threshold_score) {
            num_lines++;
            
            if (maxScore < score) maxScore = score;
            if (minScore > score) minScore = score;
        }
        
        if (minScoreAll > score) {
            way = i;
            minScoreAll = score;
        }
    }
    
    // Select the line if there is at least one line available
    if (num_lines > 0) {
        srand(time(NULL));
        UINT32 line_number = rand() % num_lines;
        UINT32 line_cnt = 0;
        
        for (UINT32 i = 0; i < assoc; i++) {
            if (replSet[i].getScore() < threshold_score) {
                if (line_cnt == line_number) {
                    way = i;
                    break;
                }
            
                line_cnt++;
            }
        }
    
    } else {
        
        found_lines = false;
    }

    //----------------------------------------------
    // Update stats
    //----------------------------------------------
    if (found_lines) {
        totalScoreRanges += maxScore - minScore;
        totalNumLines += num_lines;
        scoreRangeCounter++;
        
        totalMinScores += minScore;
        totalMaxScores += maxScore;
    
    } else {
        noLinesUnderThreshold++;
    }
    
    numLinesEvicted++;
    
    //----------------------------------------------
    // Return
    //----------------------------------------------
    return way;
}


void CACHE_REPLACEMENT_STATE::sysarchUpdateState( 
    UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
    UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit)
{
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];


    //----------------------------------------------
    // Update scores
    //----------------------------------------------
    for (UINT32 i = 0; i < assoc; i++) {
        if (i == (UINT32) updateWayID) {
            if (cacheHit) {
                replSet[i].incrScore();
            } else {
                replSet[i].setInitScore(initialScore);
            }
        
        } else {
            replSet[i].decrScore();
        }
    }
    

    //----------------------------------------------
    // Dynamically change initial score
    //----------------------------------------------
    accessCounter++;
    if (!cacheHit) currMisses++;
    
    if (accessCounter == ACCESS_INTERVAL) {
        initialScoreUp ^= (currMisses > prevMisses);
        
        initialScore += initialScoreUp ? INIT_SCORE_STEP : -INIT_SCORE_STEP;
        
        if (initialScore > MAX_LINE_SCORE) initialScore = MAX_LINE_SCORE;
        if (initialScore < 0) initialScore = 0;
    
        prevMisses = currMisses;
        currMisses = 0;

        accessCounter = 0;
        
        // Stats
        numIntervals++;
        totalInitScores += initialScore;
    }
    
    
    //----------------------------------------------
    // Update stats
    //----------------------------------------------
    for (UINT32 i = 0; i < assoc; i++) {
        totalScores += replSet[i].getScore();
    }
    
    updateCounter++;
}

ostream & CACHE_REPLACEMENT_STATE::sysarchPrintStats(ostream &out)
{
    out << "Cache replacement policy number: " << replPolicy << endl;
    
    out << "Total scores of a set: " << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*totalScores/updateCounter << endl;
        
    
    out << "Average min scores: " << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*totalMinScores/scoreRangeCounter << endl;
    
    out << "Average max scores: " << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*totalMaxScores/scoreRangeCounter << endl;
        
    out << "Average score range: " << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*totalScoreRanges/scoreRangeCounter << endl;
    

    out << "Average number of lines with scores under threshold: " 
        << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*totalNumLines/scoreRangeCounter << endl;
    

    out << "Times no lines under threshold: " << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*noLinesUnderThreshold/numLinesEvicted*100 << "%" << endl;
    

    out << "Average initial score: " << setiosflags(ios::fixed) << setprecision(2) 
        << 1.*totalInitScores/numIntervals << endl;
    
    out << endl;
    return out;
}

/* mod_end 1107 */