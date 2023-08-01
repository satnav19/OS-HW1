#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num)
{ /// need to add job to joblist
	// TODO: Add your implementation

	cout << "smash: got ctrl-Z" << endl;
	SmallShell &smash = SmallShell::getInstance();

	pid_t pid = smash.fg_pid;
	if (pid == 0)
	{
		//std::cout << smash.getPrompt() << "> " << flush;
		return;
	}

	smash.jobList.addJob(smash.fg_command_name, smash.fg_pid, true,smash.fg_jobid);
	if (kill(pid, SIGSTOP) == -1)
	{
		perror("smash error: kill failed");
		return;
	}
	cout << "smash: process " << pid << " was stopped" << endl;
	smash.fg_pid = 0;
}

void ctrlCHandler(int sig_num)
{
	cout << "smash: got ctrl-C" << endl;
	SmallShell &smash = SmallShell::getInstance();
	int pid = smash.fg_pid;
	if (pid == 0)
	{
		//std::cout << smash.getPrompt() << "> " << flush;
		return;
	}

	if (kill(pid, SIGKILL) == -1)
	{
		perror("smash error: kill failed");
		return;
	}
	cout << "smash: process " << pid << " was killed" << endl;
	smash.fg_pid = 0;
	return;
}

void alarmHandler(int sig_num)
{
	cout << "smash: got an alarm" << endl;

	SmallShell &smash = SmallShell::getInstance();

	list<int> pid_list;
	for (list<timedCommand>::iterator iter = smash.timedList.begin(); iter != smash.timedList.end(); iter++)
	{
		int status = 0;
		int job_pid = waitpid(iter->pid, &status, WNOHANG);

		if (job_pid == iter->pid)
		{

			pid_list.push_front(iter->pid);
		}
	}
	for (int i : pid_list)
	{
		for (timedCommand command : smash.timedList)
		{

			if (command.pid == i)
			{
				smash.timedList.remove(command);
				break;
			}
		}
	}
	pid_list.clear();

	if (smash.timedList.empty())
	{

		return;
	}
	smash.timedList.sort();
	int pid = smash.timedList.front().pid;
	if (kill(pid, SIGKILL) == -1)
	{
		perror("smash error: kill failed");
		return;
	}
	cout << "smash: " << smash.timedList.front().command_line << " timed out!" << endl;
	smash.timedList.pop_front();
	smash.timedList.sort();
	alarm(difftime(smash.timedList.front().end_time, time(NULL)));
	return;
}
