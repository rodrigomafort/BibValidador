#include "BibValidador.h"
namespace BibValidador
{
	ValidadorAlgoritmosRandomico::ValidadorAlgoritmosRandomico(FuncaoExecutora *e, FuncaoAvaliadora *v, FuncaoCertificadora *c, BibGrafos::ChamadaConstrutor *Construtor, int tamanho, string arquivo, unsigned long testes, int threads, void* p)
	{
		G = Construtor(tamanho);
		Pars = p;
		N = tamanho;

		exe = e;
		aval = v;
		cert = c;
		
		arquivoSaida = arquivo;
		remove(arquivoSaida.c_str());
		
		numTestes = testes;

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		randomGenerator = new default_random_engine(seed);

		nThreads = threads;

		minReq = 0;
		maxReq = G->N();

		logThread = NULL;
	}

	ValidadorAlgoritmosRandomico::ValidadorAlgoritmosRandomico(FuncaoExecutora *e, FuncaoAvaliadora *v, FuncaoCertificadora *c, Grafo* g, string arquivo, unsigned long testes, int threads, void* p)
	{
		G = g;
		Pars = p;
		N = G->N();

		exe = e;
		aval = v;
		cert = c;

		arquivoSaida = arquivo;
		remove(arquivoSaida.c_str());
		
		numTestes = testes;

		nThreads = threads;

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		randomGenerator = new default_random_engine(seed);

		minReq = 0;
		maxReq = G->N();

		logThread = NULL;
	}
		
	void ValidadorAlgoritmosRandomico::Executar(int min, int max)
	{
		if (max > G->N()) max = G->N();
		minReq = min;				
		maxReq = max;
		Executar();
	}
		
	string ValidadorAlgoritmosRandomico::ObterHora()
	{
		time_t t = time(0);

		char cstr[128];
		strftime(cstr, sizeof(cstr), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

		return cstr;
	}

	void ValidadorAlgoritmosRandomico::Executar()
	{
		stringstream init;
		init << "Inicio Processamento: " << ObterHora() << "\n";
		init << "Entradas:" << "\n";
		init << "Numero de Vertices: " << N << "\n";
		unsigned long testes = G->ObterNumeroCombinacoes();
		init << "Numero de Combinacoes: " << testes << "\n";
		init << "Numero de Testes: " << numTestes << "\n";
		double perc = ((double)numTestes / (double)testes) * 100.0;
		init << "Percentual Explorado Estimado: " << perc << "% \n";
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
		logThread = new thread(&ValidadorAlgoritmosRandomico::ThreadLog, this, ref(logArgs));

		map<int, ThreadPars*> pars;

		//Iniciar Threads de Processamento	
		for (int i = 0; i < nThreads; i++)
		{
			ThreadPars* arg = new ThreadPars(i + 1, numTestes, G, &Log, randomGenerator, minReq, maxReq, Pars);
			exeThreads.emplace_back(&ValidadorAlgoritmosRandomico::ThreadRun, this, ref(*arg));
			pars.insert(std::pair<int, ThreadPars*>(i + 1, arg));
		}

		//cout << "Threads Inicializadas";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		//Aguardar Fim das Threads de Processamento
		//for (int i = 0; i < nThreads; i++)
		//{
		//	cout << "Thread " + to_string(i + 1) + ": Aguardar Flag\n";
		//	while (pars[i + 1]->ThreadFinalizada == false) { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }
		//	cout << "Thread " + to_string(i + 1) + ": Flag ok\n";
		//}
		for (int i = 0; i < nThreads; i++)
		{
			//cout << "Thread " + to_string(i + 1) + ": Executar Join\n";
			exeThreads[i].join();
			//cout << "Thread " + to_string(i+1) + ": Join ok\n";
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		logArgs.Encerrar();

		//Aguardar Fim da Thread de Log
		//cout << "Log Thread: Aguardar Flag\n";
		//while (logArgs.logThreadEncerrada == false) { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }
		//cout << "Log Thread: Flag ok\n";

		//cout << "Log Thread: Executar Join\n";
		logThread->join();
		//cout << "Log Thread: Join ok\n";
	}

	//void ValidadorHipotesesRandomico<TGrafo,TSolucionador>::ThreadRun(ThreadPars args)

	
	void ValidadorAlgoritmosRandomico::ThreadRun(reference_wrapper<ThreadPars> args)
	{
		int tid = args.get().threadId;
		for (unsigned long ct = 1; ct <= args.get().numTestes; ct++)
		{
			int n = args.get().G->N();

			//cout << "Exe " + to_string(tid) + "_" + to_string(ct) + ": Inicio\n";		
			VetorRequisitos R = VetorRequisitos::GerarAleatorio(args.get().G, args.get().random, args.get().minReq, args.get().maxReq);			
			
			//cout << "Exe " + to_string(tid) + "_" + to_string(ct) + ": Solver\n";
			string info;			
			set<Vertice> S = exe(args.get().G, R, info, Pars);
			//cout << "Exe " + to_string(tid) + "_" + to_string(ct) + ": Solver OK\n";

			stringstream ss;
			stringstream solBT;
			ss << "TH=" << setw(3) << args.get().threadId << "\t";
			ss << "CT=" << setw(5) << ct << "\t";
			ss << "F={ " << setw(n) << R.ObterString() << "}\t";
			ss << "TamSol=" << setw(3) << S.size() << "\t";

			bool min;
			if (aval(args.get().G, R, S))
			{
				ss << "SolValida=" << setw(3) << "Sim" << "\t";

				bool res;
				set<Vertice> sOtm = cert(args.get().G, R, res);

				if (res == true)
				{
					if (S.size() <= sOtm.size())
					{
						min = true;
						ss << "SolMinima=" << setw(3) << "Sim" << "\t";
					}
					else
					{
						min = false;
						string tam = "(" + to_string(sOtm.size()) + ")";
						ss << "SolMinima=" << setw(3) << "Nao" << "\t";
						int i = 0;
						for(Vertice v : sOtm)
						{
							solBT << v.Id();
							if (i < sOtm.size() - 1) solBT << " ";
							i++;
						}
					}
				}
				else
				{
					ss << "ERRO CPLEX" << "\t";
				}
			}
			else
			{
				ss << "SolValida=Nao" << "\t";
			}

			stringstream sol;
			int i = 0;
			for(Vertice v : S)
			{
				sol << v.Id();
				if (i < S.size() - 1) sol << " ";
				i++;
			}
			ss << "Sol={ " << setw(3 * n) << sol.str() << "}";
			if (min == false)
				ss << "\tSolBT={ " << setw(3 * n) << solBT.str() << "}";
			else
				ss << "\t" << setw((3 * n) + 9) << " ";
			ss << "\tInfo=";
			ss << info;
			ss << "\n";
			//cout << "Exe " + to_string(tid) + "_" + to_string(ct) + ": Log Push\n";
			args.get().Log->Push(ss.str());
			//cout << "Exe " + to_string(tid) + "_" + to_string(ct) + ": Log Push OK\n";
			//cout << "Exe " + to_string(tid) + "_" + to_string(ct) + ": Fim\n";
		}
		//cout << "Encerrando Thread " + to_string(tid) + "\n";
		args.get().ThreadFinalizada = true;
	}

	
	void ValidadorAlgoritmosRandomico::ThreadLog(reference_wrapper<LogArgs> args)
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
