#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <string>

#include "shm.h"
#include "ipc.h"
#include "util.h"
#include "agent.h"

using namespace std;
using namespace Logging;

void sig_int(int signo);
void sig_pipe(int signo);

int 
main()
{
	Logging::Agent agent;
	// this is just a demo, so I won't check it's return value
	agent.setLogPath("demo.log");
	agent.start();

	signal(SIGINT, sig_int);
	signal(SIGPIPE, sig_pipe);
	string log;
	while(true)
	{
		cout << "Input log>>> ";
		cin >> log;
		agent.write(Logging::Level::NOTICE, log);
    }
	return 0;
}

void 
sig_int(int signo)
{
	(void) (signo);
	print_cpu_time();
	exit(0);
}

void 
sig_pipe(int signo)
{
	(void) (signo);
}