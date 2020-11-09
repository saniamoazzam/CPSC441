/*
Name: Sania Moazzam
Tutorial: 01
UCID: 30018572
File: routing.cpp

CPSC 441 Assignment 3

This assignment is a discrete-event network simulation problem. 
Think of a loop that models the arrival and/or departure of each call over time,
and using this loop to track the usage of each network link. Calls arrive at 
certain times, based on the input file, and depart at certain times, based on 
their durations. Specifically, your program should be able to compute the call
routing performance metrics for different routing algorithms, when provided a 
specified call workload file (i.e., traffic load for the network) and a specified
network topology.
*/

#include <stdio.h>
#include <iostream>
#include <limits.h> 
#include <string>
#include <algorithm>
#include <math.h> 

#define MAX_ROW 10000
#define MAX_COL 10000
#define MAX_EVENTS 10000
#define V 23

using namespace std;

// Declare functions
void event_alg(int n); 
string SHPF(char s, char d);
string SDPF(char s, char d);
string LLP(char s, char d);
string MFC(char s, char d);
void printResult(struct r);

void increaseLoadOnRoute(string r);
void decreaseLoadOnRoute(string r);
int getHops(string r);
int getPropDelay(string r);
int minDistance(int d[], bool s[]);
int minDistance(float d[], bool s[]);
int maxCircuits(int d[], bool s[]);
string getRoute(int p[], int d);

// Global variables
int propdelay[MAX_ROW][MAX_COL];
int capacity[V][V];
int available[MAX_ROW][MAX_COL];
float allcost[MAX_ROW][MAX_COL];
int totalCalls = 0;

// All start events are stored in this
struct StartEvent
{
    float start_time;
    int callid;
    char source;
    char destination;
    float duration;
  } StartEventList[MAX_EVENTS];

// All end events are stored in this
struct EndEvent
{
  float end_time;
  int callid;
  string route;
  } EndEventList[MAX_EVENTS];

// The final result is stored here
struct Result
{
  int calls;
  int SHPFsuccess;
  int SHPFblocked;
  float SHPFhops;
  float SHPFdelay;
  int SDPFsuccess;
  int SDPFblocked;
  float SDPFhops;
  float SDPFdelay;
  int LLPsuccess;
  int LLPblocked;
  float LLPhops;
  float LLPdelay;
  int MFCsuccess;
  int MFCblocked;
  float MFChops;
  float MFCdelay;
} Result;


void printResult() {
  float tmp = 0.00;
  cout << "\nPolicy\tCalls\tSucc\tBlock(%)\tAvg Hops  Avg Delay\n";
  cout << "------------------------------------------------------\n";

  tmp = (float)Result.SHPFblocked/(float)Result.calls;
  cout << "SHPF\t" << Result.calls << "\t" << Result.SHPFsuccess << 
  "\t" << Result.SHPFblocked << " (" <<  tmp*100 << "%)\t" << 
  Result.SHPFhops/Result.SHPFsuccess  << "\t  " << 
  Result.SHPFdelay/Result.SHPFsuccess << endl;

  tmp = (float)Result.SDPFblocked/(float)Result.calls;
  cout << "SDPF\t" << Result.calls << "\t" << Result.SDPFsuccess << 
  "\t" << Result.SDPFblocked << " (" <<  tmp*100 << "%)\t" << 
  Result.SDPFhops/Result.SDPFsuccess<< "\t  " << Result.SDPFdelay/Result.SDPFsuccess << endl;

  tmp = (float)Result.LLPblocked/(float)Result.calls;
  cout << "LLP\t" << Result.calls << "\t" << Result.LLPsuccess << 
  "\t" << Result.LLPblocked << " (" <<  tmp*100 << "%)\t" << 
  Result.LLPhops/Result.LLPsuccess<< "\t  " << Result.LLPdelay/Result.LLPsuccess << endl;

  tmp = (float)Result.MFCblocked/(float)Result.calls;
  cout << "MFC\t" << Result.calls << "\t" << Result.MFCsuccess << 
  "\t" << Result.MFCblocked << " (" <<  tmp*100 << "%)\t" << Result.MFChops/Result.MFCsuccess
  << "\t  " << Result.MFCdelay/Result.MFCsuccess << endl;

  cout << endl;
}


void increaseLoadOnRoute(string route) {
  for (int i = 0; i < route.length()-1; i++) {
    int f = route.at(i+1) - 'A';
    int s = route.at(i) - 'A';

    available[f][s] = available[f][s]-1;
    available[s][f] = available[s][f]-1;
  }
}


void decreaseLoadOnRoute(string route) {
  for (int i = 0; i < route.length()-1; i++) {
    int f = route.at(i+1) - 'A';
    int s = route.at(i) - 'A';

    available[f][s] = available[f][s]+1; available[s][f] = available[s][f]+1;
  }
}


int getHops(string route) {
  int len = route.length();
  return len-1;
}


int getPropDelay(string route) {
  int delay = 0;
  for (int i = 0; i < route.length()-1; i++) {
    int f = route.at(i+1) - 'A';
    int s = route.at(i) - 'A';

    delay += propdelay[f][s];
  }
  return delay;
}


int minDistance(int dist[], bool set[]) { 
  // Initialize min value 
  int min = INT_MAX, min_index; 
  
  for (int i = 0; i < V; i++) {
    if (!set[i] && dist[i] <= min) {
      min = dist[i], min_index = i;}
  }
  return min_index; 
} 


int minDistance(float dist[], bool set[]) { 
  // Initialize min value 
  float min = INT_MAX, min_index; 
  
  for (int i = 0; i < V; i++) {
    if (!set[i] && dist[i] <= min) {
      min = dist[i], min_index = i;}
  }
  return min_index; 
} 


int maxCircuits(int circ[], bool set[]) { 
  // Initialize min value 
  int max = -1, min_index; 
  
  for (int i = 0; i < V; i++) {
    if (!set[i] && circ[i] != -1 && circ[i] >= max) 
      max = circ[i], min_index = i;
  }
  return min_index; 
} 


string getRoute(int prev[], int dst){
  string path = "";
  int crawl = dst;
  while (prev[crawl] != -1) { 
    path += prev[crawl] + 'A';
    crawl = prev[crawl]; 
  }

  if (path != "") {
    char tmp = dst + 'A'; 
    path = tmp + path;
  }

  return path;
}


string MFC(char src, char dst) {
  bool visited[V];
  int prev[V];
  int free[V];

  // Initially all nodes are unvisited and all max free ciruits are -1
  for (int i = 0; i < V; i++) {
    free[i] = -1;
    visited[i] = false; 
    prev[i] = -1;
  }

  // Convert source and destination to ints
  int source = src - 'A';
  int dest = dst - 'A';

  // Free circuit to itself is 0
  free[source] = 0;

  // Iterate until destination is reached
  for (int count = 0; count < V - 1; count++) { 

    int u = maxCircuits(free, visited);

    // Mark picked vertex as processed
    visited[u] = true;

    for (int v = 0; v < V; v++)  {

      // If u == source, then free circuits are just whats currently available
      // Iterating further, free circuits for a node will be min of available 
      // circuit or previous node's circuit. 
      if (!visited[v] && available[u][v] && free[u] != -1  
        && available[u][v] > free[v]) {
          prev[v] = u;
          
          if (u == source) 
            free[v] = available[u][v]; 
          else
            free[v] = min(available[u][v], free[u]);
      }
    }

  } // While loop

  string path = getRoute(prev, dest);  
  return path;
}


string LLP(char src, char dst) {
  bool visited[V];
  int prev[V];
  float cost[V];

  // Initially all nodes are unvisited and cost to each node is Infinity
  for (int i = 0; i < V; i++) {
    cost[i] = INT_MAX;
    visited[i] = false; 
    prev[i] = -1;
  }

  // Convert source and destination to ints
  int source = src - 'A';
  int dest = dst - 'A';

  // Distance to itself is always 0
  cost[source] = 0;

  float active;
  // Initialize allcost matrix 
  for (int r = 0; r < V; r++) {
    for (int c = 0; c < V; c++) {
      // Capcity[r][c] will be 0 if no link exists
      // Prevents from diving by 0
      if (capacity[r][c]) {
        active = capacity[r][c] - available[r][c];
        allcost[r][c] = active/capacity[r][c];
        allcost[c][r] = allcost[r][c];
        }
    }
  }

  // Iterate until destination is reached
  for (int count = 0; count < V - 1; count++) { 

    int u = minDistance(cost, visited);

    // Mark picked vertex as processed
    visited[u] = true;

    for (int v = 0; v < V; v++)  {

      // Update cost[v] only if not visited, there is an edge from 
      // min_index to v, and total utilization cost of path from src to 
      // v through u is smaller than current value of cost[v] 
      if (!visited[v] && available[u][v] && cost[u] != INT_MAX  
        && cost[u] + allcost[u][v]  < cost[v]) {

          prev[v] = u;
          cost[v] = cost[u] + allcost[u][v];
      }
    }

    // If dest hasn't been reached, cost will be INT_MAX
    // If dest is reached and cost is 0, that is the lowest it can be
    if (cost[dest] == 0) 
      break; 
  } // While loop

  string path = getRoute(prev, dest);  
  return path;
}


string SDPF(char src, char dst) {
  int delay[V], prev[V];
  bool visited[V];

  // Initially all nodes are unvisited and delay to each node is Infinity
  for (int i = 0; i < V; i++) {
    delay[i] = INT_MAX;
    visited[i] = false; 
    prev[i] = -1;
  }

  // Convert source and destination to ints
  int source = src - 'A';
  int dest = dst - 'A';

  // Distance of source vertex from itself is 0
  delay[source] = 0;

  // Find least delay path until destination
  for (int count = 0; count < V - 1; count++) { 
      int u = minDistance(delay, visited); 
  
      // Mark the picked vertex as processed 
      visited[u] = true; 
  
      for (int v = 0; v < V; v++) {
  
        // Update delay[v] only if not visited, there is an edge from 
        // min_index to v, and total delay of path from src to v through 
        // u is smaller than current value of delay[v] 
        if (!visited[v] && available[u][v] && delay[u] != INT_MAX 
          && delay[u] + propdelay[u][v] < delay[v]) {

            delay[v] = delay[u] + propdelay[u][v];
            prev[v] = u;
        }
      }
    } // For end

  string path = getRoute(prev, dest);  
  return path;
} // Function SDPF end


string SHPF(char src, char dst) {
  bool visited[V];
  int prev[V], dist[V], empty[V];

  // Initially all nodes are unvisited and distance to each node is Infinity
  for (int i = 0; i < V; i++) {
    dist[i] = INT_MAX;
    empty[i] = INT_MAX;
    visited[i] = false; 
    prev[i] = -1;
  }

  // Convert source and destination to ints
  int source = src - 'A';
  int dest = dst - 'A';

  // Distance of source vertex from itself is 0
  dist[source] = 0;
  empty[source] = 0;

  // Iterate until destination is reached
  while (prev[dest] == -1) { 

    int u = minDistance(dist, visited);

    // Mark picked vertex as processed
    visited[u] = true;

    for (int v = 0; v < V; v++)  {

      // Update dist[v] only if not visited, there is an edge from 
      // min_index to v, and total weight of path from src to v through u is 
      // smaller than current value of dist[v] 
      if (!visited[v] && available[u][v] && dist[u] != INT_MAX  
        && dist[u] + 1 < dist[v]) {

          prev[v] = u;
          dist[v] = dist[u] + 1;
      }
    }
    if (u == dest) 
      break;
  } // When dest has been reached

  // Only calculate route if hops in empty are equal or less than current graph
  string path = getRoute(prev, dest);
  return path;
} // Function SHPF end


void event_alg(int num) {
    int csuccess = 0, chops = 0, cdelay = 0, cblocked = 0;
    float start, end;
    int si = 0, ei = 0, id, tmp;
    char src, dst;
    string route;

    // Deal with events
    for (int i = 0; i < totalCalls; i++) {
      start = StartEventList[si].start_time;
      end = EndEventList[ei].end_time;
    
      // Start a call
      if (start < end) {
        src = StartEventList[si].source;
        dst = StartEventList[si].destination;

        // Determine which algorithm to execute
        if (num == 1)
          route = SHPF(src, dst);
        else if (num == 2) 
          route = SDPF(src, dst);
        else if (num == 3)
          route = LLP(src, dst);
        else if (num == 4)
          route = MFC(src, dst);

        if (route != "") {
          //cout << "Sucess\n";
          increaseLoadOnRoute(route);
          csuccess++;
          chops += getHops(route);
          cdelay += getPropDelay(route);

          // Add route to struct to be able to decrease load when it ends
          // Since EndEventList is sorted, first find correct event 
          id = StartEventList[si].callid;
          for (int i = 0; i < totalCalls; i++) {
            tmp = EndEventList[i].callid;
            if (tmp == id) {
              EndEventList[i].route = route; 
              // Break as we have set the route
              break;}
          }
        }
        // If empty route, block call
        else 
          cblocked++;
        
        si++;
      }
      // End a call
      else {
        // r will be empty if the call was blocked
        string r = EndEventList[ei].route;
        if (r != "") 
          decreaseLoadOnRoute(r); 
        ei++;
        i--;
      }
    } // Iterate over all events

    // Determine which algorithm was executed
    if (num == 1) {
        Result.SHPFsuccess = csuccess;
        Result.SHPFblocked = cblocked;
        Result.SHPFhops = chops;
        Result.SHPFdelay = cdelay;
    }
    else if (num == 2) {
        Result.SDPFsuccess = csuccess;
        Result.SDPFblocked = cblocked;
        Result.SDPFhops = chops;
        Result.SDPFdelay = cdelay;
    }
    else if (num == 3) {
        Result.LLPsuccess = csuccess;
        Result.LLPblocked = cblocked;
        Result.LLPhops = chops;
        Result.LLPdelay = cdelay;
    }
    else if (num == 4) {
        Result.MFCsuccess = csuccess;
        Result.MFCblocked = cblocked;
        Result.MFChops = chops;
        Result.MFCdelay = cdelay;
    }

    // Re-initialize correct values of available and cost matrix
    for (int r = 0; r < MAX_ROW; r++) {
      for (int c = 0; c < MAX_COL; c++) {
        available[r][c] = capacity[r][c]; 
        available[c][r] = capacity[c][r]; 
        allcost[r][c] = 0; 
        allcost[c][r] = 0;

      }
    }
    // Re-initialize end event routes to empty string
    for (int i = 0; i < MAX_EVENTS; i++) 
       EndEventList[i].route = "";
} // Function event algorithm


bool sortEvent(EndEvent e1, EndEvent e2) {
    return (e1.end_time < e2.end_time);
}

int main() {
    int delay, cap;
    char src, dst;
    float time, dur;
    int callSuccess = 0, callBlocked = 0, 
        totalHops = 0, totalDelay = 0;
    FILE *file;

    // Initialize topology
    file = fopen("topology.dat","r");
    while(fscanf(file, "%c %c %d %d\n", &src, &dst, &delay, &cap) == 4) 
    { 
	    int row = src - 'A'; int col = dst - 'A';

	    propdelay[row][col] = delay; propdelay[col][row] = delay;
	    capacity[row][col] = cap; capacity[col][row] = cap;
	    available[row][col] = cap; available[col][row] = cap;
      allcost[row][col] = 0; allcost[col][row] = 0;

    } 
    fclose(file);

    // Initialize call work load
    file = fopen("callworkload.dat","r");
    while(fscanf(file, "%f %c %c %f\n", &time, &src, &dst, &dur) == 4) 
    {
      StartEventList[totalCalls] = {time, totalCalls, src, dst, dur};
      EndEventList[totalCalls] = {time+dur, totalCalls, ""};
      totalCalls++;
    }
    fclose(file);

    // Sort EndEventList based on end times
    sort(EndEventList, EndEventList+totalCalls, sortEvent);
    // Add total calls to result
    Result.calls = totalCalls;

    // Iterate over calls for each algorithm
    event_alg(1); 
    event_alg(2); 
    event_alg(3);
    event_alg(4);

    printResult();
    return 0; 

} // End main
