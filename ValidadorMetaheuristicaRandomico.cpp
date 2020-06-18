#include "BibValidador.h"
namespace BibValidador
{
	ValidadorMetaheuristicaRandomico::ValidadorMetaheuristicaRandomico(FuncaoExecutora *e, BibGrafos::ChamadaConstrutor *Construtor, int tamanho, string arquivo, unsigned long testes, int threads, void* p)
	{
		G = Construtor(tamanho);
		Pars = p;
		N = tamanho;

		exe = e;

		arquivoSaida = arquivo;
		remove(arquivoSaida.c_str());

		numTestes = testes;

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		randomGenerator = new default_random_engine(seed);

		nThreads = threads;

		minThreshold = 0;
		maxThreshold = INT_MAX;
	}

	ValidadorMetaheuristicaRandomico::ValidadorMetaheuristicaRandomico(FuncaoExecutora *e, Grafo* g, string arquivo, unsigned long testes, int threads, void* p)
	{
		G = g;
		Pars = p;
		N = G->N();

		exe = e;

		arquivoSaida = arquivo;
		remove(arquivoSaida.c_str());

		numTestes = testes;

		nThreads = threads;

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		randomGenerator = new default_random_engine(seed);

		minThreshold = 0;
		maxThreshold = INT_MAX;
	}


	void ValidadorMetaheuristicaRandomico::Executar(int min, int max)
	{
		minThreshold = min;
		maxThreshold = max;
		Executar();
	}


	string ValidadorMetaheuristicaRandomico::ObterHora()
	{
		time_t t = time(0);

		char cstr[128];
		strftime(cstr, sizeof(cstr), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

		return cstr;
	}

	void ValidadorMetaheuristicaRandomico::Executar()
	{
		stringstream init;
		init << "Inicio Processamento: " << ObterHora() << "\n";
		init << "Entradas:" << "\n";
		init << "Numero de Vertices: " << N << "\n";
		init << "Numero de Testes: " << numTestes << "\n";
		init << "Threads: " << nThreads << "\n";
		init << "Grafo:\n";
		init << G->ObterStringMatrizAdjacencias() << "\n";
		init << G->ObterStringListaAdjacencias() << "\n";
		string str = G->ObterDetalhes();
		if (str != "")  init << str << "\n";
		if (G->info != "") init << G->info << "\n";

		init << "\nResultados:\n";

		Log.Push(init.str());

		//Iniciar Thread de Log
		logArgs.Inicializar(&Log, arquivoSaida);
		logThread = new thread(&ValidadorMetaheuristicaRandomico::ThreadLog, this, ref(logArgs));

		map<int, ThreadPars*> pars;

		//Iniciar Threads de Processamento	
		for (int i = 0; i < nThreads; i++)
		{
			ThreadPars* arg = new ThreadPars(i + 1, numTestes, G, &Log, randomGenerator, minThreshold, maxThreshold, Pars);
			exeThreads.emplace_back(&ValidadorMetaheuristicaRandomico::ThreadRun, this, ref(*arg));
			pars.insert(std::pair<int, ThreadPars*>(i + 1, arg));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		for (int i = 0; i < nThreads; i++)
		{
			exeThreads[i].join();
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		
		logArgs.Encerrar();
		logThread->join();
	}
	
	void ValidadorMetaheuristicaRandomico::ThreadRun(reference_wrapper<ThreadPars> args)
	{
		Grafo* G = args.get().G;
		int n = G->N();
		int tid = args.get().threadId;
		void* Pars = args.get().Pars;

		for (unsigned long ct = 1; ct <= args.get().numTestes; ct++)
		{	
			VetorRequisitos R = VetorRequisitos::GerarAleatorio(args.get().G, args.get().random, args.get().minReq, args.get().maxReq);
			
			stringstream ss;			
			ss << "TH=" << setw(3) << args.get().threadId << "\t";
			ss << "CT=" << setw(5) << ct << "\t";
			
			int tamMeta = 0;
			int tamOtm = 0;
			
			stringstream sMeta;
			stringstream sOtm;

			bool valido = true;

			double tempoMeta;
			double tempoOtm;

			FuncaoExecutora* exeLocal = exe;
			std::thread tMeta([&sMeta, &G, &R, &exeLocal, &tamMeta, &valido, &tempoMeta, &Pars]
				{
					string info;

					clock_t begin = clock();
					set<Vertice> solucaoMetaheuristica = exeLocal(G, R, info, Pars);
					clock_t end = clock();
					tempoMeta = double(end - begin) / CLOCKS_PER_SEC;

					tamMeta = solucaoMetaheuristica.size();

					stringstream sol;
					int i = 0;
					for(Vertice v : solucaoMetaheuristica)
					{
						sol << v.Id();
						if (i < solucaoMetaheuristica.size() - 1) sol << " ";
						i++;
					}

					valido = R.VDAvaliarDominacao(solucaoMetaheuristica, G);
					sMeta << "SolMeta={ " << sol.str() << "}\t";
					sMeta << "Info=" << info;
				});

			std::thread tOtm([&sOtm, &G, &R, &tamOtm, &tempoOtm] {

				VDCPLEX ibt(G, R);
				clock_t begin = clock();
				bool resultado = ibt.BuscarSolucao();
				clock_t end = clock();
				tempoOtm = double(end - begin) / CLOCKS_PER_SEC;
				if (resultado == true)
				{					
					tamOtm = ibt.solucaoEncontrada.size();
					stringstream sol;
					int i = 0;
					for(Vertice v : ibt.solucaoEncontrada)
					{
						sol << v.Id();
						if (i < ibt.solucaoEncontrada.size() - 1) sol << " ";
						i++;
					}
					sOtm << "SolOtm={ " << sol.str() << "}";
				}
				else
				{
					sOtm << "CPLEX ERROR";
					tamOtm = -1;
				}
			});

			tMeta.join();
			tOtm.join();

			int dif = tamMeta - tamOtm;

			ss << "TamMeta=" << tamMeta << "\t";
			ss << "TamOtm=" << tamOtm << "\t";
			ss << "Dif=" << dif << "\t";
			ss << "Valido=" << (valido ? "Sim\t" : "Nao\t");
			ss << "TempoMeta" << tempoMeta << "\t";
			ss << "TempoOtm" << tempoOtm << "\t";
			ss << "R={ " << R.ObterString() << "}\t";
			ss << sMeta.str() << "\t";
			ss << sOtm.str() << "\t";
			if (dif == 0 && valido == true) ss << "OTM\n"; else ss << "\n";
			args.get().Log->Push(ss.str());
		}		
		args.get().ThreadFinalizada = true;
	}


	void ValidadorMetaheuristicaRandomico::ThreadLog(reference_wrapper<LogArgs> args)
	{
		ofstream outfile(args.get().arquivoSaida);

		while (args.get().log->Empty() == false || args.get().log->AdicaoFinalizada() == false)
		{
			string content;
			if (args.get().log->Pop(content))
			{
				cout << content;
				outfile << content;
			}

		}
		args.get().logThreadEncerrada = true;
		outfile.close();
	}
}
