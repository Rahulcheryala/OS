#include <algorithm>
#include <fstream>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>

using namespace std;

int Time_Slice = 10;

struct Process
{
    int ID;
    int Arrival_Time;
    int Turnaround_Time = 0;
    int Waiting_Time = 0;
    vector<int> Burst_Times;
};

Process createProcess(int ID, const std::string &s)
{
    Process process;
    process.ID = ID;

    std::istringstream ss(s);
    std::string token;

    ss >> process.Arrival_Time;

    while (ss >> token && token != "-1")
        process.Burst_Times.push_back(std::stoi(token));

    return process;
}

class RoundRobin
{
public:
    int IO_Available_Time = 0;

    struct Arrival_Time
    {
        bool operator()(Process a, Process b)
        {
            return a.Arrival_Time > b.Arrival_Time;
        }
    };

    queue<Process> Q_Ready;
    priority_queue<Process, vector<Process>, Arrival_Time> Q_Waiting;
    vector<Process> Finished;

    void addProcess(Process process)
    {
        Q_Waiting.push(process);
    }

    int run(int Time)
    {
        while (Q_Waiting.empty() == false && Q_Waiting.top().Arrival_Time <= Time)
        {
            Q_Ready.push(Q_Waiting.top());
            Q_Waiting.pop();
        }

        if (Q_Ready.empty() == false)
        {
            Process process = Q_Ready.front();
            Q_Ready.pop();
            printf("Process %d is running at Time %d\n", process.ID, Time);

            process.Waiting_Time += Time - process.Arrival_Time;
            process.Turnaround_Time = process.Waiting_Time + min(Time_Slice, process.Burst_Times[0]);

            Time += min(Time_Slice, process.Burst_Times[0]);

            process.Burst_Times[0] -= min(Time_Slice, process.Burst_Times[0]);

            process.Arrival_Time = Time;

            if( process.Burst_Times[0] == 0 )
            {
                process.Burst_Times.erase(process.Burst_Times.begin());

                if( process.Burst_Times.size() > 1 )
                {
                    process.Arrival_Time = max(Time, IO_Available_Time) + process.Burst_Times[0];
                    IO_Available_Time = process.Arrival_Time;
                    Q_Waiting.push(process);
                    
                    process.Burst_Times.erase(process.Burst_Times.begin());
                }
                else
                {
                    Finished.push_back(process);
                }
            }
            else
                Q_Waiting.push(process);
        }
        else
            Time++;

        return Time;
    }
};

int main(int argc, char const *argv[])
{
    ifstream input_file;
    input_file.open(argv[1]);

    vector<Process> Processes;

    while (true)
    {
        char line[1000];
        string line_str;

        input_file.getline(line, 1000);
        line_str = line;

        if (line_str.compare(0, 5, "<pre>") == 0)
            break;
    }

    int PID = 0;
    while (true)
    {
        char line[1000];
        string line_str;

        input_file.getline(line, 1000);
        line_str = line;

        if (input_file.eof() || line_str.compare(0, 6, "</pre>") == 0)
            break;

        Process process = createProcess(PID, line_str);

        Processes.push_back(process);
        PID++;
    }

    RoundRobin RR;

    for (Process process : Processes)
        RR.addProcess(process);

    int Time = 0;
    int context_switches = -1;

    while (RR.Q_Ready.empty() == false || RR.Q_Waiting.empty() == false)
    {
        context_switches++;
        Time = RR.run(Time);
    }

	printf("Process\tWaiting Time\tTurnaround Time\tPenalty Ratio\n");

	sort(RR.Finished.begin(), RR.Finished.end(), [](Process a, Process b) { return a.ID < b.ID; });

	for (Process process : RR.Finished)
	{
		int PID = process.ID;
		int WT = process.Waiting_Time;
		int TA = process.Turnaround_Time;
		float PR = (float)process.Turnaround_Time/(process.Turnaround_Time-process.Waiting_Time);
		printf("%d \t %d \t \t %d \t \t %f\n", PID, WT, TA, PR);
	}
	
	int Total_WT = 0;
	int Total_TA = 0;
	float Total_PR = 0.0;
	
    for (const Process& process : RR.Finished)
    {
        Total_WT += process.Waiting_Time;
        Total_TA += process.Turnaround_Time;

        if ((process.Turnaround_Time - process.Waiting_Time) != 0)
            Total_PR += static_cast<float>(process.Turnaround_Time) / (process.Turnaround_Time - process.Waiting_Time);
    }

    if (RR.Finished.empty() == false)
        Total_PR /= static_cast<float>(RR.Finished.size());

    float Avg_WT = (float)Total_WT / RR.Finished.size();
    float Avg_TA = (float)Total_TA / RR.Finished.size();
    float Avg_PR = Total_PR / RR.Finished.size();

    float Throughput = (float)RR.Finished.size() / (Time + context_switches * 1);

	printf("Average \t %f \t %f \t %f \n", Avg_WT, Avg_TA, Avg_PR);
	printf("Throughput: %f \n", Throughput);
	
	return 0;
}
