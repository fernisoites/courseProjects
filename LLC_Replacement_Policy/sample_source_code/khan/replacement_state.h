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
    
    // First four parametres are needed for this policy
    UINT32 *setType;  
    // setType is 2 bit variable
    // setType 0 : Follower set
    // setType 1 : Leader set that inserts in the middlePos
    // setType 2 : Leader set that inserts in the MRUPos,this is actually LRU policy
    // setType 3 : Leader set that inserts in a position adpatively chosen

    UINT32 psel1 ;     
    // psel1 is a counter, it has log2(assoc) bits,
    // for single core 1MB 16 way LLC, it has 10 bits
    // this counter is resposible for selecting winner policy between middlePos and MRUPos

    UINT32 psel2 ;    
    // psel2 is also a counter, it has log2(assoc) bits,
    // for single core 1MB 16 way LLC, it has 10 bits
    // this counter is resposible for selecting winner policy between 1st round winner 
    // and adaptive policy chosen by setType 3

    UINT32 switched;  
    // switched is a 1 bit counter, it keeps track of the policy used in setType 3

    // These values are constant and read only parameters
    UINT32 numDedicatedSet;  // number of leader set of each type
    UINT32 middlePos;	     // this is the insert position in the middle of the LRU stack
    UINT32 nearMRUPos;       // this is the insert position that is nearer to MRUPos
    UINT32 nearLRUPos;       // this is the insert position that is nearer to LRUPos    		
     
   
  public:

    // The constructor CAN NOT be changed
    CACHE_REPLACEMENT_STATE( UINT32 _sets, UINT32 _assoc, UINT32 _pol );

    INT32  GetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc, Addr_t PC, Addr_t paddr, UINT32 accessType );
    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID );

    void   SetReplacementPolicy( UINT32 _pol ) { replPolicy = _pol; } 
    void   IncrementTimer() { mytimer++; } 
   
    void   IncrementLocalTimer(UINT32 setIndex) ;
    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
                                   UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit );

    ostream&   PrintStats( ostream &out);

  private:
    
    void   InitReplacementState();
    INT32  Get_Random_Victim( UINT32 setIndex );

    INT32  Get_LRU_Victim( UINT32 setIndex );
    void   UpdateLRU( UINT32 setIndex, INT32 updateWayID );

    void    InsertToPos( UINT32 setIndex, INT32 updateWayID , UINT32 pos);
    

  
};


#endif
