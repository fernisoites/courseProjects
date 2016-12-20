#include "replacement_state.h"

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
    // initialize added structures
    MyInitReplacementState();
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
    last_miss_addr = paddr;

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
        return MyGetVictimInSet (tid, setIndex, vicSet, assoc, PC, GetLineAddr(paddr), accessType);
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
    // make sure linesize is 64
    if (!cacheHit && last_miss_addr) 
        assert(currLine->tag == GetTag(last_miss_addr));

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

        // regenerate line address from set index and tag
        Addr_t lineaddr = GetLineAddr(setIndex, currLine->tag);
        MyUpdateReplacementState(setIndex, updateWayID, currLine, tid, PC, lineaddr, accessType, cacheHit);
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
    out<<"bypassed:"<<num_bypassed<<endl;
    return out;
}

//***************************** Added **************************************

// initilization of the algorithm
// some values are hard coded to only support CRC1 default configurations
void CACHE_REPLACEMENT_STATE::MyInitReplacementState()
{
    assert(numsets == 1024 || numsets == 4096);

    LFSR = RANDSEED;

    lineShift  = 6; // hard coded for 64 bytes line size
    lineMask   = ~((1 << lineShift) - 1);
    indexShift = (numsets == 1024 ? 10 : 12);    
    indexMask  = (1 << indexShift) - 1;

    // selection counter is initilizaed to 0
    selector_global_ct = 0;

    num_bypassed = 0;
    last_miss_addr = 0;

    // get config of policies
    policy[0].aging = P0_AGING;
    policy[0].randp = P0_RANDPROMOTION;
    policy[0].enable_bypass  = (P0_BYPASS && !CRC_NOBYPASS);
    policy[1].aging = P1_AGING;
    policy[1].randp = P1_RANDPROMOTION;
    policy[1].enable_bypass  = (P1_BYPASS && !CRC_NOBYPASS);

    // allocate state for each cache set
    repset  = new SET_REPLACEMENT_STATE [ numsets ];
    assert(repset);

    // bypass tag mask
    bypass_tag_mask = ((Addr_t)1 << (Addr_t)16) - 1;

    // bypass probability for each policy
    bypass_global_p[0] = BYPASS_INITIALP;
    bypass_global_p[1] = BYPASS_INITIALP;

    // selector tag mask
    selector_tag_mask =((Addr_t)1 << (Addr_t)16) - 1;

    // get number of sampled sets
    UINT32 sample = numsets / NUMSAMPLE;

    // create tag directories for selector
    for (int p = 0; p < 2; p++) {
        selector_set[p] = new SET_REPLACEMENT_STATE [ sample ];
        selector_repl[p] = new LINE_REPLACEMENT_STATE* [ sample ];
        assert(selector_set[p] && selector_repl[p]);
        for(UINT32 setIndex=0; setIndex<sample; setIndex++) {
            selector_repl[p][setIndex]  = new LINE_REPLACEMENT_STATE[assoc];
            assert(selector_repl[p][setIndex]);
            selector_set[p][setIndex].pid = p;
            // for selector, we initialize this indication to true to enable bypassing track earlier
            selector_set[p][setIndex].warmed = true;
        }
    }

    // prepare masks to help decide whether a set is sampled
    assert(NUMSAMPLE == 32);
    UINT32 sample_bits = (sample == 32 ? 5 : 7);
    selector_offset_bits = (numsets == 1024 ? 10 : 12) - sample_bits;
    assert(sample_bits >= selector_offset_bits);
    selector_offset_mask = ((1 << selector_offset_bits) - 1);

    // set up per-set policy
    for(UINT32 setIndex=0; setIndex<numsets; setIndex++)
        repset[setIndex].pid = 0;

}

// get victim way of a set (SLRU)
INT32  CACHE_REPLACEMENT_STATE::GetVictim( LINE_REPLACEMENT_STATE *replSet ) {
    INT32 vic = -1;

    // first search ways that have not been referenced
    vic = GetLRUWay(replSet, true);
    
    // if all ways have been referenced, use LRU of all ways
    if (vic == -1) 
        vic = GetLRUWay(replSet);

    return vic;
}

// Aging the LRU way
void   CACHE_REPLACEMENT_STATE::HandleAging( LINE_REPLACEMENT_STATE *replSet, int vic ){
    assert(vic != -1);
    int way = GetLRUWay(replSet);
    assert(way != vic);
    replSet[way].my.ref = false;
}

// return LRU way of a set. if not_referenced is set, exclude ways whose ref bits are set
INT32 CACHE_REPLACEMENT_STATE::GetLRUWay(LINE_REPLACEMENT_STATE *replSet, bool not_referenced) {
    INT32 vic = -1;
    COUNTER min = 0;

    for (UINT32 w = 0; w < assoc; w++) {
        if ((replSet[w].my.last_access < min || vic == -1) && (!replSet[w].my.ref || !not_referenced)) {
            vic = w;
            min = replSet[w].my.last_access;
        }
    }

    assert(vic != -1 || not_referenced);
    return vic;
}

// Select a policy based on the selection counter
void  CACHE_REPLACEMENT_STATE::SelectPolicy(SET_REPLACEMENT_STATE *curset) {
    curset->pid = (selector_global_ct >= 1 ? 1 : 0);
}

// Update LRU and ref bit on cache hit
void  CACHE_REPLACEMENT_STATE::UpdateLRUandRefOnHit(MYLINE_REPLACEMENT_STATE *mystate) {
    // record timestamp of last access for LRU
    mystate->last_access = mytimer;
    mystate->ref = true;
}

// Update LRU and ref bit on cache miss
bool  CACHE_REPLACEMENT_STATE::UpdateLRUandRefOnMiss(SET_REPLACEMENT_STATE *curset, LINE_REPLACEMENT_STATE *replSet, int vic, bool *vb) {
    assert(vic != -1);

    MYLINE_REPLACEMENT_STATE *mystate = &replSet[vic].my;

    bool bypassed = false;
    bool promoted = false;

    int randp = policy[curset->pid].randp;

    if (RandMod(randp)) // random promotion
        promoted = true;

    if (policy[curset->pid].enable_bypass && curset->warmed) {
        // bypass
        bypassed = RandMod(bypass_global_p[curset->pid]);
    }

    if (!bypassed) {
        mystate->last_access = mytimer;
        mystate->ref = false;

        // virtual bypass with a fixed probability
        int vbypass = VIRTUAL_BYPASS;
        (*vb) = (policy[curset->pid].enable_bypass && curset->warmed && RandMod(vbypass));

        // promote the line by setting ref bit
        if (promoted)
            mystate->ref = true;
    }
    return bypassed;
}

// track effectiveness of bypassing
void  CACHE_REPLACEMENT_STATE::UpdateBypass(SET_REPLACEMENT_STATE *curset, LINE_REPLACEMENT_STATE *replSet, Addr_t lineaddr, bool bypassed, bool vb, int assoc, int way, bool cacheHit, Addr_t viclineaddr, Addr_t PC, UINT32 accessType) {
    assert(way != -1);
    assert(!(vb && bypassed));

    // for real bypass, we record the tag of the bypassed line
    // for virtual bypass, we record the tag of the victim line that has been replaced by the missed line
    Addr_t tag = PartialTag( vb ? viclineaddr : lineaddr, bypass_tag_mask); 

    bool new_tag = false;
    // get current bypass probability of the selected policy
    int *bypass_p = &bypass_global_p[curset->pid];

    // start tracking a bypass
    if ((bypassed || vb) && !curset->competitor_valid) { 
        assert(!cacheHit);
        curset->competitor_valid = true;
        curset->bypass_tag = tag;
        curset->competitor_way = way;
        curset->is_virtual_bypass = vb;
        new_tag = true;
    }

    // decision of whether a tracked bypass is effective
    bool bypass_effective = false;
    bool bypass_ineffective = false;

    // only make a decision on a demand access
    if (!new_tag && curset->competitor_valid && accessType<=ACCESS_STORE) {
        if (!curset->is_virtual_bypass) {  // real bypassed line
            if (cacheHit && way == curset->competitor_way)
                bypass_effective = true;   // hit the victim line earlier
            else if (tag == curset->bypass_tag)
                bypass_ineffective = true; // hit the bypassed line earlier
        }else { // virtual bypass
            if (cacheHit && way == curset->competitor_way)
                bypass_ineffective = true; // hit the missed line (vitually bypassed) earlier
            else if (tag == curset->bypass_tag)
                bypass_effective = true;   // hit the victim line earlier
        }

        // the competitor way is selected as the victim way, invalidate bypass tracking info
        if (!cacheHit && way == curset->competitor_way) {
            curset->competitor_valid = false;
        }

        assert(!(bypass_effective && bypass_ineffective));
        
        if (bypass_effective) {
            if ((*bypass_p) == 0)
                (*bypass_p) = BYPASS_MINP; // off to on
            else if ((*bypass_p) != 1)     // increase probability
                (*bypass_p) /= 2;
        }else if (bypass_ineffective) {
            if ((*bypass_p) < BYPASS_MINP) // reduce probability
                (*bypass_p) *= 2;
            else
                (*bypass_p) = 0;           // turn off
        }

        // a decision has been made, clear bypass tracking info
        if (bypass_effective || bypass_ineffective)
            curset->competitor_valid = false;
    }
}

// Handle a cache miss
INT32  CACHE_REPLACEMENT_STATE::MyGetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc, Addr_t PC, Addr_t lineaddr, UINT32 accessType ) {

    SET_REPLACEMENT_STATE *curset = &repset[setIndex];
    LINE_REPLACEMENT_STATE *replSet = repl[setIndex];

    // get policy based on the selection counter
    SelectPolicy(curset);

    INT32 vic = GetVictim(replSet);
    INT32 oldvic = vic;

    bool vb = false; // virtual bypass
    bool bypass =    // real bypass
        UpdateLRUandRefOnMiss(curset, replSet, vic, &vb);

    if (!bypass){
        if (policy[curset->pid].aging)
            HandleAging( replSet, vic );
    }else {
        // even bypassed, still need to update some replacement state and train the selector
        MyUpdateReplacementState(setIndex, -1, vicSet, tid, PC, lineaddr, accessType, false);
        vic = -1;
    }

    if (policy[curset->pid].enable_bypass) {
        Addr_t viclineaddr = 0;
        if (vic != -1) // get line address of the victim line
            viclineaddr = GetLineAddr(setIndex, vicSet[vic].tag);
        UpdateBypass(curset, replSet, lineaddr, bypass, vb, assoc, 
                oldvic, false, viclineaddr, PC, accessType);
    }
    return vic;
}

// Update replacement state
void  CACHE_REPLACEMENT_STATE::MyUpdateReplacementState( UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, UINT32 tid, Addr_t PC, Addr_t lineaddr, UINT32 accessType, bool cacheHit ){
    // a bypassed miss
    // currLine is vicSet from GetVictimInSet()
    // lineaddr is line address of the missed line
    if (updateWayID == -1) {
        num_bypassed ++;
        assert(!cacheHit);
    }

    SET_REPLACEMENT_STATE *curset = &repset[setIndex];
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];

    SelectPolicy(curset);

    if (cacheHit) {
        UpdateLRUandRefOnHit(&replSet[updateWayID].my);
        if (policy[curset->pid].enable_bypass)
            UpdateBypass(curset, replSet, lineaddr, false, false, assoc, updateWayID, true, 0, PC, accessType);
    }else if (!curset->warmed) {
        assert(updateWayID != -1);
        bool vb = false;
        // during warmup, still update ref and LRU (GetVictim is not called during warmup)
        UpdateLRUandRefOnMiss(curset, replSet, updateWayID, &vb);
        if (updateWayID == (INT32)(assoc - 1))
            curset->warmed = true;
    }
    
    // train selector
    if (SelectorIsSample(setIndex) != -1) {
        SelectorAccess( setIndex, curset, replSet, lineaddr, PC, accessType );
    }
}

// get partial tag of a line address
Addr_t   CACHE_REPLACEMENT_STATE::PartialTag( Addr_t lineaddr, Addr_t mask ){
    Addr_t tag = GetTag(lineaddr);
    tag &= mask;
    return tag;
}

// check whether hit in a selector set
int   CACHE_REPLACEMENT_STATE::SelectorHit( LINE_REPLACEMENT_STATE *replSet, Addr_t tag ) {
    for (UINT32 w = 0; w < assoc; w++)
        if (replSet[w].my.selector_tag == tag && replSet[w].my.selector_valid)
            return w;
    return -1;
}

// update selector for sets that are sampled
void   CACHE_REPLACEMENT_STATE::SelectorAccess( UINT32 setIndex, SET_REPLACEMENT_STATE *curset, LINE_REPLACEMENT_STATE *replSet, Addr_t lineaddr, Addr_t PC, UINT32 accessType ) {
    int hit[2] = {-1, -1};
    UINT32 selectorIndex = SelectorIsSample(setIndex);

    // get partial tag
    Addr_t tag = PartialTag(lineaddr, selector_tag_mask);

    // update selector for each policy
    for (int p = 0; p < 2; p++) {
        // get set and line replacement state
        SET_REPLACEMENT_STATE *myset = &selector_set[p][selectorIndex];
        LINE_REPLACEMENT_STATE *myreplSet = selector_repl[p][selectorIndex];

        hit[p] = SelectorHit(myreplSet, tag);

        int updateWayID = hit[p];
        bool vb = false;
        bool bypass = false;

        // handle a miss
        if (hit[p] == -1) {
            int vic = -1;
            for (UINT32 w = 0; w < assoc; w++)
                if (!myreplSet[w].my.selector_valid) {
                    vic = w;
                    break;
                }
            if (vic == -1) {
                vic = GetVictim(myreplSet);
            }

            int oldvic = vic;

            bypass = UpdateLRUandRefOnMiss(myset, myreplSet, vic, &vb);

            if (bypass) vic = -1;
            else if (policy[myset->pid].aging)
                HandleAging( myreplSet, vic );

            if (policy[myset->pid].enable_bypass) {
                Addr_t viclineaddr = 0;
                if (vic != -1)
                    viclineaddr = GetLineAddr(setIndex, myreplSet[vic].my.selector_tag);
                UpdateBypass(myset, myreplSet, lineaddr, bypass, vb, assoc, 
                        oldvic, false, viclineaddr, PC, accessType);
            }

            if (!bypass) {
                myreplSet[vic].my.selector_tag = tag;
                myreplSet[vic].my.selector_valid = true;
                updateWayID = vic;
            }
        }
        assert((updateWayID >= 0 && updateWayID < (INT32)assoc) || bypass);

        // update replacement state and track bypassing on cache hit
        if (hit[p] != -1) {
            UpdateLRUandRefOnHit(&myreplSet[updateWayID].my);
            if (policy[myset->pid].enable_bypass) {
                UpdateBypass(myset, myreplSet, lineaddr, false, false, assoc, updateWayID, true, 0, PC, accessType);
            }
        }
    }

    // only update selection counter on demand accesses
    if (accessType<=ACCESS_STORE)
        SelectorCtUpdate(hit);
}

// update policy selection counter
void  CACHE_REPLACEMENT_STATE::SelectorCtUpdate(int *hit) {
    int max = MAX_SELECTION_COUNTER;
    if ((hit[0] != -1) && (hit[1] == -1)) {
        // policy0 hit, policy1 miss, decrease selection counter
        CounterUpdate(&selector_global_ct, max, false);
    }else if ((hit[1] != -1) && (hit[0] == -1)) {
        // policy1 hit, policy0 miss, increase selection counter
        CounterUpdate(&selector_global_ct, max, true);
    }
}

// update a saturated counter
void CACHE_REPLACEMENT_STATE::CounterUpdate(int *ct, int max, bool inc) {
    if (inc && (*ct) < max)
        (*ct) ++;
    else if (!inc && (*ct) > (-max - 1))
        (*ct) --;
}

// check whether the set is sampled in selector. if yes, return the index to selector
// the method is borrowed from Qureshi's paper "Adaptive Insertion Policies for High Performance Caching"
int   CACHE_REPLACEMENT_STATE::SelectorIsSample(UINT32 setIndex) {
    if (((setIndex >> selector_offset_bits) & selector_offset_mask) == (setIndex & selector_offset_mask)) {
        int index = (setIndex >> selector_offset_bits);
        assert (index < (int)(numsets / NUMSAMPLE));
        return index;
    }else
        return -1;
}

// LFSR based random generator.
bool CACHE_REPLACEMENT_STATE::RandMod(UINT32 divisor) {
  if (divisor == 0) return 0;

  LFSR = (LFSR >> 1) ^ (UINT32)((0u - (LFSR & 1u)) & 0xd0000001u); 

  // The random number is folded first. Then lower bits are used to get a probability based on divisor.
  UINT32 R = ((LFSR >> 16) ^ LFSR) & ((1u << 16u) - 1u);

  return ((R % divisor ) == 0);
}

