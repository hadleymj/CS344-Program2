// Mike Hadley
// CS 344 - Program 2

#include <iostream>
#include <queue>
#include <list>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct request
{
        request(int arrive, int cpu, int print, int id) // constructor
        {
                time = arrive;
                cpuTime = cpu;
                printTime = print;
                reqID = id;
        }
        int time; // The time at which the request's FUTURE event will occur

        // FUTURE event:
        // 1). Arrives
        // 2). Begins execution on CPU
	// 3). Ends execution on CPU
        // 4). Begins printing
	// 5). Ends printing

        int cpuTime;
        int printTime;
        int reqID;
};

struct future_event
{
        future_event(int t, int id, string ev)
        {
                time = t;
                reqID = id;
                eventName = ev;
        }
        int time; // Time that the future event will occur 
        int reqID;
        string eventName;
};

class timeCompareGreater // Create a function class to order the priority queue
{
        public:
        bool operator()(const future_event & fe1, const future_event & fe2)
        {
                if(fe1.time > fe2.time) // Order the priority queue by the time
			return true;
		else if(fe1.time == fe2.time)
			{
				return(fe1.eventName < fe2.eventName);
			}
		else
			return  false;
        }
};

int main()
{
        // Initialization:
        // 1). Read in from file
        // 2). Put requests in vector
        // 3). Put arrive events in PQ

        int holder[3]; // Hold the incoming data
        int reqID = 1;
        ifstream inFile;
        inFile.open("p2_data.txt");
	// Use a vector to hold the requests. 
        vector<request> requestHolder;
        if(!inFile)
        {
                cout << "Could not open inFile." << endl;
                int hold;
                cin >> hold;
                return 0;
        }
        while(inFile >> holder[0]) // Read in the arrival time
        {
                inFile >> holder[1]; // Read in the CPU time, Print time
                inFile >> holder[2];
                // Now push the request to the back of the vector
                requestHolder.push_back(*(new request(holder[0], holder[1], holder[2], reqID)));
                reqID++; // Increment request ID
        }
      
        inFile.close();
        
	// Use a priority queue to hold the future events. The priority queue will place the future_event  with
	// the greatest priority at the front of the queue. Using the timeCompareGreater function class,
	// this means that the future_event that will occur next will be at the front of the queue.

        priority_queue<future_event, vector<future_event>, timeCompareGreater> futureEvents;

        for(int i = 0; i < requestHolder.size(); i++) // Form the initial priority queue from requests that are going to arrive
        {
		// Push the future event onto the PQ
                futureEvents.push(*(new future_event((requestHolder[i]).time, i+1, "Arrives")));
        }
	
	// Set up the queues and the log file
	// The STL queues are a FIFO structure (First in, First out).
        queue<request> cpu1, cpu2, printer; 
        ofstream logFile;

	logFile.open("logfile.txt");
	 
	if(!logFile)
        {
            	cout << "Could not open file." << endl;
                int hold;
                cin >> hold;
                return 0;
        }
                               
	// Now loop until there are no more future events
	// No more future events means that all the requests have been satisfied

        while(!futureEvents.empty())
        {
		// Pick out the top element in the PQ 
                future_event fe = futureEvents.top();

                // Find the corresponding reqeust
                request req = requestHolder[fe.reqID - 1];
              
                //
                // First, push the future events onto the appropriate queues
                //

                // Check the event first, because this will tell us what queues to check

                if(fe.eventName == "Arrives")
                {
                       	// Write to the log file first

                        logFile << req.time << "                " << req.reqID << "             " << fe.eventName << endl;
			
			 // If CPU 1 has less requests in the queue, push it onto the queue

                        if(cpu1.size() <= cpu2.size())
                        {
                                fe.eventName = "Begins execution on CPU#1"; // The future event will be that it begins execution

                                if(cpu1.size() != 0)
                                {
                                        request nextReq = cpu1.back(); // Store the element that will be in front of the one we are pushing onrequestHolder.erase(requestHolder.begin() + req.reqID -1);
                                        // Obtain the new time from the next item's end of execution time
					if(cpu1.size() == 1) // If there is only one item in the queue, then it must be executing
						fe.time = nextReq.time; // nextReq.time gives you the time the next request is done executing
					else				
                                       		fe.time = nextReq.time + nextReq.cpuTime;  // fe.time is when the request BEGINS execution

                                        req.time = fe.time; // req.time is when the request BEGINS execution
                                }

                                cpu1.push(req); // Need to push it on the queue AFTER you modify fe and req

                                futureEvents.pop(); // Update the PQ
                                futureEvents.push(fe);
				requestHolder[fe.reqID -1] = req; // Update the vector 
                        }
                        else
                        {
                                fe.eventName = "Begins execution on CPU#2";

                                if(cpu2.size() != 0)
                                {
                                      
                                        request nextReq = cpu2.back(); // Store the element that will be in front of the one we are pushing on

                                       	if(cpu2.size() == 1) // If there is only one item in the queue, then it must be executing
						fe.time = nextReq.time; // nextReq.time gives you the time the next request is done executing
					else				
                                       		fe.time = nextReq.time + nextReq.cpuTime;  // fe.time is when the request BEGINS execution

                                        req.time = fe.time;
                                }

                                cpu2.push(req);

                                futureEvents.pop(); // Update the PQ 
                                futureEvents.push(fe);
				requestHolder[fe.reqID -1] = req; // Update the vector 
                        }

                }

                else if(fe.eventName == "Begins execution on CPU#1")
                {
                        logFile << req.time << "                " << req.reqID << "             " << fe.eventName << endl;

                        fe.eventName = "Ends execution on CPU#1";
                        fe.time = fe.time + req.cpuTime; // Ends execution at fe.time
                        req.time = fe.time;
                      
                        // pop and push the PQ
                        futureEvents.pop();
                        futureEvents.push(fe);
			
			cpu1.front() = req; // Update the queue
			requestHolder[fe.reqID -1] = req; // Update the vector 
                }
              
                else if(fe.eventName == "Begins execution on CPU#2")
                {
                        logFile << req.time << "                " << req.reqID << "             " << fe.eventName << endl;

                        fe.eventName = "Ends execution on CPU#2";
                        fe.time = fe.time + req.cpuTime; // Ends execution at fe.time
                        req.time = fe.time;
                      
                        // pop and push the PQ
                        futureEvents.pop();
                        futureEvents.push(fe);

			cpu2.front() = req; // Update the queue
			requestHolder[fe.reqID -1] = req; // Update the vector 
                }
              
                else if(fe.eventName == "Ends execution on CPU#1" || fe.eventName == "Ends execution on CPU#2")
                {
                                logFile << req.time << "                " << req.reqID << "             " << fe.eventName << endl;
                              
                                // Pop the request off of the queue
                                if(fe.eventName == "Ends execution on CPU#1")
                                        cpu1.pop();
                                else
                                        cpu2.pop();

                              	if(req.printTime != 0) // Only push onto the printer queue if the request needs to print
				{
                                	fe.eventName = "Begins Printing";

                               	        if(printer.size() != 0)
                             	        {
                                    	    request nextReq = printer.back(); // Store the element that will be in front of the one we are pushing on

                                    	    if(printer.size() == 1) // If there is only one item in the queue, then it must be executing
							fe.time = nextReq.time; // nextReq.time gives you the time the next request is done executing
				       	    else				
                                     	  		fe.time = nextReq.time + nextReq.printTime;  // fe.time is when the request BEGINS execution

                                       		 req.time = fe.time;
                               		 }
                              
                               			printer.push(req);

                                		futureEvents.pop(); // Update the PQ 
                               			futureEvents.push(fe);
						requestHolder[fe.reqID -1] = req; // Update the vector 
				 }
				 else
				 {
					// Request is satisfied -- clean up
					futureEvents.pop();
				 }
				
                }

                else if(fe.eventName == "Begins Printing")
                {
                        logFile << req.time << "                " << req.reqID << "             " << fe.eventName << endl;
                        fe.eventName = "Ends Printing";
                        fe.time = fe.time + req.printTime; // Ends execution at fe.time
                        req.time = fe.time;
                      
                        // pop and push the PQ
                        futureEvents.pop();
                        futureEvents.push(fe);
			
			printer.front() = req;
			requestHolder[fe.reqID -1] = req; // Update the vector 
                }

                else if(fe.eventName == "Ends Printing")
                {
                                //Write to the log
                              
                                logFile << req.time << "                " << req.reqID << "             " << fe.eventName << endl;

                                // Clean Up, the request has been satisfied
                                printer.pop();
                                futureEvents.pop();
                              
                }     
        }     
	requestHolder.clear(); // Clean up the vector (DO NOT erase contents of vector in loop, it will change the indices)
        logFile.clear();
        logFile.close();
      
}
