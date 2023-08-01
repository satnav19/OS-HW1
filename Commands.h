#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_
#include <list>
#include <vector>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include "signals.h"
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
using namespace std;
class Command
{
  // TODO: Add your data members
public:
  Command(const char *cmd_line);
  virtual ~Command();
  virtual void execute() = 0;
  // virtual void prepare();
  // virtual void cleanup();
  //  TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command
{
public:
  BuiltInCommand(const char *cmd_line);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command
{
public:
  ExternalCommand(const char *cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command
{
  // TODO: Add your data members
public:
  PipeCommand(const char *cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command
{
  // TODO: Add your data members
public:
  explicit RedirectionCommand(const char *cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  // void prepare() override;
  // void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand
{
  // TODO: Add your data members public:
  ChangeDirCommand(const char *cmd_line, char **plastPwd);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand
{
public:
  GetCurrDirCommand(const char *cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand
{
public:
  ShowPidCommand(const char *cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand
{
  // TODO: Add your data members
public:
  QuitCommand(const char *cmd_line, JobsList *jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};

class JobsList
{
public:
  class JobEntry
  {
  public:
    JobEntry(string command, bool isStopped, int pid, int id) : name(command), isStopped(isStopped), pid(pid), jobid(id)
    {
      start_time = time(NULL);
    }
    JobEntry() : name("command"), isStopped(false), pid(0), jobid(0)
    {
      start_time = time(NULL);
    }
    bool operator==(JobEntry job)
    {
      return this->jobid == job.jobid;
    }
    bool operator<(JobEntry job)
    {
      return this->jobid < job.jobid;
    }
    ~JobEntry() = default;
    // JobEntry(const &JobEntry)=default;
    // TODO: Add your data members
    string name;
    bool isStopped;
    int pid;
    int jobid;
    time_t start_time;
  };
  // TODO: Add your data members
public:
  list<JobEntry> jobs;
  JobsList() = default;
  ~JobsList() = default;
  void addJob(string cmd, int pid, bool isStopped = false,int jobid=0)
  {
    if(jobid){
      jobs.push_back(JobEntry(cmd, isStopped, pid, jobid));
      jobs.sort();
      return;
    }
    int id = 0;
    for (JobEntry job : jobs)
    {
      
      if (job.jobid > id)
      {
        id = job.jobid;
      }
    }

    id++;

    jobs.push_back(JobEntry(cmd, isStopped, pid, id));
   
    return;
  }
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry *getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry *getLastJob(int *lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand
{
  // TODO: Add your data members
public:
  JobsCommand(const char *cmd_line, JobsList *jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand
{
  // TODO: Add your data members
public:
  ForegroundCommand(const char *cmd_line, JobsList *jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand
{
  // TODO: Add your data members
public:
  BackgroundCommand(const char *cmd_line, JobsList *jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand
{
  /* Optional */
  // TODO: Add your data members
public:
  explicit TimeoutCommand(const char *cmd_line);
  virtual ~TimeoutCommand() {}
  void execute() override;
};

class FareCommand : public BuiltInCommand
{
  /* Optional */
  // TODO: Add your data members
public:
  FareCommand(const char *cmd_line);
  virtual ~FareCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand
{
  /* Optional */
  // TODO: Add your data members
public:
  SetcoreCommand(const char *cmd_line);
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand
{
  /* Bonus */
  // TODO: Add your data members
public:
  KillCommand(const char *cmd_line, JobsList *jobs);
  virtual ~KillCommand() {}
  void execute() override;
};
class timedCommand
{
public:
  int pid;
  string command_line;
  time_t start_time;
  time_t end_time;
  bool operator<(timedCommand command)
  {
    return this->end_time < command.end_time;
  }
  bool operator==(timedCommand command)
  {
    return this->pid == command.pid;
  }
};
class SmallShell
{
private:
  // TODO: Add your data members
  string prompt;

  SmallShell() = default;

public:
  int fg_pid = 0;
  string fg_command_name = "default_name";
  int fg_jobid = 0;
  bool isPipe = false;
  string current;
 string last;
  JobsList jobList;
  list<string> directoryList;
  list<timedCommand> timedList;
  Command *CreateCommand(const char *cmd_line);
  SmallShell(SmallShell const &) = delete;     // disable copy ctor
  void operator=(SmallShell const &) = delete; // disable = operator
  static SmallShell &getInstance()             // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  const string getPrompt();
  void setPrompt(const string newPrompt);
  ~SmallShell() = default;
  void executeCommand(const string cmd_line);
  // TODO: add extra methods as needed
};

#endif // SMASH_COMMAND_H_
