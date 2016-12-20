#ifndef REPL_STATE_H
#define REPL_STATE_H

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This file is distributed as part of our submissions to the Cache           //
// Replacement Championship workshop held in conjunction with ISCA'2010.      //
//                                                                            //
//                                                                            //
// Everyone is granted permission to copy, modify, and/or re-distribute       //
// this software.                                                             //
//                                                                            //
// Please contact Hongliang Gao <hlgao@hlgao.com> should you have any         //
// questions                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cassert>
#include "utils.h"
#include "crc_cache_defs.h"


// Knob to select one out of our three submitted configurations
// 1: basic without random promotion and only one policy has bypassing
// 2: without random promotion and both policies have bypassing
// 3: with random promotion and both policies have bypassing
#define CRC_CONFIG 1

#define CRC_NOBYPASS 0 // disable bypassing

/////////////////////////////////// Three configurations //////////////////////
#if CRC_CONFIG == 1
// Enable bypass or not
#define P0_BYPASS 1
#define P1_BYPASS 0
// Probability of random promotion
#define P0_RANDPROMOTION 0
#define P1_RANDPROMOTION 0
// Enable aging or not 
#define P0_AGING 0
#define P1_AGING 1
// Fixed probability of virtual bypass
#define VIRTUAL_BYPASS 16
// Second minimum bypass probability (minimum is 0)
#define BYPASS_MINP 256
// Initial bypass probability
#define BYPASS_INITIALP 64
// Seed of LFSR
#define RANDSEED 0x89ABCDEF

#elif CRC_CONFIG == 2
#define P0_BYPASS 1
#define P1_BYPASS 1
#define P0_RANDPROMOTION 0
#define P1_RANDPROMOTION 0
#define P0_AGING 0
#define P1_AGING 1
#define VIRTUAL_BYPASS 8
#define BYPASS_MINP 4096
#define BYPASS_INITIALP 64
#define RANDSEED 0x168BCDEF

#else
#define P0_BYPASS 1
#define P1_BYPASS 1
#define P0_RANDPROMOTION 0
#define P1_RANDPROMOTION 16
#define P0_AGING 0
#define P1_AGING 1
#define VIRTUAL_BYPASS 8
#define BYPASS_MINP 4096
#define BYPASS_INITIALP 8
#define RANDSEED 0xCEEDCEED

#endif
///////////////////////////////////////////////////////////////////////////
#define NUMSAMPLE 32   // sampling 1/32 of sets in selector
#define MAX_SELECTION_COUNTER 1023

// Replacement Policies Supported
typedef enum 
{
    CRC_REPL_LRU        = 0,
    CRC_REPL_RANDOM     = 1,
    CRC_REPL_CONTESTANT = 2
} ReplacemntPolicy;

// configuration of a policy
struct POLICY_CONFIG
{
    bool aging;                 // whether age LRU way on cache miss
    int  randp;                 // probability of random promotion
    bool enable_bypass;         // enable bypassing
};

// Replacement state for each set
struct SET_REPLACEMENT_STATE_STRUCT
{
    // which policy should be used for this set, not needed in hardware
    int pid;

    // fields added to support bypass (total is 22-bit per set)
    bool competitor_valid;       // indicate the competitor info is valid
    Addr_t bypass_tag;           // tag of the competitor (16-bit)
    int competitor_way;          // point to the competitor way (4-bit)
    bool is_virtual_bypass;      // indicate whether the bypass tracked is real bypass or virtual bypass
    
    // this bit is set when all ways are valid, not needed in hardware
    bool warmed;
};

class SET_REPLACEMENT_STATE : public SET_REPLACEMENT_STATE_STRUCT
{
public:
    // this will zero out all values
    SET_REPLACEMENT_STATE():SET_REPLACEMENT_STATE_STRUCT() { } 
};

// Replacement state per cache line
// The hardware cost is 5 bits per line for main tag directory
// and 22 bits per line for selector's auxiliary tag directory
struct MYLINE_REPLACEMENT_STATE_STRUCT
{
    // instead of using LRU stack to model true LRU, we use timestamp of last access for convenience
    COUNTER last_access;

    // mark whether a way has been re-referenced since allocation
    // it can also be set by random promotion during allocation
    bool ref;

    // 16-bit partial tag used in selector
    Addr_t selector_tag;
    // way valid used in selector
    bool selector_valid;
};

class MYLINE_REPLACEMENT_STATE : public MYLINE_REPLACEMENT_STATE_STRUCT
{
public:
    MYLINE_REPLACEMENT_STATE():MYLINE_REPLACEMENT_STATE_STRUCT() { }
};

// Replacement State Per Cache Line
typedef struct
{
    UINT32  LRUstackposition;

    // CONTESTANTS: Add extra state per cache line here
    MYLINE_REPLACEMENT_STATE my;
} LINE_REPLACEMENT_STATE;


// The implementation for the cache replacement policy
class CACHE_REPLACEMENT_STATE
{

  private:
    UINT32 numsets;
    UINT32 assoc;
    UINT32 replPolicy;
    
    LINE_REPLACEMENT_STATE   **repl;

    COUNTER mytimer;  // tracks # of references to the cache

    // CONTESTANTS:  Add extra state for cache here
    UINT32 num_bypassed;

  public:

    // The constructor CAN NOT be changed
    CACHE_REPLACEMENT_STATE( UINT32 _sets, UINT32 _assoc, UINT32 _pol );

    INT32  GetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc, Addr_t PC, Addr_t paddr, UINT32 accessType );
    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID );

    void   SetReplacementPolicy( UINT32 _pol ) { replPolicy = _pol; } 
    void   IncrementTimer() { 
        last_miss_addr = 0;
        mytimer++; 
    } 

    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
                                   UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit );

    ostream&   PrintStats( ostream &out);

  private:
    
    void   InitReplacementState();
    INT32  Get_Random_Victim( UINT32 setIndex );

    INT32  Get_LRU_Victim( UINT32 setIndex );
    void   UpdateLRU( UINT32 setIndex, INT32 updateWayID );

    //***************************** Added **************************************
    // replacement state per set
    SET_REPLACEMENT_STATE *repset;

    // configuration of the two policies tracked by selector
    POLICY_CONFIG policy[2];

    int bypass_global_p[2];   // dynamic bypass probability for each policy
    int bypass_min_p;         // second minimum bypass probability (minimum is 0)

    // used to make sure linesize is 64
    Addr_t last_miss_addr;

    // Lookup Parameters
    UINT32 lineShift;
    Addr_t lineMask;
    UINT32 indexShift;
    UINT32 indexMask;

    // selector's auxiliary tag directory 
    LINE_REPLACEMENT_STATE **selector_repl[2];
    SET_REPLACEMENT_STATE *selector_set[2];
    Addr_t selector_tag_mask;
    
    // policy selection counter
    int selector_global_ct;
   
    // used to detect whether a set is sampled in selector
    UINT32 selector_offset_bits;
    UINT32 selector_offset_mask;

    Addr_t bypass_tag_mask;

    // a 32-bit LFSR register used for random number generation
    UINT32 LFSR;

    ////////////////////////////////// Functions ///////////////////////////

    // the following three are interface functions to the framework
    void   MyInitReplacementState();
    INT32  MyGetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc, Addr_t PC,
            Addr_t lineaddr, UINT32 accessType );
    void   MyUpdateReplacementState( UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
            UINT32 tid, Addr_t PC, Addr_t lineaddr, UINT32 accessType, bool cacheHit );

    // Select a policy based on the selection counter
    void  SelectPolicy(SET_REPLACEMENT_STATE *curset);
    // get the victim way of a set
    INT32  GetVictim( LINE_REPLACEMENT_STATE *replSet);
    // LRU and ref bit update on cache miss
    bool  UpdateLRUandRefOnMiss(SET_REPLACEMENT_STATE *curset, LINE_REPLACEMENT_STATE *replSet, int vic, bool *vb);
    // LRU and ref bit update on cache hit
    void  UpdateLRUandRefOnHit(MYLINE_REPLACEMENT_STATE *mystate);
    // age ref bit of the LRU way
    void   HandleAging( LINE_REPLACEMENT_STATE *replSet, int vic );
    // track bypassing
    void  UpdateBypass(SET_REPLACEMENT_STATE *curset, LINE_REPLACEMENT_STATE *replSet, Addr_t lineaddr, 
            bool bypassed, bool vb, int assoc, int way, bool cacheHit, Addr_t viclineaddr, Addr_t PC, UINT32 accessType);

    // handle an access to a sampled set in selector
    void  SelectorAccess( UINT32 setIndex, SET_REPLACEMENT_STATE *curset, LINE_REPLACEMENT_STATE *replSet, 
            Addr_t lineaddr, Addr_t PC, UINT32 accessType);
    // return the way that hits in a selector set. if miss, return -1
    int   SelectorHit( LINE_REPLACEMENT_STATE *replSet, Addr_t tag );
    // return index of selector. if a set is not sampled in selector, return -1
    int   SelectorIsSample(UINT32 setIndex);
    // update selection counter
    void  SelectorCtUpdate(int *hit);
    void  CounterUpdate(int *ct, int max, bool inc);

    // utility functions:

    // return LRU way of a set. if not_referenced is set, exclude ways whose ref bits are set
    INT32  GetLRUWay(LINE_REPLACEMENT_STATE *replSet, bool not_referenced = false);
    // LFSR based random number generator. it returns true with a probability of 1/divisor
    bool RandMod(UINT32 divisor);
    Addr_t GetLineAddr( Addr_t addr ) { return (addr & lineMask); }
    // regenerate line addr from index and tag
    Addr_t GetLineAddr( UINT32 index, Addr_t tag ) { return (((tag << indexShift) | index) << lineShift); }
    Addr_t GetTag( Addr_t addr ) { return ((addr >> lineShift) >> indexShift); }
    // generate partial tag for bypass and selector
    Addr_t PartialTag( Addr_t lineaddr, Addr_t mask );
};


#endif
