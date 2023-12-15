#include <stdio.h>
#include <string.h>
#include "project3.h"

#define NODE0 0

extern int TraceLevel;
extern float clocktime;
extern int ConnectCosts[MAX_NODES][MAX_NODES];

struct distance_table dt0;
struct NeighborCosts *neighbor0;

int cached_min_costs0[MAX_NODES];

void send_pkt0();
void check_update_pkt0();
void cache_min_costs0();

void printdt0(int, struct NeighborCosts*, struct distance_table*);

/* students to write the following two routines, and maybe some others */

void rtinit0()
{
    printf("At time %.3f, rtinit0() called.\n", clocktime);

    for(int i = 0; i < MAX_NODES; i++)
    {
        for(int j = 0; j < MAX_NODES; j++)
        {
            // If i and j are equal, the nodes are neighbors
            if(i == j)
            {
                dt0.costs[i][j] = ConnectCosts[NODE0][j];

                if(i == NODE0)
                {
                    cached_min_costs0[NODE0] = 0;
                }
                else
                {
                    cached_min_costs0[i] = dt0.costs[i][j];
                }
            }
            else
            {
                dt0.costs[i][j] = INFINITY;
            }
        }
    }

    printf("At time t=%.3f, node 0 initial distance vector: %d  %d  %d  %d\n",
            clocktime, cached_min_costs0[0], cached_min_costs0[1],
            cached_min_costs0[2], cached_min_costs0[3]);

    send_pkt0();
}

void rtupdate0(struct RoutePacket *rcvd_pkt)
{
    int source = rcvd_pkt->sourceid;

    printf("At time t=%.3f, rtupdate0() called, by a pkt received from Sender id: %d\n", clocktime, source);

    for(int i = 0; i < MAX_NODES; i++)
    {
        // Calculate the min cost from the dt0 costs table and min cost from the received packet
        int min_cost = dt0.costs[source][source] + rcvd_pkt->mincost[i];

        // Need to clamp to infinity because of above addition
        if(min_cost < INFINITY)
        {
            dt0.costs[i][source] = min_cost;
        }
        else
        {
            dt0.costs[i][source] = INFINITY;
        }
    }

    check_update_pkt0();
}

void send_pkt0()
{
    struct RoutePacket packet;

    for(int i = 0; i < MAX_NODES; i++)
    {
        // Create a route packet
        packet.sourceid = NODE0;
        packet.destid = i;
        memcpy(packet.mincost, cached_min_costs0, sizeof(cached_min_costs0));

        // Send the route packet to every node except the calling node
        if(i != NODE0 && ConnectCosts[NODE0][i] < INFINITY)
        {
            printf("At time t=%.3f, node %d sends packet to node %d with: %d  %d  %d  %d\n",
                    clocktime, packet.sourceid, packet.destid, packet.mincost[0], packet.mincost[1],
                    packet.mincost[2], packet.mincost[3]);

            toLayer2(packet);
        }
    }
}

void check_update_pkt0()
{
    // Saves the previous cache for comparison
    int previous_cache[MAX_NODES];
    memcpy(previous_cache, cached_min_costs0, sizeof(cached_min_costs0));

    // Recalculate the cache
    cache_min_costs0();

    // Check for an update in the DV
    int update = 0;
    for(int i = 0; i < MAX_NODES; i++)
        if(previous_cache[i] != cached_min_costs0[i])
            update = 1;

    // Only send a new packet if there was a DV update
    if(update == 1)
    {
        printf("At time t=%.3f, node0 current distance vector: %d  %d  %d  %d\n",
                clocktime, cached_min_costs0[0], cached_min_costs0[1],
                cached_min_costs0[2], cached_min_costs0[3]);

        send_pkt0();
    }
}

// Generates the cache of min costs for use in sending packets
void cache_min_costs0()
{
    for(int i = 0; i < MAX_NODES; i++)
        cached_min_costs0[i] = find_min_cost(dt0.costs[i]);
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
void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr)
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
}    // End of printdt0

