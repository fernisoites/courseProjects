#include "replacement_state.h"


#define SAT_INC(x,max)    (x<max)? x+1:max
#define SAT_DEC(x,min)    (x>min)? x-1:min

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

	repl[ setIndex ]->flag=-1;	//flag= -1 normal sets
        for(UINT32 way=0; way<assoc; way++) 
        {
            // initialize stack position (for true LRU)
            repl[ setIndex ][ way ].LRUstackposition = way;
        }
    }

    // Contestants:  ADD INITIALIZATION FOR YOUR HARDWARE HERE
    
    num_group = BASE_GROUP_NUM;
    threshold = BASE_SET_THRESHOLD;
    last_follow=0;
    global_follow=0;
    active_group = (UINT32*) calloc (numsets, sizeof(UINT32)); // 
    set_miss = (UINT32*) calloc (numsets, sizeof(UINT32)); //
    miss_counter_group = (UINT32*) calloc (9, sizeof(UINT32)); //


    
    for(UINT32 i=0;i<9;i++)
    {
	miss_counter_group[i] = 0;
    }

    //initialize active_group,set_miss
    for(UINT32 i=0; i<numsets; i++)
    {
	active_group[i] = 0;
	set_miss[i]= 0;
    }                          


    //choose dedicated sets for ASRP here ...
    
    UINT32 region = numsets/(asrp_dedicated_sets * 8);
    for(UINT32 ii=0;ii<asrp_dedicated_sets;ii++)
    {
	repl[ii*region*8]->flag = 1;
	repl[ii*region*8+region]->flag = 2;
	repl[ii*region*8+region*2]->flag = 3;
	repl[ii*region*8+region*3]->flag = 4;
	repl[ii*region*8+region*4]->flag = 5;
	repl[ii*region*8+region*5]->flag = 6;
	repl[ii*region*8+region*6]->flag = 7;
	repl[ii*region*8+region*7]->flag = 8;
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
	return Get_ASRP_Victim( setIndex ); 		//get asrp victim
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

	if(cacheHit)
	{
	    UpdateLRU( setIndex, updateWayID );
	}
	else 
	{
	    switch (repl[setIndex]->flag)
	    {
		case 1: miss_counter_group[1]++; threshold = THRESHOLD_SAMPLE_1; break;	//sample sets use the fixed threshold
		case 2: miss_counter_group[2]++; threshold = THRESHOLD_SAMPLE_2; break;
		case 3: miss_counter_group[3]++; threshold = THRESHOLD_SAMPLE_3; break;
		case 4: miss_counter_group[4]++; threshold = THRESHOLD_SAMPLE_4; break;
		case 5: miss_counter_group[5]++; threshold = THRESHOLD_SAMPLE_5; break;
		case 6: miss_counter_group[6]++; threshold = THRESHOLD_SAMPLE_6; break;
		case 7: miss_counter_group[7]++; threshold = THRESHOLD_SAMPLE_7; break;
		case 8: miss_counter_group[8]++; threshold = THRESHOLD_SAMPLE_8; break;
		default:
			{
			    UINT32 count, low=1;
			    for(count=2;count<9;count++)
			    {
				low = (miss_counter_group[low]<=miss_counter_group[count]) ? low : count;
			    }
			    switch (low) 
			    {
		 		case 1: threshold = THRESHOLD_SAMPLE_1;break;
				case 2: threshold = THRESHOLD_SAMPLE_2;break;
				case 3: threshold = THRESHOLD_SAMPLE_3;break;
				case 4: threshold = THRESHOLD_SAMPLE_4;break;
				case 5: threshold = THRESHOLD_SAMPLE_5;break;
				case 6: threshold = THRESHOLD_SAMPLE_6;break;
				case 7: threshold = THRESHOLD_SAMPLE_7;break;
				case 8: threshold = THRESHOLD_SAMPLE_8;break;
			    }
			    if(low==last_follow)
			    {
				global_follow++;
				if(global_follow > SMOOTH_THRESHOLD)
				{
				    for(count=1;count<9;count++)
				    {
					miss_counter_group[count] = 0;
				    }
			 	    global_follow=0;
		 		}
			    }
			    else 
			    {
				global_follow=SAT_DEC ( global_follow ,0 );
				last_follow = low; 
			    }
			}
	    }
	    set_miss[setIndex]++; 
	    if(set_miss[setIndex]>threshold)	//when the set's miss exceeds the threshold the active subset will 
	    {					//change to adjacent one
		set_miss[setIndex] = 0;
		active_group[setIndex]++;
		active_group[setIndex] = (active_group[setIndex]%num_group);
	    }
	}
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


INT32 CACHE_REPLACEMENT_STATE::Get_ASRP_Victim( UINT32 setIndex )
{
    // Get pointer to replacement state of current set
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];
    INT32   AsrpWay   = active_group[setIndex] * (assoc / num_group);
    UINT32 way=active_group[setIndex] * (assoc / num_group);		//find the begin of the active subset
    UINT32 end=way+assoc / num_group;					//find the end of the active subset
    for(;way<end;way++)
    {
	if(replSet[way].LRUstackposition>replSet[AsrpWay].LRUstackposition)
	{
	    AsrpWay=way;
	}
    }
    return AsrpWay;

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

