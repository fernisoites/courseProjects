#ifndef REPL_STATE_H
#define REPL_STATE_H

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

#include <cstdlib>
#include <cassert>
#include "utils.h"
#include "crc_cache_defs.h"


#define GET_CORE_COUNT(X) (X/1024)
// For Single Core Track
// #define GET_CORE_COUNT(X) 1
// For Multi Core Track
// #define GET_CORE_COUNT(X) 4


// Replacement Policies Supported
typedef enum 
{
    CRC_REPL_LRU        = 0,
    CRC_REPL_RANDOM     = 1,
    CRC_REPL_CONTESTANT = 2
} ReplacemntPolicy;

// Replacement State Per Cache Line
typedef struct
{
    UINT32  LRUstackposition;

    // CONTESTANTS: Add extra state per cache line here

} LINE_REPLACEMENT_STATE;


//******************************************//
// Additional Data Structure and Constants  //
//******************************************//

////////////////////////////////////////////
// Data structure for Set Dueling Monitor //
////////////////////////////////////////////
enum InsertionPolicy {
  DEDICATED_LRU = 0,
  FOLLOWER_LRU  = 1,
  FOLLOWER_MAP  = 2,
  DEDICATED_MAP = 3,
};

////////////////////////////////////////////
// Data structure for Memory Access Map   //
////////////////////////////////////////////
enum MemoryAccessState {
  AMPM_INIT    = 0,
  AMPM_ACCESS  = 1,
};

////////////////////////////////////////////
// Temporal Data                          //
////////////////////////////////////////////
struct MemoryAccessResult {
  int reuseAccessCount;
  int totalAccessCount;
};

////////////////////////////////////////////
// Constants Values                       //
////////////////////////////////////////////

// for bypass filter table
#define BFT_SIZE 512
#define BFT_MASK (BFT_SIZE-1)
#define BFT_MAX  127
#define BFT_MIN -128

// for memory access map
#define MAP_SIZE 256
#define IDX_SIZE 16
#define WAY_SIZE 12
#define COUNT_MAX (MAP_SIZE-1)
#define TAG_BITS 30
#define TAG_MASK ((1<<TAG_BITS)-1)

// for policy selection counter
#define PSEL_BIT   10
#define PSEL_RANGE (1<<PSEL_BIT)
#define PSEL_MIN   0
#define PSEL_MAX   ((1<<PSEL_BIT)-1)
#define PSEL_THRES (1<<(PSEL_BIT-1))

////////////////////////////////////////////
// Bypass Filter Table                    //
////////////////////////////////////////////

class BypassFilterTable {
  INT32 table[BFT_SIZE]; // 512 * 8 bit = 4096 bit

public:
  BypassFilterTable() { }
  
  void Init() { for(int i=0; i<BFT_SIZE; i++) { table[i] = 0; } }

  // Read Table
  bool IsBypass (int idx) { return table[idx & BFT_MASK] < (BFT_MIN + 32); }
  bool IsReuse  (int idx) { return table[idx & BFT_MASK] > (BFT_MAX - 32); }
  bool IsUseless(int idx) { return table[idx & BFT_MASK] < (BFT_MIN + 64); }
  bool IsUseful (int idx) { return table[idx & BFT_MASK] > (BFT_MAX - 64); }

  // Update Table
  void Update(int idx, bool inc, bool dec) {
    idx &= BFT_MASK;
    if(inc) {
      if(IsUseless(idx)) { table[idx] += 8; }
      else               { table[idx] += 1; }
      if(table[idx] > BFT_MAX){ table[idx] = BFT_MAX; }
    }
    if(dec) {
      if(IsUseful(idx))  { table[idx] -= 8; }
      else               { table[idx] -= 1; }
      if(table[idx] < BFT_MIN){ table[idx] = BFT_MIN; }
    }
  }
};

////////////////////////////////////////////
// Memory Access Map                      //
////////////////////////////////////////////

class MemoryAccessMap {
public:
  Addr_t tag;    // 30 bit
  int    lru;    // 4 bit
  int    reuse;  // 8 bit
  int    access; // 8 bit
  enum MemoryAccessState MAP[MAP_SIZE]; // 256 bit
  
public:
  MemoryAccessMap() { }
  
  void Init(Addr_t t) {
    reuse = 0;
    access = 0;
    tag = t & TAG_MASK;
    for(int i=0; i<MAP_SIZE; i++) { MAP[i] = AMPM_INIT; }
  }

  bool Hit      (Addr_t t) { return ((t & TAG_MASK) == tag); }
  bool IsAccess (Addr_t n) { return (MAP[n] == AMPM_ACCESS); }

  // Update State and Counters
  void Update(Addr_t offset, bool hit) {
    if(IsAccess(offset)) {
      if(reuse < COUNT_MAX) { reuse += 1; }
    } else {
      MAP[offset] = AMPM_ACCESS;
      if(access < COUNT_MAX) { access +=            1 ; }
      if(reuse  < COUNT_MAX) { reuse  += (hit ? 1 : 0); }
    }
  }
};


////////////////////////////////////////////
// Memory Access Map Table                //
////////////////////////////////////////////
class MemoryAccessMapTable {
  MemoryAccessMap Table[IDX_SIZE][WAY_SIZE]; // 306 bit x 192 entries

public:
  void Init() {
    for(int i=0; i<IDX_SIZE; i++) {
      for(int j=0; j<WAY_SIZE; j++) {
	Table[i][j].Init(j);
	Table[i][j].lru = j;
      }
    }
  }

  //////////////////////////////////////
  // Search corresponding zone
  //////////////////////////////////////
  void Update(Addr_t paddr, bool hit);
  INT32 Access(Addr_t tag);
  UINT32 Read(Addr_t paddr, bool hit, struct MemoryAccessResult *result);
};

//******************************************//
// End of Additional Data Structure         //
//******************************************//


// The implementation for the cache replacement policy
class CACHE_REPLACEMENT_STATE
{

  private:
    UINT32 numsets;
    UINT32 assoc;
    UINT32 replPolicy;
    
    COUNTER mytimer;  // tracks # of references to the cache

    // CONTESTANTS:  Add extra state for cache here

//******************************************//
// Start of Additional Method for CRC       //
//******************************************//

    //////////////////////////////////////////
    // Budget Counting
    //////////////////////////////////////////

    // Additional Constant (0 bit)
    UINT32 ncore; // Number of core
    
    // Shared Resource (LRU bit)
    // Single core track:  65536 bit
    // Multi  core track: 262144 bit
    LINE_REPLACEMENT_STATE **repl;
    
    // Per-core Resource Total // 62858 bit / core
    MemoryAccessMapTable *mam; // 58752 bit / core
    BypassFilterTable    *bpf; //  4096 bit / core
    UINT32              *psel; //    10 bit / core

    ///////////////////////////////////////////
    // Set Dueling Mechanism Support
    ///////////////////////////////////////////
    enum InsertionPolicy GetSDM(UINT32 idx, UINT32 tid);
    
    //////////////////////////////////////
    // Updating State
    //////////////////////////////////////
    void Update(Addr_t paddr, Addr_t PC, bool cacheHit, bool mamHit, UINT32 tid);
    
    //////////////////////////////////////
    // Calculating insertion position
    //////////////////////////////////////
    UINT32 GetInsertPos(Addr_t paddr, Addr_t PC, UINT32 idx, bool cacheHit, UINT32 tid);
    
    //////////////////////////////////////
    // Cache bypass detection
    //////////////////////////////////////
    bool JudgeBypass(Addr_t paddr, Addr_t PC, UINT32 idx, UINT32 tid);
    
    //////////////////////////////////////
    // Selects victim set
    //////////////////////////////////////
    INT32 GetVictim(UINT32 tid, UINT32 setIndex,
		    const LINE_STATE *vicSet, UINT32 assoc,
		    Addr_t PC, Addr_t paddr, UINT32 accessType );

    //////////////////////////////////////
    // Updates internal state
    //////////////////////////////////////
    void UpdateState(UINT32 setIndex, INT32 updateWayID,
		     const LINE_STATE *currLine, UINT32 assoc, 
		     UINT32 tid, Addr_t PC, Addr_t paddr,
		     UINT32 accessType, bool cacheHit);

//******************************************//
// End of Additional Function               //
//******************************************//
    
  public:

    // The constructor CAN NOT be changed
    CACHE_REPLACEMENT_STATE( UINT32 _sets, UINT32 _assoc, UINT32 _pol );

    INT32  GetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc, Addr_t PC, Addr_t paddr, UINT32 accessType );
    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID );

    void   SetReplacementPolicy( UINT32 _pol ) { replPolicy = _pol; } 
    void   IncrementTimer() { mytimer++; } 

    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
                                   UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit );

    ostream&   PrintStats( ostream &out);

  private:
    
    void   InitReplacementState();
    INT32  Get_Random_Victim( UINT32 setIndex );

    INT32  Get_LRU_Victim( UINT32 setIndex );
    void   UpdateLRU( UINT32 setIndex, INT32 updateWayID );
};


#endif
