#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class ValidadorAlgoritmosExaustivo
	{
	private:
		Grafo* G;
		void* Pars;


		FilaParalela<string> Log;

		int N;
		string arquivoSaida;

		int nThreads;
		unsigned long ct;
		unsigned long combinacoes;

		//bool continuar;
		vector<string> combinacoesProcessadas;

		vector<thread> exeThreads;
		thread* logThread;
		LogArgs logArgs;

		FilaParalela<ThreadPars*>* Fila;

		FuncaoExecutora* exe;
		FuncaoAvaliadora* aval;
		FuncaoCertificadora* cert;

		int minRequisito;
		int maxRequisito;

		void ProcessarExaustivo();
		void ProcessarExaustivoDividido(int requisitoV1);
		void Recursao(int i, VetorRequisitos &R);

		void ContinuarExaustivo();
		void ContinuarExaustivoDividido(int requisitoV1);
		void ContinuarRecursao(int i, VetorRequisitos &R);

		string ObterHora();

	public:
		ValidadorAlgoritmosExaustivo(FuncaoExecutora *exe, FuncaoAvaliadora *aval, FuncaoCertificadora *cert, BibGrafos::ChamadaConstrutor *Construtor, int tamanho, string arquivoSaida, int nThreads, void* Pars=NULL);
		ValidadorAlgoritmosExaustivo(FuncaoExecutora *exe, FuncaoAvaliadora *aval, FuncaoCertificadora *cert, Grafo* G, string arquivoSaida, int nThreads, void* Pars = NULL);

		void Executar();
		void Executar(int minRequisito, int maxRequisito);
		void ExecutarDividido(int requisitoV1);

		void Continuar();
		void ContinuarDividido(int requisitoV1);

		void ExecutarKDominacao();
		void ExecutarKDominacao(int maxReq);


		void ThreadRun(FilaParalela<ThreadPars*>* args, int id);
		void ThreadRun_Continuar(FilaParalela<ThreadPars*>* args, int id, vector<string>* proc);
		void ThreadLog(reference_wrapper<LogArgs> args);
	};
}
