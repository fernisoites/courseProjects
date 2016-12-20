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

    // Core count
    ncore = GET_CORE_COUNT(numsets); // Core count (we assume it as a constant value)

    // Initialize Per-Core Resources
    psel = new UINT32[ncore];
    mam = new MemoryAccessMapTable[ncore];
    bpf = new BypassFilterTable[ncore];
    for(UINT32 i=0; i<ncore; i++) {
      psel[i] = PSEL_THRES;
      mam[i].Init();
      bpf[i].Init();
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
        paddr = paddr / 64;
        return GetVictim(tid, setIndex, vicSet, assoc, PC, paddr, accessType );
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
        Addr_t paddr = ((currLine->tag * numsets) + setIndex);
        UpdateState(setIndex, updateWayID, currLine, assoc, tid, PC, paddr, accessType, cacheHit);
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

    // Print core count
    out<<"CORE COUNT: "<<ncore<<endl;

    return out;
    
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Function for Memory Access Map Table                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////
// Search corresponding zone
//////////////////////////////////////
INT32 MemoryAccessMapTable::Access(Addr_t tag) {
  Addr_t idx = tag % IDX_SIZE;
  
  // Search hit way from memory access map table.
  INT32 hitway = -1;
  for(INT32 way=0; way<WAY_SIZE; way++) {
    if(Table[idx][way].Hit(tag)) { hitway = way; }
  }
  return hitway; // When miss, -1 is returned.
}

void MemoryAccessMapTable::Update(Addr_t paddr, bool hit) {
  UINT32 offset = paddr % MAP_SIZE;
  Addr_t tag = paddr / MAP_SIZE;
  Addr_t idx = tag % IDX_SIZE;
  INT32 way = Access(tag);
  
  //////////////////////////////////////
  // Replace LRU entry
  //////////////////////////////////////
  if(way < 0) {  // Memory access map is not found in the table.
    way = 0;
    for(INT32 w=0; w<WAY_SIZE; w++){
      if(Table[idx][w].lru > Table[idx][way].lru) { way = w; } // Search LRU entry
    }
    Table[idx][way].Init(tag); // Allocate new entry
  }
  
  //////////////////////////////////////
  // Update LRU field
  //////////////////////////////////////
  for(INT32 w=0; w<WAY_SIZE; w++) {
    if(Table[idx][w].lru < Table[idx][way].lru) {
      Table[idx][w].lru++;
    }
  }
  Table[idx][way].lru = 0;
  Table[idx][way].Update(offset, hit);
}

UINT32 MemoryAccessMapTable::Read(Addr_t paddr, bool hit, struct MemoryAccessResult *result) {
  UINT32 offset = paddr % MAP_SIZE;
  Addr_t tag    = paddr / MAP_SIZE;
  
  /////////////////////////////////////////////////////////////
  // Read memory access map
  /////////////////////////////////////////////////////////////
  Addr_t lidx = tag % IDX_SIZE;
  INT32 lway = Access(tag);
  
  /////////////////////////////////////////////////////////////
  // Check whether the address is already accessed or not.
  /////////////////////////////////////////////////////////////
  bool detectHit = false;
  if(lway >= 0) { detectHit = Table[lidx][lway].IsAccess(offset); }
  
  /////////////////////////////////////////////////////////////
  // Collects reuse count and access count
  /////////////////////////////////////////////////////////////
  if(result != NULL) {
    result->reuseAccessCount = 0;
    result->totalAccessCount = 0;
    if(lway >= 0) result->reuseAccessCount += Table[lidx][lway].reuse;
    if(lway >= 0) result->totalAccessCount += Table[lidx][lway].access;
  }
  
  return detectHit;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Function for Map-based adaptive insertion policy                           //
// This is implemented on CACHE_REPLACEMENT_STATE.                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////
// Set Dueling Mechanism Support
///////////////////////////////////////////
enum InsertionPolicy CACHE_REPLACEMENT_STATE::GetSDM(UINT32 idx, UINT32 tid) {
  // Adaptive Decicated Set Reduction
  static const UINT32 SET_SKIP_SIZE = 4;
  for(UINT32 i=0; i<SET_SKIP_SIZE; i++) {
    UINT32 t = SET_SKIP_SIZE-i;
    bool range_h = psel[tid] > (PSEL_MAX - ((PSEL_RANGE/8)*t));
    bool range_l = psel[tid] < (PSEL_MIN + ((PSEL_RANGE/8)*t));
    if(((idx >> (i+1)) & 1) && (range_h || range_l)) {
      if(psel[tid] > PSEL_THRES) { return FOLLOWER_LRU; }
      else                       { return FOLLOWER_MAP; }
    }
  }
  
  // Calculating Replacement Policy
  INT32  bits = (CRC_FloorLog2(numsets) - 5);
  UINT32 mask = (1 << bits) - 1;
  UINT32 idx_h = idx >> bits;
  UINT32 idx_l = idx &  mask;
  
  idx_h += (2*tid); idx_h &= mask;
  if(idx_h == idx_l) return DEDICATED_LRU;
  idx_h += 1; idx_h &= mask;
  if(idx_h == idx_l) return DEDICATED_MAP;
  
  if(psel[tid] > PSEL_THRES) { return FOLLOWER_LRU; }
  else                       { return FOLLOWER_MAP; }
}

//////////////////////////////////////
// Updating State
//////////////////////////////////////
void CACHE_REPLACEMENT_STATE::Update(Addr_t paddr, Addr_t PC, bool cacheHit,
				     bool mamHit, UINT32 tid) {
  mam[tid].Update(paddr, cacheHit);
  bpf[tid].Update(PC, cacheHit, !(cacheHit || mamHit));
}

//////////////////////////////////////
// Calculating Insertion Position
//////////////////////////////////////
UINT32 CACHE_REPLACEMENT_STATE::GetInsertPos(Addr_t paddr, Addr_t PC,
					     UINT32 idx, bool cacheHit, UINT32 tid) {
  struct MemoryAccessResult result;
  bool mamHit = mam[tid].Read(paddr, cacheHit, &result);
  
  Update(paddr, PC, cacheHit, mamHit, tid);
  // This flag indicates a reusability of a corresponding zone.
  bool reusable = result.reuseAccessCount >= result.totalAccessCount;
  
  // Prediction From Spatial Locality
  UINT32 pos_map = assoc - 1;
  if(mamHit)   { pos_map = pos_map/2; }
  if(reusable) { pos_map = pos_map/2; }
  
  // Prediction From Temporal Locality
  if(bpf[tid].IsReuse(PC))   { pos_map =         0; } // Reuse Line Insert to MRU
  if(bpf[tid].IsUseful(PC))  { pos_map = pos_map/2; } // Useful Line Insert to Middle
  if(bpf[tid].IsUseless(PC)) { pos_map =   assoc-1; } // Useless Line Insert to LRU
  
  // Cache Hit Entries are Promote to MRU position
  if(cacheHit) { pos_map = 0; }
  
  // Update Insertion Policy
  switch(GetSDM(idx, tid)) {
  case DEDICATED_LRU: if( (!cacheHit) && (psel[tid] > PSEL_MIN) ) { psel[tid] -= 1; }
  case FOLLOWER_LRU : return       0;
  case DEDICATED_MAP: if( (!cacheHit) && (psel[tid] < PSEL_MAX) ) { psel[tid] += 1; }
  case FOLLOWER_MAP : return pos_map;
  default: break;
  }
  return 0;
}

//////////////////////////////////////
// Cache Bypass Detection
//////////////////////////////////////
bool CACHE_REPLACEMENT_STATE::JudgeBypass(Addr_t paddr, Addr_t PC, UINT32 idx, UINT32 tid) {
  struct MemoryAccessResult result;
  mam[tid].Read(paddr, false, &result);
  if((result.totalAccessCount/16) > result.reuseAccessCount) { return true; }
  if(bpf[tid].IsBypass(PC)) { return true; }
  return false;
}

INT32 CACHE_REPLACEMENT_STATE::GetVictim(UINT32 tid, UINT32 setIndex,
					 const LINE_STATE *vicSet, UINT32 assoc,
					 Addr_t PC, Addr_t paddr, UINT32 accessType ) {
  ////////////////////////////////////////////////
  // Judge Cache Bypass or Not
  ////////////////////////////////////////////////
  if(JudgeBypass(paddr, PC, setIndex, tid)) {
    GetInsertPos(paddr, PC, setIndex, false, tid); // Update State
    return -1;
  }
  ////////////////////////////////////////////////
  // Search LRU Way
  ////////////////////////////////////////////////
  for(INT32 way=0; way<(INT32)assoc; way++) {
    if(repl[setIndex][way].LRUstackposition == (assoc-1)) { return way; }
  }
  
  return 0;
}

void CACHE_REPLACEMENT_STATE::UpdateState(UINT32 setIndex, INT32 updateWayID,
					  const LINE_STATE *currLine, UINT32 assoc, 
					  UINT32 tid, Addr_t PC, Addr_t paddr,
					  UINT32 accessType, bool cacheHit) {
  ////////////////////////////////////////////////
  // Get Insertion Position
  ////////////////////////////////////////////////
  UINT32 pos = GetInsertPos(paddr, PC, setIndex, cacheHit, tid);
  
  ////////////////////////////////////////////////
  // LRU Update
  ////////////////////////////////////////////////
  UINT32 updt = repl[setIndex][updateWayID].LRUstackposition;
  for(INT32 way=0; way<(INT32)assoc; way++) {
    if(way != updateWayID) {
      if( repl[setIndex][way].LRUstackposition < updt ) {
	repl[setIndex][way].LRUstackposition += 1;
      }
      if( repl[setIndex][way].LRUstackposition <= pos ) {
	repl[setIndex][way].LRUstackposition -= 1;
      }
    }
    assert(repl[setIndex][way].LRUstackposition >= 0);
    assert(repl[setIndex][way].LRUstackposition < 16);
  }
  repl[setIndex][updateWayID].LRUstackposition = pos;
}