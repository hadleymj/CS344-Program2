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
	// 2). Ends execution on CPU 
	// 3). Ends printing 

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
		event = ev;
	}
	int time;
	int reqID;
	string event;
};

class timeCompareGreater // Create a function class to order the priority queue
{
	public:
	bool operator()(const future_event & fe1, const future_event & fe2)
	{
		return(fe1.time > fe2.time);
	}
};

/*void pushPop(const queue<request> & server, const future_event & fe, const request & req, const priority_queue<future_event, vector<future_event>, timeCompareGreater> & futureEvents)
{
	server.push(req);
	fe.event = "Ends execution on CPU#2"; // Need to check fe.event here, the only place

	if(cpu2.size() == 0)
	{
		// Update the time
		fe.time = req.time + req.cpuTime;
		req.time = fe.time;
	}
	else
	{
					
		request nextReq = cpu2.back(); // Store the element that will be in front of the one we are pushing on
		// Obtain the new time from the next item's end of execution time
		fe.time = req.cpuTime + nextReq.time; // nextReq.time should be when nextReq Ends Execution
		req.time = fe.time;
	}

	futureEvents.pop();
	futureEvents.push(fe);
}
*/	
int main()
{
	// Initialization:
	// 1). Read in from file
	// 2). Put requests in vector
	// 3). Put arrive events in PQ

	// Maybe put this code in a separate init function

	int holder[3];
	int reqID = 1;
	ifstream inFile;
	inFile.open("p2_data.txt");
	vector<request> requestHolder;
	if(!inFile)
	{
		cout << "Could not open file." << endl;
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

	int test = (requestHolder.back()).time;
	cout <<  test << endl; // Should be 38

	priority_queue<future_event, vector<future_event>, timeCompareGreater> futureEvents;
	for(int i = 0; i < requestHolder.size(); i++) // Form the initial priority queue from requests that are going to arrive
	{
		futureEvents.push(*(new future_event((requestHolder[i]).time, i+1, "Arrives"))); 
	
	}
	
	queue<request> cpu1, cpu2, printer;

	while(!futureEvents.empty())
	{
		ofstream logFile;
		logFile.open("logfile.txt");
		
		
		future_event fe = futureEvents.top();

		// Find the corresponding reqeust 
		request req = requestHolder[fe.reqID - 1];
		
		//
		// First, push the future events onto the appropriate queues
		//

		// Check the event first, because this will tell us what queues to check

		if(fe.event == "Arrives")
		{
			//If CPU 1 has less requests in the queue, push it onto the queue
			// Put these into a templated function? Don't repeat yourself?

			logFile << req.time << "		" << req.reqID << "		" << fe.event << endl;

			if(cpu1.size() <= cpu2.size())
			{
				cpu1.push(req);
				fe.event = "Ends execution on CPU#1";

				if(cpu1.size() == 0)
				{
					// Update the times
					fe.time = req.time + req.cpuTime;
					req.time = fe.time;
				}
				else
				{
					request nextReq = cpu1.back(); // Store the element that will be in front of the one we are pushing on
					// Obtain the new time from the next item's end of execution time
					fe.time = req.cpuTime + nextReq.time;  // nextReq.time should be when nextReq Ends Execution
					req.time = fe.time; 
				}

				futureEvents.pop();
				futureEvents.push(fe);
			}
			else
			{
				cpu2.push(req);
				fe.event = "Ends execution on CPU#2";

				if(cpu2.size() == 0)
				{
					// Update the time
					fe.time = req.time + req.cpuTime;
					req.time = fe.time;
				}
				else
				{
					
					request nextReq = cpu2.back(); // Store the element that will be in front of the one we are pushing on
					// Obtain the new time from the next item's end of execution time
					fe.time = req.cpuTime + nextReq.time; // nextReq.time should be when nextReq Ends Execution
					req.time = fe.time;
				}

				futureEvents.pop();
				futureEvents.push(fe);
			}

		}
		
		if(fe.event == "Ends execution on CPU#1" || fe.event == "Ends execution on CPU#2")
		{
				logFile << req.time << "		" << req.reqID << "		" << fe.event << endl;
				
				// Pop the request off of the queue 
				if(fe.event == "Ends execution on CPU#1")
					cpu1.pop();
				else
					cpu2.pop();

				printer.push(req);
				fe.event = "Ends Printing";

				if(printer.size() == 0)
				{
					// Update the time
					fe.time = req.time + req.printTime;
					req.time = fe.time;
				}
				else
				{
					request nextReq = printer.back(); // Store the element that will be in front of the one we are pushing on
					// Obtain the new time from the next item's end of execution time
					fe.time = req.printTime + nextReq.time;  // nextReq.time should be when nextReq Ends Execution
					req.time = fe.time;
				}

				futureEvents.pop();
				futureEvents.push(fe);	
		}

		if(fe.event == "Ends Printing")
		{
				//Write to the log
				
				logFile << req.time << "		" << req.reqID << "		" << fe.event << endl;

				// Clean Up, the request has been satisfied
				printer.pop();
				requestHolder.erase(requestHolder.begin() + req.reqID -1);
				futureEvents.pop();
				
		}

		//
		// Now, check all the queues and write to the log file
		//
		//request cpu1Req, cpu2Req, printerReq;
		//cpu1Req = cpu1.front();
		//cpu2Req = cpu2.front();
		//printerReq = printer.front();
		//logFile << (cpu1Req.time - cpu1Req.cpuTime)	 << "		" << 
		// Include begin/end printing in priority queue so that the log file is displayed on the correct timeline?
		
	}	
}
