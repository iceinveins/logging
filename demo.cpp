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
	Util::setCpuAffinity(1);
	
	Logging::Agent agent;
	// this is just a demo, so I won't check it's return value
	agent.start();
	if(!agent.isConnected()) return -1;
	agent.setLogPath();
	agent.setShmName();

	// test Logging::Level limitation
	for(int i=0; i<5;++i)
	{
		agent.write(Level::DEBUG, "bla bla blablabla");	// default level=Notice, so these will not write to log
	}
	agent.setLogLevel(Level::DEBUG);
	for(int i=0; i<5;++i)
	{
		agent.write(Level::DEBUG, "hello hello hello");	// default level=Notice, so these will not write to log
	}
	// test changing the logPath while connected
	agent.write(Level::WARNING, "change to demo.log");
	
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
	Util::print_cpu_time();
	std::exit(0);
}
