#include "project3.h"

// Finds the min cost given an array of DV's
int find_min_cost(int dvs[])
{
    int min_cost = INFINITY;

    for(int i = 0; i < MAX_NODES; i++)
        if(dvs[i] < min_cost)
            min_cost = dvs[i];

    return min_cost;
}