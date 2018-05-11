#include <smirtl.hxx>

class EvtBuilder: public SmiProxy, public DimTimer
{
// Command Handler
	void smiCommandHandler();
// Internal functions
	void do_start();
	void do_stop();
	void timerHandler() 
	{	
		setState("ERROR");
	}
public:
// Constructor
	EvtBuilder(char *domain, char *object): 
		SmiProxy(object) 
		{ 
			attach(domain);
		};
};

int main(int argc, char *argv[])
{
	int num = 123;
// Creatting the Proxy
	EvtBuilder evtBuilder(argv[1], "EVT_BUILDER");

// Setting Volatile
	evtBuilder.setVolatile();

// Setting object parameters

	evtBuilder.setParameter("NUMBER_P",1);
	evtBuilder.setParameter("NUMBER_T",1);

// Setting Object states
	evtBuilder.setState("READY");
	while(1)
	{
		pause();
	}
	return 0;
}

void EvtBuilder::smiCommandHandler()
{
	if(testAction("START"))
		do_start();
	else if(testAction("STOP"))
		do_stop();
	else if(testAction("RECOVER"))
		do_stop();
}

void EvtBuilder::do_start()
{

// Getting Action Parameters
	int runNumber = getParameterInt("NR");
	char *runType = getParameterString("TYPE");

// Setting an Object parameter
	runNumber++;

	if(!strcmp(runType,"TEST"))
	{
		setParameter("NUMBER_T",runNumber);
	}
	else
	{
		setParameter("NUMBER_P",runNumber);
	}
	setState("RUNNING");
// start timer
	start(10);
}

void EvtBuilder::do_stop()
{
//stop timer
	stop();

	setState("READY");
}
