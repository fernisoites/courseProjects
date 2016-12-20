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
            repl[ setIndex ][ way ].use = 0;
        }
    }

    // Contestants:  ADD INITIALIZATION FOR YOUR HARDWARE HERE

    for (int i=0; i<4; i++) p[i] = 0;
    for (int i=0; i<4; i++) tmiss[i] = 0;
    for (int i=0; i<4; i++) bipctr[i] = 0;
    hand = new int [numsets];
    assert(hand);
    for (int i=0; i<(int)numsets; i++) {
      hand[i] = 0;
    }

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
      return Get_Victim(setIndex);
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
      Update_State(setIndex, updateWayID, tid, accessType, cacheHit);
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

    return out;
    
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Proposed replacement policy                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// PM: we use numsets to know if it is the 1M single-core or the 4M multi-core
#define MULTICORE_CONFIG (numsets > 1024)

// PM: NPOL=3 for policy 3P, NPOL=4 for policy 4P
#define NPOL ((MULTICORE_CONFIG)? 4:3)

// PM: INSERT_RATIO is the inverse of the bimodal throttling parameter (epsilon)
#define INSERT_RATIO ((MULTICORE_CONFIG)? 32:64)

// PM: INSERT_LENGTH is the parameter called L in the paper
#define INSERT_LENGTH 64

// PM: RSIZE is the constituency size for set sampling (cf. Qureshi's ISCA'06 paper)
#define RSIZE 64

// PM: PBITS is the number of bits of each P[i] counter
#define PBITS 11

// PM: TMISSBITS is the number of bits of each TMISS counter (4P policy)
#define TMISSBITS 14

#define PMIN (-(1<<(PBITS-1)))
#define PMAX (-PMIN-1)
#define TMISSMIN (-(1<<(TMISSBITS-1)))
#define TMISSMAX (-TMISSMIN-1)



INT32 CACHE_REPLACEMENT_STATE::Get_Victim(UINT32 setIndex)
{
  while (1) {
    if (! repl[setIndex][hand[setIndex]].use) {
      break;
    }
    repl[setIndex][hand[setIndex]].use = 0;
    hand[setIndex] = (hand[setIndex] + 1) % assoc;
  }
  return hand[setIndex];
}


void CACHE_REPLACEMENT_STATE::Update_State(UINT32 setIndex, INT32 updateWayID, UINT32 tid, UINT32 accessType, bool cacheHit)
{

  if (accessType == ACCESS_WRITEBACK) return; // PM: write-back bypass

  if (!cacheHit && (updateWayID != hand[setIndex])) {
    // PM: victim was invalid block
    return;
  }

  if (cacheHit) {
    repl[setIndex][updateWayID].use = 1;
    return ;
  } 

  // PM: below is miss treatment

  int st = (setIndex ^ (setIndex/RSIZE)) % RSIZE;

  if (st < NPOL) {
    // PM: we access a dedicated set, update the P[i] counters
    bool saturate = ((p[st]+NPOL-1) > PMAX);
    for (int i=1; i<NPOL; i++) {
      saturate = saturate || ((p[(st+i)%NPOL]-1) < PMIN);
    }
    if (! saturate) {
      p[st] += NPOL-1;
      for (int i=1; i<NPOL; i++) {
	p[(st+i)%NPOL]--;
      }
    }
  }

  // PM: find which policy is best
  int bp = 0;
  for (int i=1; i<NPOL; i++) {
    bp = (p[i] < p[bp])? i : bp;
  }

  if ((st==0) || ((st>=NPOL) && (bp==0))) {
    // PM: normal CLOCK
    repl[setIndex][updateWayID].use = 1;
  } else if ((st==1) || ((st>=NPOL) && (bp==1))) {
    // PM: CLOCK BIP, epsilon = 1/INSERT_RATIO
    if (bipctr[tid] < INSERT_LENGTH) {
      repl[setIndex][updateWayID].use = 1;
    }
  } else if ((st==2) || ((st>=NPOL) && (bp==2))) {
    // PM: CLOCK BIP, epsilon = 1/2
    if ((bipctr[tid] % (2*INSERT_LENGTH))  < INSERT_LENGTH) {
      repl[setIndex][updateWayID].use = 1;
    }
  } else if ((st==3) || ((st>=NPOL) && (bp==3))) {
    // PM: CLOCK TUBIP, used only by 4P
    if ((bipctr[tid] < INSERT_LENGTH) || (tmiss[tid] < 0)) {
      repl[setIndex][updateWayID].use = 1;
    }
  } else {
    assert(0);
  }

  bipctr[tid] = (bipctr[tid]+1) % (INSERT_RATIO * INSERT_LENGTH);

  if (MULTICORE_CONFIG && (st==3)) {
    if (((tmiss[tid]+3) <= TMISSMAX) && ((tmiss[(tid+1)%4]-1) >= TMISSMIN) && ((tmiss[(tid+2)%4]-1) >= TMISSMIN) && ((tmiss[(tid+3)%4]-1) >= TMISSMIN)) {
      tmiss[tid] += 3;
      tmiss[(tid+1)%4]--;
      tmiss[(tid+2)%4]--;
      tmiss[(tid+3)%4]--;
    }
  }

}
