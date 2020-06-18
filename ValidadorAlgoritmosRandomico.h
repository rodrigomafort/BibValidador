#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class ValidadorAlgoritmosRandomico
	{
	private:
		Grafo* G;
		void* Pars;
		FilaParalela<string> Log;

		FuncaoExecutora* exe;
		FuncaoAvaliadora* aval;
		FuncaoCertificadora* cert;

		int N;
		string arquivoSaida;
		unsigned long numTestes;

		int nThreads;
		vector<thread> exeThreads;
		thread* logThread;
		LogArgs logArgs;

		default_random_engine* randomGenerator;

		int minReq;
		int maxReq;

		string ObterHora();
	public:
		ValidadorAlgoritmosRandomico(FuncaoExecutora *exe, FuncaoAvaliadora *aval, FuncaoCertificadora *cert, BibGrafos::ChamadaConstrutor *Construtor, int tamanho, string arquivoSaida, unsigned long numTestes, int nThreads, void* pars=NULL);
		ValidadorAlgoritmosRandomico(FuncaoExecutora *exe, FuncaoAvaliadora *aval, FuncaoCertificadora *cert, Grafo* G, string arquivoSaida, unsigned long numTestes, int nThreads, void* pars=NULL);

		void Executar();
		void Executar(int minReq, int maxReq=INT_MAX);

		void ThreadRun(reference_wrapper<ThreadPars> args);
		void ThreadLog(reference_wrapper<LogArgs> args);
	};
}
