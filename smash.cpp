#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char *argv[])
{
    struct sigaction alarm_handler;
    alarm_handler.sa_flags = SA_RESTART;
    alarm_handler.sa_handler = alarmHandler;
    if (signal(SIGTSTP, ctrlZHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if (signal(SIGINT, ctrlCHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-C handler");
    }
    if (sigaction(SIGALRM, &alarm_handler, NULL))
    {
        perror("smash error: failed to set alarm handler");
    }

    // TODO: setup sig alarm handler

    SmallShell &smash = SmallShell::getInstance();
    /*things here should be in constructor*/
    // smash.setPrompt("smash");
    smash.setPrompt(string("smash"));
    /*constructor stuff*/

    while (true)
    {

        std::cout << smash.getPrompt() << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line);
    }
    return 0;
}