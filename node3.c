#include <stdio.h>
#include <string.h>
#include "project3.h"

#define NODE3 3

extern int TraceLevel;
extern float clocktime;
extern int ConnectCosts[MAX_NODES][MAX_NODES];

struct distance_table dt3;
struct NeighborCosts *neighbor3;

int cached_min_costs3[MAX_NODES];

void send_pkt3();
void check_update_pkt3();
void cache_min_costs3();

void printdt3(int, struct NeighborCosts*, struct distance_table*);

/* students to write the following two routines, and maybe some others */

void rtinit3()
{
    printf("At time %0.3f, rtinit3() called.\n", clocktime);

    for(int i = 0; i < MAX_NODES; i++)
    {
        for(int j = 0; j < MAX_NODES; j++)
        {
            // If i and j are equal, the nodes are neighbors
            if(i == j)
            {
                dt3.costs[i][j] = ConnectCosts[NODE3][j];

                if(i == NODE3)
                {
                    cached_min_costs3[NODE3] = 0;
                }
                else
                {
                    cached_min_costs3[i] = dt3.costs[i][j];
                }
            }
            else
            {
                dt3.costs[i][j] = INFINITY;
            }
        }
    }

    printf("At time t=%.3f, node3 initial distance vector: %d  %d  %d  %d\n",
            clocktime, cached_min_costs3[0], cached_min_costs3[1],
            cached_min_costs3[2], cached_min_costs3[3]);

    send_pkt3();
}

void rtupdate3(struct RoutePacket *rcvd_pkt)
{
    int source = rcvd_pkt->sourceid;

    printf("At time t=%.3f, rtupdate3() called, by a pkt received from Sender id: %d\n",
            clocktime, source);

    for(int i = 0; i < MAX_NODES; i++)
    {
        // Calculate the min cost from the dt0 costs table and min cost from the received packet
        int min_cost = dt3.costs[source][source] + rcvd_pkt->mincost[i];

        dt3.costs[i][source] = min_cost;
    }

    check_update_pkt3();
}

void send_pkt3()
{
    struct RoutePacket packet;

    for(int i = 0; i < MAX_NODES; i++)
    {
        // Create a route packet
        packet.sourceid = NODE3;
        packet.destid = i;
        memcpy(packet.mincost, cached_min_costs3, sizeof(cached_min_costs3));

        // Send the route packet to every node except the calling node
        if(i != NODE3 && ConnectCosts[NODE3][i] < INFINITY)
        {
            printf("At time t=%.3f, node %d sends packet to node %d with: %d  %d  %d  %d\n",
                    clocktime, packet.sourceid, packet.destid, packet.mincost[0], packet.mincost[1],
                    packet.mincost[2], packet.mincost[3]);

            toLayer2(packet);
        }
    }
}

void check_update_pkt3()
{
    // Saves the previous cache for comparison
    int previous_cache[MAX_NODES];
    memcpy(previous_cache, cached_min_costs3, sizeof(cached_min_costs3));

    // Recalculate the cache
    cache_min_costs3();

    // Check for an update in the DV
    int update = 0;
    for(int i = 0; i < MAX_NODES; i++)
        if(previous_cache[i] != cached_min_costs3[i])
            update = 1;


    // Only send a new packet if there was a DV update
    if(update == 1)
    {
        printf("At time t=%.3f, node3 current distance vector: %d  %d  %d  %d\n",
                clocktime, cached_min_costs3[0], cached_min_costs3[1],
                cached_min_costs3[2], cached_min_costs3[3]);

        send_pkt3();
    }
}

// Generates the cache of min costs for use in sending packets
void cache_min_costs3()
{
    for(int i = 0; i < MAX_NODES; i++)
        cached_min_costs3[i] = find_min_cost(dt3.costs[i]);
}


/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt3(int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr)
{
    int i, j;
    int TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int NumberOfNeighbors = 0;                     // How many neighbors
    int Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for(i = 0; i < TotalNodes; i++)
    {
        if((neighbor->NodeCosts[i] != INFINITY) && i != MyNodeNumber)
        {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }

    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );

    for(i = 0; i < NumberOfNeighbors; i++)
        printf("     %d", Neighbors[i]);

    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for(i = 0; i < TotalNodes; i++)
    {
        if(i != MyNodeNumber)
        {
            printf("dest %d|", i );

            for(j = 0; j < NumberOfNeighbors; j++)
            {
                printf("  %4d", dtptr->costs[i][Neighbors[j]]);
            }

            printf("\n");
        }
    }

    printf("\n");
}    // End of printdt3

