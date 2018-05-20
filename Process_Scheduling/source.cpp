#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct process//struct for the process table
{
    vector <string> type; //  stores types of requests
    vector <int> time; // times for each request
    int pID; // process id such as 0, 1, etc.
    int initialTime; // the initial time of the request
    int endTime; //the end time of the request
    string state; // the state after each event
};
// Compare the earliest request time for each process
struct earliestTime
{
    bool operator()(const process* x, const process* y) const
    {
        return x->endTime > y->endTime;
    }
};

int main()
{
    string storeType;
    int storeTime;
    int num_core;//number of cores that will be used
    int current_process = 0;
    vector <process> processes; // All of the process will be stored on here
    cin >> storeType >> num_core; // read the number of NCORES that will be used in this simulation
    while (!cin.eof())
    {
        cin >> storeType >> storeTime; // read the requests and the corresponding time
        if (storeType!= "NEW") // it will only read the requests after NEW and push back each new process into the vector
        {
            while(storeType!= "NEW" && (!cin.eof()))
            {
                processes[current_process].type.push_back(storeType);
                processes[current_process].time.push_back(storeTime);
                cin >> storeType >> storeTime;
            }
            current_process = current_process + 1;
            if (cin.eof())
            {
                continue;
            }
        }
        //
        process currentProcess;
        processes.push_back(currentProcess);
        processes[current_process].pID = current_process;
        processes[current_process].initialTime = storeTime;
        processes[current_process].endTime = storeTime;
    }
    queue<int> readyQueue;
    queue<int> inputQueue;
    queue<int> ssdQueue;
    priority_queue<process*, vector<process*>, earliestTime> priorityQueue;// a queue for prioritizing the earliest event
    vector<int> processElements;
    int clock = 0;
    int num_processes = processes.size();
    int core =1; // core that will be used at least once
    int ssd =0;
    int input =0;
    int nextProcessToArrive =1;
    int coreTime =0;
    int num_ssd =0;
    int ssdTime =0;
    // print the initial arrival time for process
    cout << "Process 0 starts at time " << processes[0].initialTime << " ms" << endl;
    clock = processes[0].initialTime;
    processes[0].endTime = clock + processes[0].time[0];
    process *temp = &processes[0];
    priorityQueue.push(temp);
    processElements.push_back(0);
    processes[0].state = "RUNNING";
    // while loop will keep running until every process has gone through the simulation
    while(!priorityQueue.empty() || (processElements.size() != 0))
    {
        if(num_processes >= nextProcessToArrive)
        {
            process *current = priorityQueue.top();
            if(current ->endTime > processes[nextProcessToArrive].initialTime)// if statement for the arrival of each process
            {
                clock = processes[nextProcessToArrive].initialTime;
                cout << "Process " << processes[nextProcessToArrive].pID << " starts at time " << processes[nextProcessToArrive].initialTime << " ms" << endl;
                for(int i = 0; i < processElements.size(); i++)
                {
                    int currentElement = processElements[i];
                    cout << "Process " << currentElement << " is " <<processes[currentElement].state << endl;//output the state of each process during this event
                }
                cout << endl << endl;
                if(core < num_core)// checks for available cores
                {
                    processes[nextProcessToArrive].state = "RUNNING";
                    processes[nextProcessToArrive].endTime = clock + processes[nextProcessToArrive].time[0];
                    process *temp = &processes[nextProcessToArrive];
                    priorityQueue.push(temp);
                    processElements.push_back(nextProcessToArrive);
                    core = core + 1;
                    nextProcessToArrive = nextProcessToArrive + 1;
                }
                else// if not then it has to wait in the ready queue
                {
                    processes[nextProcessToArrive].state = "READY";
                    readyQueue.push(nextProcessToArrive);
                    processElements.push_back(nextProcessToArrive);
                    nextProcessToArrive = nextProcessToArrive + 1;
                }
                continue;
            }
        }
        process *current = priorityQueue.top();
        int currentPId = current -> pID;// gets the position of the process ID
        if(current ->type[0] == "CORE")//if the following request is the CORE
        {
            coreTime = coreTime + current ->time[0];
            clock = current ->endTime;
            if (readyQueue.size() != 0)
            {
                int element = readyQueue.front();
                readyQueue.pop();
                processes[element].state = "RUNNING";//update the state at this event
                processes[element].endTime = clock + processes[element].time[0];//update the clock at this event
                process *temp = &processes[element];
                priorityQueue.push(temp);
            }
            else
            {
                core = core - 1;
            }
            priorityQueue.pop();
            // erasing the process element since the event is completed; NOTE: this will be done on SSD and INPUT as well.
            processes[currentPId].time.erase(processes[currentPId].time.begin() + 0);
            processes[currentPId].type.erase(processes[currentPId].type.begin() + 0);
            // if current process is complete then it will output the states
            if (processes[currentPId].type.size() == 0)
            {
                cout << "Process " << currentPId << " terminates at time " << clock << " ms" << endl;
                int eraseElement;//will be used to exclude the process that is being terminated when outputting
                for (int i = 0; i < processElements.size(); i++)
                {
                    int current_element = processElements[i];
                    if(current_element == currentPId)
                    {
                        eraseElement  = i;
                        continue;
                    }
                    cout << "Process " << current_element << " is " << processes[current_element].state << endl;//output the state of each process during this event
                }
                cout << "Process " << currentPId << " is TERMINATED" << endl;
                cout << endl << endl;
                processElements.erase(processElements.begin()+eraseElement);// erases the process since its terminated
                continue;
            }
            if(processes[currentPId].type[0] == "SSD")//if the following request is the SSD
            {
                if(ssd == 0)//SDD not being used
                {
                processes[currentPId].state = "BLOCKED";//update the state at this event
                processes[currentPId].endTime = clock + processes[currentPId].time[0];//update the state at this event
                    process *temp = &processes[currentPId];
                    priorityQueue.push(temp);
                    ssdTime = ssdTime - clock;
                    ssd = 1;
                }
                else//SSD being used
                {
                    ssdQueue.push(currentPId);
                    processes[currentPId].state = "BLOCKED";
                    ssdTime = ssdTime - clock;
                }
                continue;
            }
            if(processes[currentPId].type[0] == "INPUT")//if the following request is the INPUT
            {
                if(input == 0)//INPUT not being used
                {
                    processes[currentPId].state = "BLOCKED";//update the state at this event
                    processes[currentPId].endTime = clock + processes[currentPId].time[0];//update the state at this event
                    process *temp = &processes[currentPId];
                    priorityQueue.push(temp);
                    input = 1;
                }
                else//INPUT not being used
                {
                    inputQueue.push(currentPId);
                    processes[currentPId].state = "BLOCKED";
                }
                continue;
            }
        }
        if(current ->type[0] == "SSD")//if the following request is the SSD
        {
            clock = current ->endTime;
            ssdTime = ssdTime + clock;
            num_ssd = num_ssd + 1;
            if(ssdQueue.size() != 0)//check if the there any processes waiting
            {
                int element_num = ssdQueue.front();
                ssdQueue.pop();
                processes[element_num].state = "BLOCKED";//update the state at this event
                processes[element_num].endTime = clock + processes[element_num].time[0];//update the state at this event
                process *temp = &processes[element_num];
                priorityQueue.push(temp);
            }
            else
            {
                ssd = ssd - 1;
            }
            priorityQueue.pop();
            processes[currentPId].time.erase(processes[currentPId].time.begin());
            processes[currentPId].type.erase(processes[currentPId].type.begin());
            // if current process is complete then it will output the states
            if(processes[currentPId].type.size() == 0)
            {
                cout << "Process " << currentPId << " terminates at time " << clock << " ms" << endl;
                int eraseElement;//will be used to exclude the process that is being terminated when outputting
                for(int i = 0; i < processElements.size(); i++)
                {
                    int current_element = processElements[i];
                    if(current_element == currentPId)
                    {
                        eraseElement = 1;
                        continue;
                    }
                    cout << "Process " << current_element << " is " << processes[current_element].state << endl;//output the state of each process during this event
                }
                cout << "Process " << currentPId << " is TERMINATED" << endl;
                cout << endl << endl;
                processElements.erase(processElements.begin()+eraseElement);// erases the process since its terminated
                continue;
            }
            if (processes[currentPId].type[0] == "CORE")//if the following request is the CORE
            {
                if(core < num_core)// checks for available cores
                {
                    processes[currentPId].state = "RUNNING";//update the state at this event
                    processes[currentPId].endTime = clock + processes[currentPId].time[0];//update the state at this event
                    process *temp = &processes[currentPId];
                    priorityQueue.push(temp);
                    core = core + 1;
                }
                else// if the cores are not available then put in the ready queue
                {
                    readyQueue.push(currentPId);
                    processes[currentPId].state = "READY";
                }
                continue;
            }
            if (processes[currentPId].type[0] == "INPUT")//if the following request is the INPUT
            {
                if(input == 0)// INPUT not being used
                {
                    processes[currentPId].state = "BLOCKED";//update the state at this event
                    processes[currentPId].endTime = clock + processes[currentPId].time[0];//update the state at this event
                    process *temp = &processes[currentPId];
                    priorityQueue.push(temp);
                    input = 1;
                }
                else//INPUT being used
                {
                    inputQueue.push(currentPId);
                    processes[currentPId].state = "BLOCKED";
                }
                continue;
            }
        }
        if(current ->type[0] == "INPUT")//if the following request is the INPUT
        {
            clock = current ->endTime;
            if(inputQueue.size() != 0)//check if the there any processes waiting
            {
                int element_number = inputQueue.front();
                inputQueue.pop();
                processes[element_number].state = "BLOCKED";//update the state at this event
                processes[element_number].endTime = clock + processes[element_number].time[0];//update the state at this event
                process *temp = &processes[element_number];
                priorityQueue.push(temp);
            }
            else
            {
                input = input - 1;
            }
            priorityQueue.pop();
            processes[currentPId].time.erase(processes[currentPId].time.begin());
            processes[currentPId].type.erase(processes[currentPId].type.begin());
            // if current process is complete then it will output the states
            if(processes[currentPId].type.size() == 0)
            {
                cout << "Process " << currentPId << " terminates at time " << clock << " ms" << endl;
                int eraseElement;//will be used to exclude the process that is being terminated when outputting
                for(int i = 0; i < processElements.size(); i++)
                {
                    int current_element = processElements[i];
                    if(current_element == currentPId)
                    {
                        eraseElement = i;
                        continue;
                    }
                    cout << "Process " << current_element << " is " << processes[current_element].state << endl;//output the state of each process during this event
                };
                cout << "Process " << currentPId << " is TERMINATED" << endl;
                cout << endl << endl;
                processElements.erase(processElements.begin()+eraseElement);// erases the process since its terminated
                continue;
            }
            if(processes[currentPId].type[0] == "CORE")//if the following request is the CORE
            {
                if(core < num_core)// checks for available cores
                {
                    processes[currentPId].state = "RUNNING";//update the state at this event
                    processes[currentPId].endTime = clock + processes[currentPId].time[0];//update the state at this event
                    process *temp = &processes[currentPId];
                    priorityQueue.push(temp);
                    core = core + 1;
                }
                else// if not then it has to wait in the ready queue
                {
                    readyQueue.push(currentPId);
                    processes[currentPId].state = "READY";
                }
                continue;
            }
            if(processes[currentPId].type[0] == "SSD")//if the following request is the SSD
            {
                if(ssd == 0)// SSD not being used
                {
                    processes[currentPId].state = "BLOCKED";//update the state at this event
                    processes[currentPId].endTime = clock + processes[currentPId].time[0];//update the state at this event
                    process *temp = &processes[currentPId];
                    priorityQueue.push(temp);
                    ssdTime = ssdTime - clock;
                    ssd = 1;
                }
                else// SSD being used
                {
                    ssdQueue.push(currentPId);
                    processes[currentPId].state = "BLOCKED";
                    ssdTime = ssdTime - clock;
                }
                continue;
            }
        }
    }
    // summary
    cout << "SUMMARY:" << endl;
    cout << "Number of processes that completed: " << num_processes << endl;
    cout << "Total number of SSD accesses: " << num_ssd << endl;
    float avg_ssd = float(ssdTime) / float(num_ssd);//calculation for average SSD access time
    cout << "Average SSD access time: " << avg_ssd << " ms" << endl;
    cout << "Total elapsed time: " << clock << " ms" << endl;
    float coreUtilization = (float(coreTime) / float(clock)) * 100;//calculation for core utilization
    cout.precision(4);
    cout << "Core Utilization: " << coreUtilization << " percent" << endl;
    float ssdUtilization = (float(ssdTime) / float(clock)) * 100;//calculation for SSD utilization
    cout.precision(2);
    cout << "SSD Utilization: " << ssdUtilization << " percent" << endl;
}

