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
#define INSPOLICY_FOLLOWER_SET 0
#define INSPOLICY_INS_SET 1
#define INSPOLICY_LRU_SET 2
#define INSPOLICY_ADAPTIVE_SET 3


#define SAT_INC(x,max)    (x<max)? x+1:max
#define SAT_DEC(x,min)    (x>min)? x-1:min


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
    
    if(replPolicy == CRC_REPL_CONTESTANT  )
    {

	// Initializing the parameters
	switched = 0;
	psel1 = psel2 = numsets/2;    	   
    	nearMRUPos = (assoc/2 -1 ) - assoc/4;
	nearLRUPos = (assoc/2 -1 ) + assoc/4;
        middlePos = assoc/2 -1 ;
	numDedicatedSet = numsets/64;

	
	// Initializing the setType
    	setType = new UINT32 [numsets];
    	for(UINT32 set=0; set<numsets; set++) 
    	{
		setType[set] = INSPOLICY_FOLLOWER_SET; 
    	}

	// Initilized in the same way as Dynamic Insertion Policy
	for(UINT32 i=0; i< numDedicatedSet; i++) 
    	{
	    	UINT32 region_size = numsets/numDedicatedSet; 
	    	UINT32 mru_offset = i%region_size;
	    	UINT32 lru_offset = (~mru_offset)%region_size;
	    	UINT32 mruins_set = i*region_size + mru_offset;
	    	UINT32 lruins_set = i*region_size + lru_offset;
	    	assert(mruins_set != lruins_set);
            	setType[mruins_set] = INSPOLICY_LRU_SET; // LRU_SET:  inserts at MRU
 	   	setType[lruins_set] = INSPOLICY_INS_SET; // INS_SET:  inserts at middle pos
		setType[lruins_set-1] = INSPOLICY_ADAPTIVE_SET; // ADAPTIVE_SET: set that chooses the insertion position
	    		
   	}
	
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
	// Victim selection is same as the LRU policy
        return Get_LRU_Victim( setIndex );
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
		// If this is a hit, move the line in MRU position, same as LRU policy	
		if(cacheHit)
		{
			UpdateLRU( setIndex, updateWayID );
		}
		// If this is a miss and a new line is getting inserted
		// we will insert the line depending on the setType
		// we will also update psel counters, if this is a leader set
		else
		{
			
			// code for each type set, though it takes more code, but easy to understand

			if( setType[setIndex] == INSPOLICY_INS_SET ) 
			{
				// inserts the line in the middlepos of the LRU stack
				InsertToPos( setIndex, updateWayID, middlePos );  
			}
			else if (setType[setIndex] == INSPOLICY_LRU_SET) 
			{
				 // LRU set, inserts in MRU position of the LRU stack
				UpdateLRU( setIndex, updateWayID );		 
			}
			else if(setType[setIndex] == INSPOLICY_ADAPTIVE_SET) 
			{
				// this set depending on winner in the first round, inserts either in nearMRUPos, nearLRUPos or LRUPos
				// psel1 >= numsets/2 means winner is middlePos in 1st round
				if( psel1 >= numsets/2) 
				{					
					// adaptive set selects LRU position to insert,
					// so now setduling is going on between middle position and LRU position (second round)
					// if middlePos wins here also, it means inserting at LRU is no good, but it might be beneficial 
					// to insert in nearLRU position, so adaptive set will switch to nearLRU position insertion
					// switched keep track if this nearLRU/LRU position switching							
					if(  switched == 0 )	
					{	
						// psel2 saturated means that middle position insertion is winning in the second round		
						// this is switching the policy in adaptive sets, now on they will insert in nearLRU position			
						if( psel2 == (numsets -1)) 
						{	
							switched = 1;	
							psel2 = numsets/2;								
							InsertToPos( setIndex, updateWayID, nearLRUPos );
						}
						// keep inserting in the LRU position
						else 
						{
							InsertToPos( setIndex, updateWayID, assoc-1 ); 
						}
					}
					else if( switched == 1)
					{
						// adaptive set has switched in insertion position, so insert in the nearLRU position
						InsertToPos( setIndex, updateWayID, nearLRUPos );
					}
				}
				// psel1 < numsets/2 means winner is MRUPos in 1st round
				else 
				{
					// first round winner is MRU insertion, so adaptive set decides to insert in nearMRU position
					// so second round in between MRU and nearMRU position
					InsertToPos( setIndex, updateWayID, nearMRUPos ); 
				}
							
			}					
			else if(setType[setIndex] == INSPOLICY_FOLLOWER_SET)
			{
				// follower sets follow the decision taken from the leader sets
				// possible cases are
				// case 1: psel1 < numsets/2, psel2 >= numsets/2 , final winner is MRU insertion
				// case 2: psel1 < numsets/2, psel2 < numsets/2 , final winner is nearMRU insertion
				// case 3: psel1 >= numsets/2, switched = 0, psel2 >= numsets/2 , final winner is middle position insertion
				// case 4: psel1 >= numsets/2, switched = 0, psel2 < numsets/2 , final winner is LRU position insertion
				// case 5: psel1 >= numsets/2, switched = 1, psel2 >= numsets/2 ,final winner is middle position insertion
				// case 6: psel1 >= numsets/2, switched = 1, psel2 < numsets/2 , final winner is nearLRU position insertion
				// here are the tree to visualize the winner
				/*
				     						psel1
										  |
						       ___________________________|____________________________
						       |  < numsets/2                           >= numsets/2   |
						       |                                                       |
						     psel2                                                 switched
						       |                                                       |
					     __________|__________                                    _________|________         
                               < numsets/2   |                    | >= numsets/2                    0 |                 |  1  
					     |                    |                                   |                 |
					nearMRU pos            MRU pos                              psel2             psel2
                                                                                          < numsets/2 | >= numsets/2    |
										 	       _______|_______    ______|_______
										               |              |	  | < numsets/2 | >= numsets/2 	
											       |	      |   |		|
											  LRU pos     middle pos  nearLRU    middle pos	
				*/
				
				if (psel1 < numsets/2) // MRU postion insertion is winner in first round
				{
					if(psel2 >= numsets/2)   // MRU postion insertion is winner in second round also
					{
						UpdateLRU( setIndex, updateWayID );
					}
					else		        //  nearMRU postion insertion is winner in second round 
					{
						InsertToPos( setIndex, updateWayID, nearMRUPos );
					}
				}
				else 				//  middlePos insetion is winner in first round
				{
							
					if( switched == 0)
					{
						if(psel2 >= numsets/2) // middle postion insertion is winner in second round also
						{
							InsertToPos( setIndex, updateWayID, middlePos );
									
						}
						else                  // LRU postion insertion is winner in second round
						{
							InsertToPos( setIndex, updateWayID, assoc-1 );
						}
					}
					else if(switched == 1)
					{
						if(psel2 >= numsets/2) // middle postion insertion is winner in second round also
						{
							InsertToPos( setIndex, updateWayID, middlePos );									
						}
						else                   // nearLRUPos insertion is winner in second round 
						{
							InsertToPos( setIndex, updateWayID, nearLRUPos );
						}
					}							
				}
			}
				
			// Now we have to update the psel counters is this set is a leader set
			if(setType[setIndex] == INSPOLICY_INS_SET) 
			{
				if(psel1 >= numsets/2) // middle postion insertion is winner in first round 
					psel2 = SAT_DEC(psel2, 0);
				psel1 = SAT_DEC(psel1, 0);
			}
			else if (setType[setIndex] == INSPOLICY_LRU_SET) 
			{
				if(psel1 < numsets/2) // MRU postion insertion is winner in first round 
					psel2 = SAT_DEC(psel2, 0);
				psel1= SAT_INC(psel1, numsets-1);
			}
			else if(setType[setIndex] == INSPOLICY_ADAPTIVE_SET ) 
			{
				psel2 = SAT_INC(psel2, numsets-1);
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
 
    return out;
        
}






void CACHE_REPLACEMENT_STATE::InsertToPos( UINT32 setIndex, INT32 updateWayID , UINT32 pos)
{

    
    // Determine current LRU stack position
    UINT32 currLRUstackposition = repl[ setIndex ][ updateWayID ].LRUstackposition;
    if(currLRUstackposition == pos)
	return;

    
    // current pos nearer to LRU
    if( currLRUstackposition > pos)
    {
    	for(UINT32 way=0; way<assoc; way++) 
   	{
       		if( repl[setIndex][way].LRUstackposition < currLRUstackposition && repl[setIndex][way].LRUstackposition >= pos ) 
        	{
            		repl[setIndex][way].LRUstackposition++;
        	}
    	}
	repl[ setIndex ][ updateWayID ].LRUstackposition = pos;
    }
    // current pos nearer to MRU
    if( currLRUstackposition < pos)
    {
    	for(UINT32 way=0; way<assoc; way++) 
   	{
       		if( repl[setIndex][way].LRUstackposition > currLRUstackposition && repl[setIndex][way].LRUstackposition <= pos ) 
        	{
            		repl[setIndex][way].LRUstackposition--;
        	}
    	}
	repl[ setIndex ][ updateWayID ].LRUstackposition = pos;
    }
    
}


