#include "BibValidador.h"
namespace BibValidador
{
	ValidadorAlgoritmosExaustivo::ValidadorAlgoritmosExaustivo(FuncaoExecutora *e, FuncaoAvaliadora *a, FuncaoCertificadora *c, BibGrafos::ChamadaConstrutor *Construtor, int tamanho, string arquivo, int threads, void* p)
	{		
		exe = e;
		aval = a;
		cert = c;

		G = Construtor(tamanho);
		Pars = p;
		N = tamanho;
		arquivoSaida = arquivo;

		nThreads = threads;

		Fila = new FilaParalela<ThreadPars*>(nThreads * 100);

		minRequisito = 0;
		maxRequisito = INT_MAX;
	}

	ValidadorAlgoritmosExaustivo::ValidadorAlgoritmosExaustivo(FuncaoExecutora *e, FuncaoAvaliadora *a, FuncaoCertificadora *c, Grafo* g, string arquivo, int threads, void* p)
	{
		exe = e;
		aval = a;
		cert = c;

		G = g;
		Pars = p;
		N = G->N();
		arquivoSaida = arquivo;

		nThreads = threads;
		Fila = new FilaParalela<ThreadPars*>(nThreads * 100);

		minRequisito = 0;
		maxRequisito = INT_MAX;
	}

	string ValidadorAlgoritmosExaustivo::ObterHora()
	{
		time_t t = time(0);

		char cstr[128];
		strftime(cstr, sizeof(cstr), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

		return cstr;
	}

	void ValidadorAlgoritmosExaustivo::Executar(int min, int max)
	{
		minRequisito = min;
		maxRequisito = max;
		Executar();
	}

	void ValidadorAlgoritmosExaustivo::Executar()
	{
		combinacoes = G->ObterNumeroCombinacoes();

		stringstream init;
		init << "Inicio Processamento: " << ObterHora() << "\n";
		init << "Entradas:\n";
		init << "Numero de Vertices: " << N << "\n";
		init << "Numero de Combinacoes: " << combinacoes << "\n";
		init << "Threads: " << nThreads << "\n";
		init << "Grafo:\n";
		init << G->ObterStringMatrizAdjacencias() << "\n";
		init << G->ObterStringListaAdjacencias() << "\n";
		string str = G->ObterDetalhes();
		if (str != "")  init << str << "\n";
		if (G->info != "") init << G->info << "\n";
		cout << init.str();
		cout << "\nResultados:" << arquivoSaida << "\n";

		init << "\nResultados:\n";
		Log.Push(init.str());

		//Iniciar Thread de Log
		logArgs.Inicializar(&Log, arquivoSaida);
		logThread = new thread(&ValidadorAlgoritmosExaustivo::ThreadLog, this, ref(logArgs));

		VetorRequisitos R;

		//Iniciar Threads de Processamento	
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads.emplace_back(&ValidadorAlgoritmosExaustivo::ThreadRun, this, Fila, i + 1);
		}

		ProcessarExaustivo();

		Fila->FinalizarAdicao();

		for (int i = 0; i < nThreads; i++)
		{
			exeThreads[i].join();
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		cout << "Fim Processamento: " + ObterHora() + "\n";
		logArgs.Encerrar();
		logThread->join();
	}

	void ValidadorAlgoritmosExaustivo::ExecutarDividido(int requisitoV1)
	{
		stringstream init;
		init << "Inicio Processamento Dividido: " << ObterHora() << "\n";
		init << "Entradas:\n";
		init << "Numero de Vertices: " << N << "\n";
		init << "Requisito V1: " << requisitoV1 << "\n";
		init << "Numero de Combinacoes: " << G->ObterNumeroCombinacoes() / (G->Grau(G->VerticePorId(1)) + 1) << "\n";
		init << "Threads: " << nThreads << "\n";
		init << "Grafo:\n";
		init << G->ObterStringMatrizAdjacencias() << "\n";
		init << G->ObterStringListaAdjacencias() << "\n";
		string str = G->ObterDetalhes();
		if (str != "")  init << str << "\n";

		init << "\nResultados:\n";

		Log.Push(init.str());

		//Iniciar Thread de Log
		logArgs.Inicializar(&Log, arquivoSaida);
		logThread = new thread(&ValidadorAlgoritmosExaustivo::ThreadLog, this, ref(logArgs));

		VetorRequisitos R;

		//Iniciar Threads de Processamento	
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads.emplace_back(&ValidadorAlgoritmosExaustivo::ThreadRun, this, Fila, i + 1);
		}

		ProcessarExaustivoDividido(requisitoV1);

		Fila->FinalizarAdicao();

		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		//Aguardar Fim das Threads de Processamento
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads[i].join();
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
		logArgs.encerrarThread = true;

		//Aguardar Fim da Thread de Log
		logThread->join();
	}

	void ValidadorAlgoritmosExaustivo::Continuar()
	{
		//Ler requisitos j� processados
		vector<string> processados;

		string line;
		fstream file(arquivoSaida, std::fstream::in);

		while (getline(file, line))
		{
			if (line == "Resultados:")
			{
				break;
			}
		}

		while (getline(file, line))
		{
			std::stringstream res;
			res.str(line);
			string item;
			while (std::getline(res, item, '\t'))
			{
				if (item[0] == 'F')
				{
					processados.push_back(item);
				}
			}
		}


		sort(processados.begin(), processados.end());

		file.close();

		string retomando = "Retomando Processamento: " + ObterHora() + "\n";
		Log.Push(retomando);

		logArgs.Inicializar(&Log, arquivoSaida);
		logThread = new thread(&ValidadorAlgoritmosExaustivo::ThreadLog, this, ref(logArgs));

		VetorRequisitos R;
		//Iniciar Threads de Processamento	
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads.emplace_back(&ValidadorAlgoritmosExaustivo::ThreadRun_Continuar, this, Fila, i + 1, &processados);
		}

		ContinuarExaustivo();

		Fila->FinalizarAdicao();

		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		//Aguardar Fim das Threads de Processamento
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads[i].join();
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
		logArgs.encerrarThread = true;

		//Aguardar Fim da Thread de Log
		logThread->join();


	}

	void ValidadorAlgoritmosExaustivo::ContinuarDividido(int thresholdV1)
	{
		//Ler requisitos j� processados
		vector<string> processados;

		string line;
		fstream file(arquivoSaida, std::fstream::in);

		while (getline(file, line))
		{
			if (line == "Resultados:")
			{
				break;
			}
		}

		while (getline(file, line))
		{
			std::stringstream res;
			res.str(line);
			string item;
			while (std::getline(res, item, '\t'))
			{
				if (item[0] == 'F')
				{
					processados.push_back(item);
				}
			}
		}
		file.close();

		sort(processados.begin(), processados.end());

		string retomando = "Retomando Processamento: " + ObterHora() + "\n";
		Log.Push(retomando);

		logArgs.Inicializar(&Log, arquivoSaida);
		logThread = new thread(&ValidadorAlgoritmosExaustivo::ThreadLog, this, ref(logArgs));

		VetorRequisitos R;
		//Iniciar Threads de Processamento	
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads.emplace_back(&ValidadorAlgoritmosExaustivo::ThreadRun_Continuar, this, Fila, i + 1, &processados);
		}

		ContinuarExaustivoDividido(thresholdV1);

		Fila->FinalizarAdicao();

		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		//Aguardar Fim das Threads de Processamento
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads[i].join();
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
		logArgs.encerrarThread = true;

		//Aguardar Fim da Thread de Log
		logThread->join();


	}

	void ValidadorAlgoritmosExaustivo::ProcessarExaustivo()
	{
		ct = 0;
		int i = 1;
		VetorRequisitos R(G);
		Recursao(i, R);
	}

	void ValidadorAlgoritmosExaustivo::ProcessarExaustivoDividido(int requisitoV1)
	{
		ct = 0;

		int i = 2;
		VetorRequisitos R(G);

		Vertice v = G->VerticePorId(1);
		R[v] = requisitoV1;

		Recursao(i, R);
	}

	void ValidadorAlgoritmosExaustivo::ContinuarExaustivo()
	{
		ct = 0;

		int i = 1;
		VetorRequisitos F(G);
		ContinuarRecursao(i, F);
	}

	void ValidadorAlgoritmosExaustivo::ContinuarExaustivoDividido(int requisitoV1)
	{
		ct = 0;

		int i = 2;
		VetorRequisitos R(G);

		Vertice v = G->VerticePorId(1);
		R[v] = requisitoV1;

		ContinuarRecursao(i, R);
	}
	   
	void ValidadorAlgoritmosExaustivo::Recursao(int i, VetorRequisitos &R)
	{
		if (i > G->N())
		{
			ct = ct + 1;
			Fila->Push(new ThreadPars(ct, G, R, &Log, Pars));
		}
		else
		{
			Vertice v = G->VerticePorId(i);
			for (int t = minRequisito; t <= min(G->Grau(v), maxRequisito); t++)
			{
				R[v] = t;
				Recursao(i + 1, R);
				R[v] = 0;
			}
		}
	}

	void ValidadorAlgoritmosExaustivo::ContinuarRecursao(int i, VetorRequisitos &R)
	{
		if (i > G->N())
		{
			ct = ct + 1;
			Fila->Push(new ThreadPars(ct, G, R, &Log, Pars));
		}
		else
		{
			Vertice v = G->VerticePorId(i);
			for (int t = minRequisito; t <= min(G->Grau(v), maxRequisito); t++)
			{
				R[v] = t;
				ContinuarRecursao(i + 1, R);
				R[v] = 0;
			}
		}
	}

	void ValidadorAlgoritmosExaustivo::ThreadRun(FilaParalela<ThreadPars*>* fila, int id)
	{
		while (fila->Empty() == false || fila->AdicaoFinalizada() == false)
		{
			ThreadPars* par;
			if (fila->Pop(par))
			{
				//Solucionador* solver = new TSolucionador();
				//solver->Inicializar(par->G, par->F);
				string info;
				set<Vertice> S = exe(par->G, par->R, info, par->Pars);

				stringstream ss;
				stringstream solBT;
				ss << "CT=" << par->ct << "\t";
				ss << "TH=" << id << "\t";
				ss << "R={ " << par->R.ObterString() << "}\t";
				ss << "TamSol=" << S.size() << "\t";						
				
				if (aval(par->G, par->R, S))
				{
					ss << "SolValida=Sim" << "\t";
					
					bool res;
					set<Vertice> solOtm = cert(par->G, par->R, res);
					if (res == true)
					{
						if (S.size() <= solOtm.size())
						{
							ss << "SolMinima=Sim" << "\t";
						}
						else
						{
							ss << "SolMinima=Nao" << "\t";
							solBT << "\tSolBT={ ";
							int i = 0;
							for(Vertice v : solOtm)
							{
								solBT << v.Id();
								if (i < solOtm.size() - 1) solBT << " ";
								i++;

							}
							solBT << " }";
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

				ss << "Sol={ ";
				int i = 0;
				for(Vertice v : S)
				{
					ss << v.Id();
					if (i < S.size() - 1) ss << " ";
					i++;
				}
				ss << " }";
				ss << solBT.str();
				ss << "\tInfo=";
				ss << info;
				ss << "\n";
				par->Log->Push(ss.str());


				//delete(solver);
				//free(solver);

				delete(par);
				//free(par);
			}
		}
	}

	void ValidadorAlgoritmosExaustivo::ThreadRun_Continuar(FilaParalela<ThreadPars*>* fila, int id, vector<string>* proc)
	{
		while (fila->Empty() == false || fila->AdicaoFinalizada() == false)
		{
			ThreadPars* par;
			if (fila->Pop(par))
			{
				string fstr = "F={ " + par->R.ObterString() + "}";
				vector<string>::iterator pos = lower_bound(proc->begin(), proc->end(), fstr);
				if (fstr == *pos) continue;

				//Solucionador* solver = new TSolucionador();
				//solver->Inicializar(par->G, par->F);
				//vector<Vertice> solucao = solver->Executar();

				string info;
				set<Vertice> S = exe(par->G, par->R, info, par->Pars);


				stringstream ss;
				stringstream solBT;
				ss << "CT=" << par->ct << "\t";
				ss << "TH=" << id << "\t";
				ss << "R={ " << par->R.ObterString() << "}\t";
				ss << "TamSol=" << S.size() << "\t";

				if (aval(par->G, par->R, S))
				{
					ss << "SolValida=Sim" << "\t";

					bool res;
					set<Vertice> solOtm = cert(par->G, par->R, res);
					if (res == true)
					{
						if (S.size() <= solOtm.size())
						{
							ss << "SolMinima=Sim" << "\t";
						}
						else
						{
							ss << "SolMinima=Nao" << "\t";
							solBT << "\tSolBT={ ";
							int i = 0;
							for(Vertice v : solOtm)
							{
								solBT << v.Id();
								if (i < solOtm.size() - 1) solBT << " ";
								i++;

							}
							solBT << " }";
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

				ss << "Sol={ ";
				int i = 0;
				for(Vertice v : S)
				{
					ss << v.Id();
					if (i < S.size() - 1) ss << " ";
					i++;
				}
				ss << " }";
				ss << solBT.str();
				ss << "\n";
				par->Log->Push(ss.str());


				//delete(solver);
				//free(solver);

				delete(par);
				//free(par);
			}
		}
	}
	
	void ValidadorAlgoritmosExaustivo::ThreadLog(reference_wrapper<LogArgs> args)
	{
		ofstream outfile(args.get().arquivoSaida, ofstream::app);


		while (args.get().log->Empty() == false || args.get().log->AdicaoFinalizada() == false)
		{
			string content;
			if (args.get().log->Pop(content))
			{
				cout << content;
				outfile << content;
			}
		}
		outfile.flush();
		outfile.close();
	}

	void ValidadorAlgoritmosExaustivo::ExecutarKDominacao(int maxReq)
	{
		combinacoes = G->ObterNumeroCombinacoes();

		stringstream init;
		init << "Inicio Processamento: " << ObterHora() << "\n";
		init << "Entradas:\n";
		init << "Numero de Vertices: " << N << "\n";
		init << "Numero de Combinacoes: " << combinacoes << "\n";
		init << "Threads: " << nThreads << "\n";
		init << "Grafo:\n";
		init << G->ObterStringMatrizAdjacencias() << "\n";
		init << G->ObterStringListaAdjacencias() << "\n";
		string str = G->ObterDetalhes();
		if (str != "")  init << str << "\n";
		if (G->info != "") init << G->info << "\n";
		cout << init.str();
		cout << "\nResultados:" << arquivoSaida << "\n";

		init << "\nResultados:\n";
		Log.Push(init.str());

		//Iniciar Thread de Log
		logArgs.Inicializar(&Log, arquivoSaida);
		logThread = new thread(&ValidadorAlgoritmosExaustivo::ThreadLog, this, ref(logArgs));

		VetorRequisitos R;

		//Iniciar Threads de Processamento
		for (int i = 0; i < nThreads; i++)
		{
			exeThreads.emplace_back(&ValidadorAlgoritmosExaustivo::ThreadRun, this, Fila, i + 1);
		}

		for (int i = 1; i <= maxReq; i++)
		{
			VetorRequisitos R = VetorRequisitos::GerarConstante(G,i);
			Fila->Push(new ThreadPars(ct, G, R, &Log, Pars));
		}
		Fila->FinalizarAdicao();

		for (int i = 0; i < nThreads; i++)
		{
			exeThreads[i].join();
		}

		//Sinalisar Fim para Thread de Log
		Log.Push("Fim Processamento: " + ObterHora() + "\n");
		cout << "Fim Processamento: " + ObterHora() + "\n";
		logArgs.Encerrar();
		logThread->join();
	}

	void ValidadorAlgoritmosExaustivo::ExecutarKDominacao()
	{
		int grauMin = G->ObterGrauMinimo();
		ExecutarKDominacao(grauMin);
	}



}
