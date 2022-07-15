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

int 
main()
{
	Logging::Agent agent;
	// this is just a demo, so I won't check it's return value
	agent.start();
	agent.setLogPath();
	agent.setShmName();
	agent.setLogLevel(Level::DEBUG);

	// test changing the logPath while connected
	agent.write(Logging::Level::NOTICE, "change logPath to demo.log");
	agent.setLogPath("demo.log");

	signal(SIGINT, sig_int);
	signal(SIGPIPE, SIG_IGN);

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
