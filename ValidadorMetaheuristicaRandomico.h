#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class ValidadorMetaheuristicaRandomico
	{
	private:
		Grafo* G;
		void* Pars;
		FilaParalela<string> Log;

		FuncaoExecutora* exe;

		int N;
		string arquivoSaida;
		unsigned long numTestes;

		int nThreads;
		vector<thread> exeThreads;
		thread* logThread;
		LogArgs logArgs;

		default_random_engine* randomGenerator;

		int minThreshold;
		int maxThreshold;

		string ObterHora();

	public:
		ValidadorMetaheuristicaRandomico(FuncaoExecutora *exe, BibGrafos::ChamadaConstrutor *Construtor, int tamanho, string arquivoSaida, unsigned long numTestes, int nThreads, void* pars=NULL);
		ValidadorMetaheuristicaRandomico(FuncaoExecutora *exe, Grafo* G, string arquivoSaida, unsigned long numTestes, int nThreads, void* pars=NULL);

		void Executar();
		void Executar(int minThreshold, int maxThreshold);


		void ThreadRun(reference_wrapper<ThreadPars> args);
		void ThreadLog(reference_wrapper<LogArgs> args);
	};
}