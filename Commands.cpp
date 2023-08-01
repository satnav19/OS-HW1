
#include "Commands.h"

using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY() \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT() \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string &s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string &s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string &s)
{
  return _rtrim(_ltrim(s));
}

void _parseCommandLine(string cmd_line, std::vector<string> &args, bool is_bg)
{
  FUNC_ENTRY()
  string temp = _trim(cmd_line);
  if (is_bg)
  {
    //  cout << "temp was"<< temp << endl;
    temp.pop_back();
    // cout << "temp is "<< temp << endl;
  }
  std::istringstream iss(temp);

  string s;
  while (iss >> s)
  {
    args.push_back(s);
  }

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char *cmd_line)
{
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char *cmd_line)
{
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos)
  {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&')
  {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h
/*
SmallShell::SmallShell() {
// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}
*/
/**
 * Creates and returns a pointer to Command class which matches the given command line (cmd_line)
 */
Command *SmallShell::CreateCommand(const char *cmd_line)
{
  // For example:
  /*
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if (firstWord.compare("pwd") == 0) {
      return new GetCurrDirCommand(cmd_line);
    }
    else if (firstWord.compare("showpid") == 0) {
      return new ShowPidCommand(cmd_line);
    }
    else if ...
    .....
    else {
      return new ExternalCommand(cmd_line);
    }
    */
  return nullptr;
}
const string SmallShell::getPrompt()
{
  return this->prompt;
}
void SmallShell::setPrompt(const string newPrompt)
{
  this->prompt = newPrompt;
}
void SmallShell::executeCommand(string cmd_line) /////add erorr message for syscalls
{
  bool is_background = false;
  int bg_pid;
  string leftOfPipe;
  string rightOfPipe;
  string leftOfRed;
  string rightOfRed;
  string old_cmd_line;
  int timeout = 0;
  std::vector<string> arr;
  if (cmd_line.find('&') != string::npos)
  {
    is_background = true;
  }

  if (cmd_line.find("|&") != string::npos)
  {

    leftOfPipe = cmd_line.substr(0, cmd_line.find("|&"));
    rightOfPipe = cmd_line.substr(cmd_line.find("|&") + 2);
    int myPipe[2];
    if (pipe(myPipe) == -1)
    {
      perror("mash error: pipe failed");
      return;
    }
    int oldFd = dup(2);
    dup2(myPipe[1], 2);
    executeCommand(leftOfPipe);
    close(2);
    dup2(oldFd, 2);
    close(oldFd);
    close(myPipe[1]);
    oldFd = dup(0);
    dup2(myPipe[0], 0);
    executeCommand(rightOfPipe);
    close(0);
    close(myPipe[0]);
    dup2(oldFd, 0);
    close(oldFd);
    return;
  }
  if (cmd_line.find('|') != string::npos)
  {

    leftOfPipe = cmd_line.substr(0, cmd_line.find('|'));
    rightOfPipe = cmd_line.substr(cmd_line.find('|') + 1);

    int myPipe[2];
    if (pipe(myPipe) == -1)
    {
      perror("smash error: pipe failed");
      return;
    }
    int oldFd = dup(1);
    dup2(myPipe[1], 1);
    executeCommand(leftOfPipe);
    close(1);
    dup2(oldFd, 1);
    close(oldFd);
    close(myPipe[1]);
    oldFd = dup(0);
    dup2(myPipe[0], 0);
    executeCommand(rightOfPipe);
    close(0);
    close(myPipe[0]);
    dup2(oldFd, 0);
    close(oldFd);
    return;
  }

  if (cmd_line.find(">>") != string::npos)
  {
    leftOfRed = cmd_line.substr(0, cmd_line.find(">>"));
    rightOfRed = cmd_line.substr(cmd_line.find(">>") + 2);
    rightOfRed = _trim(rightOfRed);
    int oldFD = dup(1);

    int newFd = open(rightOfRed.c_str(), O_RDWR | O_APPEND | O_CREAT, 0655); /// should be 0666
    if (newFd == -1)
    {
      perror("smash error: open failed");
      return;
    }

    dup2(newFd, 1);

    executeCommand(leftOfRed);
    dup2(oldFD, 1);
    close(newFd);
    return;
  }
  if (cmd_line.find(">") != string::npos)
  {
    leftOfRed = cmd_line.substr(0, cmd_line.find(">"));
    rightOfRed = cmd_line.substr(cmd_line.find(">") + 1);
    rightOfRed = _trim(rightOfRed);
    int oldFD = dup(1);

    int newFd = open(rightOfRed.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0655); /// should be 0666
    if (newFd == -1)
    {
      perror("smash error: open failed");
      return;
    }

    dup2(newFd, 1);

    executeCommand(leftOfRed);
    dup2(oldFD, 1);
    close(newFd);
    return;
  }

  list<int> pid_list;
  for (list<JobsList::JobEntry>::iterator iter = this->jobList.jobs.begin(); iter != this->jobList.jobs.end(); iter++)
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
    for (JobsList::JobEntry job : this->jobList.jobs)
    {

      if (job.pid == i)
      {
        this->jobList.jobs.remove(job);
        break;
      }
    }
  }
  pid_list.clear();
  for (list<JobsList::JobEntry>::iterator iter = this->jobList.jobs.begin(); iter != this->jobList.jobs.end(); iter++)
  {
    int status = 0;
    waitpid(iter->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (WIFSTOPPED(status))
    {
      iter->isStopped = true;
    }

    if (WIFCONTINUED(status))
    {
      iter->isStopped = false;
    }
  }

  _parseCommandLine(cmd_line, arr, is_background);
  if (arr.empty())
  {
    return;
  }

  if (arr[0] == "timeout")
  {
    if (arr.size() < 3)
    {
      cerr << "smash error: timeout: invalid arguments" << endl;
      return;
    }
    for (char c : arr[1])
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: timeout: invalid arguments" << endl;
        return;
      }
    }

    timeout = stoi(arr[1]);
    old_cmd_line = cmd_line;
    int index_of_command = cmd_line.find(arr[2]);
    cmd_line = cmd_line.substr(index_of_command);

    arr.clear();
    _parseCommandLine(cmd_line, arr, is_background);
  }
  if (cmd_line.find('*') != string::npos || cmd_line.find('?') != string::npos)
  {
    goto COMPLEX_COMMAND;
  }
  if (arr[0] == "chprompt")
  {
    if (arr.size() != 1)
    {
      this->setPrompt(arr[1]);
    }
    else
    {
      this->setPrompt("smash");
    }
    return;
  }
  if ((arr[0] == "showpid"))
  {
    cout << "smash pid is " << getpid() << endl;
    return;
  }

  if ((arr[0] == "pwd"))
  {
    char address[3000];
    cout << getcwd(address, 3000) << endl;

    return;
  }
  if ((arr[0] == "cd"))
  { // add error when cd is alone

    if (arr.size() > 2)
    {

      cerr << "smash error: cd: too many arguments" << endl;
      return;
    }
    if (arr.size() == 1)
    {
      cerr << "smash error:> " << cmd_line << endl;
      return;
    }
    if (current.empty())
    {
      char address[3000];
      getcwd(address, 3000);
      current = address;
    }
    if ((arr[1] == "-"))
    {
      if (!last.empty())
      {
        // cout << "the last directory is :" << this->directoryList.front() << "\n";
        if (chdir(last.c_str()) == -1)
        {
          perror("smash error: chdir failed");
          return;
        }
        last = current;
        char address[3000];
        getcwd(address, 3000);
        current = address;
        return;
      }

      cerr << "smash error: cd: OLDPWD not set" << endl;
      return;
    }

    else
    {

      if (chdir(arr[1].c_str()) == -1)
      {
        perror("smash error: chdir failed");
        return;
      }
      last = current;
      char address[3000];
      getcwd(address, 3000);
      current = address;

      return;
    }
  }
  if ((arr[0] == "jobs"))
  {
    this->jobList.jobs.sort();
    for (list<JobsList::JobEntry>::iterator iter = this->jobList.jobs.begin(); iter != this->jobList.jobs.end(); iter++)
    {

      if (iter->isStopped)
      {
        cout << "[" << iter->jobid << "] " << iter->name << " : " << iter->pid << " " << difftime(time(NULL), iter->start_time) << " secs"
             << " (stopped)" << endl;
      }
      else
      {
        cout << "[" << iter->jobid << "] " << iter->name << " : " << iter->pid << " " << difftime(time(NULL), iter->start_time) << " secs" << endl;
      }
    }
    return;
  }
  if (arr[0] == "fg")
  {
    if (arr.size() > 2)
    {
      cerr << "smash error: fg: invalid arguments" << endl;
      return;
    }
    if (arr.size() == 1)
    {
      if (this->jobList.jobs.empty())
      {
        cerr << "smash error: fg: jobs list is empty" << endl;
        return;
      }
      this->jobList.jobs.sort();
      JobsList::JobEntry job = this->jobList.jobs.back();
      this->fg_pid = job.pid;
      this->fg_command_name = job.name;
      this->fg_jobid = job.jobid;
      this->jobList.jobs.remove(job);
      cout << job.name << " : " << job.pid << endl;
      if (kill(job.pid, SIGCONT) == -1)
      {
        perror("smash error: kill failed");
        return;
      }
      waitpid(job.pid, NULL, WUNTRACED);
      timedCommand timed_check;
      bool found_timed_check = false;
      for (timedCommand command : this->timedList)
      {
        if (fg_pid == command.pid)
        {
          timed_check = command;
          found_timed_check = true;
          break;
        }
      }
      if (found_timed_check)
      {
        this->timedList.remove(timed_check);
      }
      this->fg_pid = 0;
      this->fg_jobid = 0;
      return;
    }
    if (((arr[1].at(0) == '-') && (arr[1].size() == 1)) || (!(((arr[1].at(0) > '0') && (arr[1].at(0) < '9')) || (arr[1].at(0) == '-'))))
    {
      cerr << "smash error: fg: invalid arguments" << endl;
      return;
    }
    string scheck = arr[1];
    scheck.erase(scheck.begin());

    for (char c : scheck)
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: fg: invalid arguments" << endl;
        return;
      }
    }
    for (JobsList::JobEntry job : this->jobList.jobs)
    {

      if (arr[1] == to_string(job.jobid))
      {
        this->fg_pid = job.pid;
        this->fg_command_name = job.name;
        this->fg_jobid = job.jobid;
        this->jobList.jobs.remove(job);
        cout << job.name << " : " << job.pid << endl;
        if (kill(job.pid, SIGCONT) == -1)
        {
          perror("smash error: kill failed");
          return;
        }
        waitpid(job.pid, NULL, WUNTRACED);
        timedCommand timed_check;
        bool found_timed_check = false;
        for (timedCommand command : this->timedList)
        {
          if (fg_pid == command.pid)
          {
            timed_check = command;
            found_timed_check = true;
            break;
          }
        }
        if (found_timed_check)
        {
          this->timedList.remove(timed_check);
        }
        this->fg_pid = 0;
        this->fg_jobid = 0;
        return;
      }
    }

    cerr << "smash error: fg: job-id " << arr[1] << " does not exist" << endl;
    return;
  }

  if (arr[0] == "bg")
  {
    if (arr.size() > 2)
    {
      cerr << "smash error: bg: invalid arguments" << endl;
      return;
    }
    if (arr.size() == 1)
    {
      this->jobList.jobs.reverse();
      for (list<JobsList::JobEntry>::iterator iter = this->jobList.jobs.begin(); iter != this->jobList.jobs.end(); iter++)
      {
        if (iter->isStopped)
        {
          cout << iter->name << " : " << iter->pid << endl;
          if (kill(iter->pid, SIGCONT) == -1)
          {
            perror("smash error: kill failed");
            return;
          }
          this->jobList.jobs.reverse();
          return;
        }
      }
      this->jobList.jobs.reverse();
      cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
      return;
    }
    if (((arr[1].at(0) == '-') && (arr[1].size() == 1)) || (!(((arr[1].at(0) > '0') && (arr[1].at(0) < '9')) || (arr[1].at(0) == '-'))))
    {
      cerr << "smash error: bg: invalid arguments" << endl;
      return;
    }
    string scheck = arr[1];
    scheck.erase(scheck.begin());

    for (char c : scheck)
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: bg: invalid arguments" << endl;
        return;
      }
    }
    for (JobsList::JobEntry job : this->jobList.jobs)
    {

      if (arr[1] == to_string(job.jobid))
      {
        if (!job.isStopped)
        {
          cerr << "smash error: bg: job-id " << job.jobid << " is already running in the background" << endl;
          return;
        }
        cout << job.name << " : " << job.pid << endl;
        if (kill(job.pid, SIGCONT) == -1)
        {
          perror("smash error: kill failed");
          return;
        }
        return;
      }
    }
    cerr << "smash error: bg: job-id " << arr[1] << " does not exist" << endl;
    return;
  }

  // add optional parameter for jobs
  if (arr[0] == "quit")
  {
    if (arr.size() == 1)
    {
      exit(0);
    }
    if (arr[1] == "kill")
    {
      cout << "smash: sending SIGKILL signal to " << this->jobList.jobs.size() << " jobs:" << endl;
      for (JobsList::JobEntry job : this->jobList.jobs)
      {
        if (kill(job.pid, SIGKILL) == -1)
        {
          perror("smash error: kill failed");
          return;
        }
        cout << job.pid << ": " << job.name << endl;
      }
      exit(0);
    }
  }
  if (arr[0] == "kill")
  {
    if (arr.size() != 3 || arr[1].empty() || arr[1].at(0) != '-')
    {

      cerr << "smash error: kill: invalid arguments" << endl;
      return;
    }
    arr[1].erase(arr[1].begin());
    for (char c : arr[1])
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: kill: invalid arguments" << endl;
        return;
      }
    }
    if (((arr[2].at(0) == '-') && (arr[2].size() == 1)) || (!(((arr[2].at(0) > '0') && (arr[2].at(0) < '9')) || (arr[2].at(0) == '-'))))
    {
      cerr << "smash error: kill: invalid arguments" << endl;
      return;
    }
    string scheck = arr[2];
    scheck.erase(scheck.begin());

    for (char c : scheck)
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: kill: invalid arguments" << endl;
        return;
      }
    }

    if (arr.size() != 3 || stoi(arr[1]) < 1 || stoi(arr[1]) > 31)
    {
      cerr << "smash error: kill: invalid arguments" << endl;
      return;
    }

    for (JobsList::JobEntry job : this->jobList.jobs)
    {

      if (to_string(job.jobid) == arr[2])
      {
        if (kill(job.pid, stoi(arr[1])) == -1)
        {
          perror("smash error: kill failed");
          return;
        }
        cout << "signal number " << arr[1] << " was sent to pid " << job.pid << endl;
        return;
      }
    }
    cerr << "smash error: kill: job-id " << arr[2] << " does not exist" << endl;
    return;
  }

  //// special commands go here
  if (arr[0] == "fare")
  {
    if (arr.size() != 4)
    {
      cerr << "smash error: fare: invalid arguments" << endl;
      return;
    }
    string file = "";
    fstream myFile;
    string line;
    int changes = 0;
    myFile.open(arr[1], fstream::in | fstream::out);
    if (!myFile.is_open())
    {
      cerr << "smash error: fare: invalid arguments" << endl;
      return;
    }
    while (getline(myFile, line))
    {
      file.append(line);
      file.append("\n");
    }
    unsigned int index = 0;
    do
    {
      index = file.find(arr[2]);

      if (index < file.size())
      {
        file.replace(index, arr[2].length(), arr[3]);
        changes++;
      }
    } while (index < file.size());

    myFile.close();
    myFile.open(arr[1], fstream::in | fstream::out | fstream::trunc);
    myFile << file;
    cout << "replaced " << changes << " instances of the string \"" << arr[2] << "\"" << endl;
    return;
  }
  if (arr[0] == "setcore")
  {
    cpu_set_t mask;
    if (arr.size() != 3)
    {
      cerr << "smash error: setcore: invalid arguments" << endl;
      return;
    }

    for (char c : arr[1])
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: setcore: invalid arguments" << endl;
        return;
      }
    }
    for (char c : arr[2])
    {
      if (c < '0' || c > '9')
      {
        cerr << "smash error: setcore: invalid arguments" << endl;
        return;
      }
    }
    unsigned int cpu_requested = stoi(arr[2]);
    if (stoi(arr[2]) < 0 || cpu_requested > std::thread::hardware_concurrency())
    {
      cerr << "smash error: setcore: invalid core number" << endl;
      return;
    }
    int pid = -1;
    for (list<JobsList::JobEntry>::iterator iter = this->jobList.jobs.begin(); iter != this->jobList.jobs.end(); iter++)
    {
      if (iter->jobid == stoi(arr[1]))
      {
        pid = iter->pid;
        break;
      }
    }
    if (pid == -1)
    {
      cerr << "smash error: setcore: job-id " << arr[1] << " does not exist" << endl;
      return;
    }
    CPU_ZERO(&mask);
    CPU_SET(stoi(arr[2]), &mask);
    int x = sched_setaffinity(pid, sizeof(mask), &mask);
    if (x == -1)
    {
      perror("smash error: sched_setaffinity failed");
      return;
    }
    return;
  }

  //// external commands go here

  bg_pid = fork();
  if (bg_pid == 0)
  {
    if (setpgrp() == -1)
    {
      perror("smash error: setpgrp failed");
      return;
    }
    string str;
    str.append(arr[0]);
    vector<const char *> out;
    out.reserve(arr.size() + 1);
    out.push_back(str.c_str());
    int counter = 0;
    for (const auto &s : arr)
    {
      if (counter)
      {
        out.push_back(s.data());
      }
      counter++;
    }
    out.push_back(NULL);
    // out.shrink_to_fit();

    const char *t = str.c_str();

    if (execvp(t, const_cast<char *const *>(out.data())) == -1)
    {
      perror("smash error: execvp failed");

      return;
    }
    exit(0);
  }
  if (timeout)
  {
    timedCommand timed;
    timed.command_line = old_cmd_line;
    timed.pid = bg_pid;
    timed.start_time = time(NULL);
    timed.end_time = timed.start_time + timeout;
    this->timedList.push_back(timed);
    this->timedList.sort();
    alarm(difftime(this->timedList.front().end_time, time(NULL)));
  }
  if (!is_background)
  {

    this->fg_pid = bg_pid;
    if (timeout)
    {
      this->fg_command_name = old_cmd_line;
    }
    else
    {
      this->fg_command_name = cmd_line;
    }
    waitpid(bg_pid, NULL, WUNTRACED);
    timedCommand timed_check;
    bool found_timed_check = false;
    for (timedCommand command : this->timedList)
    {
      if (fg_pid == command.pid)
      {
        timed_check = command;
        found_timed_check = true;
        break;
      }
    }
    if (found_timed_check)
    {
      this->timedList.remove(timed_check);
    }
    this->fg_pid = 0;
    this->fg_jobid = 0;
  }
  else
  {
    if (timeout)
    {
      this->jobList.addJob(old_cmd_line, bg_pid);
    }
    else
    {
      this->jobList.addJob(cmd_line, bg_pid);
    }
  }
  return;

COMPLEX_COMMAND:

  bg_pid = fork();
  if (bg_pid == 0)
  {
    if (setpgrp() == -1)
    {
      perror("smash error: setpgrp failed");
      return;
    }

    vector<const char *> out;
    out.reserve(cmd_line.size() + 3);
    out.push_back("/bin/bash");
    out.push_back("-c");
    out.push_back(cmd_line.data());
    out.push_back(NULL);
    // out.shrink_to_fit();

    string s = "/bin/bash";
    const char *t = s.c_str();

    if (execvp(t, const_cast<char *const *>(out.data())) == -1)
    {
      perror("smash error: execvp failed");
      return;
    }
    exit(0);
  }
  if (timeout)
  {
    timedCommand timed;
    timed.command_line = old_cmd_line;
    timed.pid = bg_pid;
    timed.start_time = time(NULL);
    timed.end_time = timed.start_time + timeout;
    this->timedList.push_back(timed);
    this->timedList.sort();
    alarm(difftime(this->timedList.front().end_time, time(NULL)));
  }
  if (!is_background)
  {

    this->fg_pid = bg_pid;
    if (timeout)
    {
      this->fg_command_name = old_cmd_line;
    }
    else
    {
      this->fg_command_name = cmd_line;
    }
    waitpid(bg_pid, NULL, WUNTRACED);
    timedCommand timed_check;
    bool found_timed_check = false;
    for (timedCommand command : this->timedList)
    {
      if (fg_pid == command.pid)
      {
        timed_check = command;
        found_timed_check = true;
        break;
      }
    }
    if (found_timed_check)
    {
      this->timedList.remove(timed_check);
    }
    this->fg_pid = 0;
    this->fg_jobid = 0;
  }
  else
  {
    if (timeout)
    {
      this->jobList.addJob(old_cmd_line, bg_pid);
    }
    else
    {
      this->jobList.addJob(cmd_line, bg_pid);
    }
  }
}
