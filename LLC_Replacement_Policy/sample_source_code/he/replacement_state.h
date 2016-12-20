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

#define asrp_dedicated_sets 4 //sample sets number for each threshold

#define THRESHOLD_SAMPLE_1 1  //sample sets' threshold 
#define THRESHOLD_SAMPLE_2 2
#define THRESHOLD_SAMPLE_3 4
#define THRESHOLD_SAMPLE_4 8
#define THRESHOLD_SAMPLE_5 16
#define THRESHOLD_SAMPLE_6 32 
#define THRESHOLD_SAMPLE_7 64 
#define THRESHOLD_SAMPLE_8 128	//sample sets' threshold 

#define BASE_SET_THRESHOLD 4  	//initial threshold for followed_sets in ASRP. 
#define BASE_GROUP_NUM     4  	//initial subset number for sets in ASRP. 
#define SMOOTH_THRESHOLD   4096	//sample miss count back to 0 when follow_set choose one threshold more than it in ASRP

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

    UINT32  flag;	//differentiate dedicate sample sets from normal sets. -1 normal sets, 1-8 sample sets. 

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

    UINT32 last_follow;			//record the normal sets' last threshold choice 
    UINT32 global_follow;		//record the times that normal sets keep on taking a same threshold
    UINT32 *miss_counter_group;		//count the misses happened on corresponding sample set
    UINT32 *active_group;		//record the current active subset for each set
    UINT32 *set_miss;			//count the miss number for certain cache set
    UINT32 threshold;			//equal to BASE_GROUP_NUM
    UINT32 num_group;			//equal to BASE_SET_THRESHOLD


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
    INT32  Get_ASRP_Victim( UINT32 setIndex );		//get asrp victim
    void   UpdateLRU( UINT32 setIndex, INT32 updateWayID );
};


#endif
