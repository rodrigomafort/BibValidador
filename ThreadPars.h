#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class ThreadPars
	{
	public:
		Grafo* G;
		void* Pars;

		FilaParalela<string>* Log;
		int threadId;

		unsigned long numTestes;
		int minReq;
		int maxReq;
		default_random_engine* random;

		VetorRequisitos R;
		unsigned long ct;

		ThreadPars();
		//    ThreadPars(const ThreadPars & obj);	

		ThreadPars(int id, unsigned long testes, Grafo* g, FilaParalela<string>* log, default_random_engine* random, int min, int max, void* Pars);
		ThreadPars(unsigned long ct, Grafo* g, VetorRequisitos r, FilaParalela<string>* log, void* Pars);

		bool ThreadFinalizada;

		~ThreadPars();
	};
}