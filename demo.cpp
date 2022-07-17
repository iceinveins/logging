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
	if(!agent.isConnected()) return -1;
	agent.setLogPath();
	agent.setShmName();
	agent.setLogLevel(Level::DEBUG);

	// test changing the logPath while connected
	for(int i=0; i<10;++i)
	{
		agent.write(Logging::Level::NOTICE, "bla bla blablabla");
	}
	
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
	std::exit(0);
}
