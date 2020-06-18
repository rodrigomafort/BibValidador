#include "BibValidador.h"
namespace BibValidador
{
	ThreadPars::ThreadPars(int id, unsigned long testes, Grafo* g, FilaParalela<string>* log, default_random_engine* rnd, int min, int max, void* p)
	{
		threadId = id;
		G = g;
		Pars = p;
		Log = log;
		random = rnd;

		ct = 0;

		numTestes = testes;
		minReq = min;
		maxReq = max;

		ThreadFinalizada = false;
	}

	ThreadPars::ThreadPars(unsigned long num, Grafo* g, VetorRequisitos r, FilaParalela<string>* log, void* p)
	{
		ct = num;
		G = g;
		Pars = p;
		Log = log;
		R = r;
		maxReq = 0;
		minReq = 0;
		numTestes = 0;
		threadId = 0;
		ThreadFinalizada = false;
		random = new default_random_engine();
	}

	ThreadPars::ThreadPars() // @suppress("Class members should be properly initialized")
	{
		ThreadFinalizada = false;
		ct = 0;
		maxReq = 0;
		minReq = 0;
		G = NULL;
	}

	//template <class TGrafo>	
	//ThreadPars<TGrafo>::ThreadPars(const ThreadPars & obj)
	//{}

	ThreadPars::~ThreadPars()
	{

	}
}
