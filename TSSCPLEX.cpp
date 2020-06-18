#include "BibValidador.h"
namespace BibValidador
{	
	TSSCPLEX::TSSCPLEX(Grafo * g, VetorRequisitos r)// :		
	{
		R = r;
		G = g;
		/*		
		
		env(),
		handler(env, cout),
		modelo(),
		streamModelo(modelo.ObterModeloTSS()),
		modelSource(env, streamModelo, "TSS"),
		settings(env, handler),
		def(modelSource, settings),
		cp(env),
		opl(def,cp),
		aborter(env)

		cp.setParam(IloCplex::Threads, 1);
		cp.setParam(IloCplex::BoolParam::CloneLog, false);
		cp.setOut(env.getNullStream());
		cp.use(aborter);
		cout << "CONSTRUTOR OK\n";

		*/
	}

	const char* TSSCPLEX::ObterModelo()
	{
		return (char*)"\
		int N = ...;\
		int MAX_S = ...;\
		range V = 1..N;\
		int ADJ[v in V][u in V] = ...;\
		int R[1..N] = ...;\
		dvar int S[v in V] in 0 .. 1;\
		dvar int D[v in V][0..N] in 0 .. 1;\
		dexpr int custo = sum(v in V) S[v];\
		minimize custo;\
		subject to{\
			custo <= MAX_S;\
			forall(v in V)\
				D[v][0] == S[v];\
			forall(v in V)\
				D[v][N] == 1;\
			forall(k in 0..N - 1)\
				forall(v in V)\
					D[v,k + 1] >= D[v,k];\
			forall(k in 0..N - 1)\
				forall(v in V)\
					D[v,k + 1] <= D[v,k] + ((sum(j in V) D[j,k] * ADJ[v,j]) / R[v]);\
		}\
		execute DISPLAY{\
			for (var v in V)\
				writeln(v,\"\t\",S[v]);\
		}\
	";
	}
	
	CPLEXResult TSSCPLEX::BuscarSolucao()
	{
		IloEnv env;
		CPLEXResult result = ERRO;
		try
		{
			IloOplErrorHandler handler(env, cout);
			std::istringstream in(ObterModelo());
			IloOplModelSource modelSource(env, in, "IntervalSet");
			IloOplSettings settings(env, handler);
			IloOplModelDefinition def(modelSource, settings);
			IloCplex cp(env);
			//cp.setParam(IloCplex::TiLim, 30);
			//cp.setParam(IloCplex::MIPDisplay, 3);
			//cp.setParam(IloCplex::TiLim, 5);
			//cp.setParam(IloCplex::Threads, 1);
			cp.setParam(IloCplex::BoolParam::CloneLog, false);
			cp.setOut(env.getNullStream());
			IloOplModel opl(def, cp);			
			CPLEXDataSource data(env, G, &R);
			IloOplDataSource dataSource(&data);
			opl.addDataSource(dataSource);
			opl.generate();

			if (cp.solve())
			{
				IloIntMap S_Res = opl.getElement("S").asIntMap();

				for (int i = 0; i < S_Res.getSize(); i++)
				{
					IloInt n = i + 1;
					if (S_Res.get(n) == 1)
					{
						Vertice v = G->VerticePorId(i + 1);
						solucaoEncontrada.insert(v);
					}
				}
				result = SOLUCAOENCONTRADA;
			}
			else 
			{				
				result = SOLUCAONAOENCONTRADA;
			}
		}
		catch (IloOplException & e) {
			cout << "### OPL exception: " << e.getMessage() << endl;
		}
		catch (IloException & e) {
			cout << "### exception: ";
			e.print(cout);
		}
		catch (...) {
			cout << "### UNEXPECTED ERROR ..." << endl;
		}

		env.end();

		return result;
	}

	CPLEXResult TSSCPLEX::BuscarSolucao(int tamSolucaoViavel)
	{
		IloEnv env;
		CPLEXResult result = ERRO;
		int status = 127;
		try
		{
			IloOplErrorHandler handler(env, cout);
			std::istringstream in(ObterModelo());
			IloOplModelSource modelSource(env, in, "TSS");
			IloOplSettings settings(env, handler);
			IloOplModelDefinition def(modelSource, settings);
			IloCplex cp(env);
			//cp.setParam(IloCplex::TiLim, 30);
			cp.setParam(IloCplex::MIPDisplay, 3);
			//cp.setParam(IloCplex::TiLim, 60);//*****
			cp.setParam(IloCplex::Threads, 1);
			cp.setParam(IloCplex::BoolParam::CloneLog, false);
			cp.setOut(env.getNullStream());
			IloOplModel opl(def, cp);
			CPLEXDataSource data(env, G, &R, tamSolucaoViavel);
			IloOplDataSource dataSource(&data);
			opl.addDataSource(dataSource);
			opl.generate();

			if (cp.solve())
			{
				IloIntMap S_Res = opl.getElement("S").asIntMap();

				for (int i = 0; i < S_Res.getSize(); i++)
				{
					IloInt n = i + 1;
					if (S_Res.get(n) == 1)
					{
						Vertice v = G->VerticePorId(i + 1);
						solucaoEncontrada.insert(v);
					}
				}
				
				result = SOLUCAOENCONTRADA;
			}
			else 
			{				
				result = SOLUCAONAOENCONTRADA;
			}
		}
		catch (IloOplException & e) {
			cout << "### OPL exception: " << e.getMessage() << endl;
		}
		catch (IloException & e) {
			cout << "### exception: ";
			e.print(cout);			
		}
		catch (...) {
			cout << "### UNEXPECTED ERROR ..." << endl;
		}

		env.end();
		return result;
	}

	CPLEXResult TSSCPLEX::BuscarSolucaoTempo(int tempoMax)
	{
		IloEnv env;
		CPLEXResult result = ERRO;
		try
		{
			IloOplErrorHandler handler(env, cout);
			std::istringstream in(ObterModelo());
			IloOplModelSource modelSource(env, in, "IntervalSet");
			IloOplSettings settings(env, handler);
			IloOplModelDefinition def(modelSource, settings);
			IloCplex cp(env);
			cp.setParam(IloCplex::MIPDisplay, 3);
			cp.setParam(IloCplex::TiLim, tempoMax);
			cp.setParam(IloCplex::Threads, 1);
			cp.setParam(IloCplex::BoolParam::CloneLog, false);
			cp.setOut(env.getNullStream());
			IloOplModel opl(def, cp);
			CPLEXDataSource data(env, G, &R);
			IloOplDataSource dataSource(&data);
			opl.addDataSource(dataSource);
			opl.generate();
			if (cp.solve())
			{
				IloCplex::CplexStatus status = cp.getCplexStatus();
				//cout << status << endl;


				IloIntMap S_Res = opl.getElement("S").asIntMap();
				for (int i = 0; i < S_Res.getSize(); i++)
				{
					IloInt n = i + 1;
					if (S_Res.get(n) == 1)
					{
						Vertice v = G->VerticePorId(i + 1);
						solucaoEncontrada.insert(v);
					}
				}
				if (status == IloCplex::Optimal)
				{
					result = SOLUCAOENCONTRADA;
				}
				else
				{
					result = SOLUCAOINTERROMPIDA;
				}
			}
			else
			{
				result = SOLUCAONAOENCONTRADA;
			}
		}
		catch (IloOplException & e) {
			cout << "### OPL exception: " << e.getMessage() << endl;
		}
		catch (IloException & e) {
			cout << "### exception: ";
			e.print(cout);
		}
		catch (...) {
			cout << "### UNEXPECTED ERROR ..." << endl;
		}
		env.end();
		return result;
	}

	set<Vertice> TSSCPLEX::BuscarSolucao(Grafo * G, VetorRequisitos R, bool & res)
	{
		TSSCPLEX cplex(G, R);
		CPLEXResult r = cplex.BuscarSolucao();
		if (r == SOLUCAOENCONTRADA)
		{
			res = true;
			return cplex.solucaoEncontrada;
		}
		else
		{
			res = false;
			return set<Vertice>();
		}
	}


	//IloCplex::SolveHandle TSSCPLEX::BuscarSolucaoAsync()
	//{
	//	cout << "BUSCAR SOLUCAO ASYNC\n";
	//	try
	//	{
	//		CPLEXDataSource data(env, G, &R);
	//		IloOplDataSource dataSource(&data);
	//		opl.addDataSource(dataSource);
	//		opl.generate();

	//		IloCplex::SolveHandle handle = cp.solve(true);

	//		return handle;
	//	}
	//	catch (IloCplex::Exception e)
	//	{
	//		cout << "EXCEPTION!!!\n" << e.getMessage();
	//	}
	//}

	//set<Vertice> TSSCPLEX::RetornarSolucaoAsync(IloCplex::SolveHandle handle)
	//{
	//	set<Vertice> solucao;
	//	//aborter.abort
	//	if (handle.test())
	//	{
	//		aborter.abort();
	//	}

	//	if (handle.joinSolve())
	//	{
	//		IloIntMap S_Res = opl.getElement("S").asIntMap();
	//		for (int i = 0; i < S_Res.getSize(); i++)
	//		{
	//			IloInt n = i + 1;
	//			if (S_Res.get(n) == 1)
	//			{
	//				Vertice v = G->VerticePorId(i + 1);
	//				solucao.insert(v);
	//			}
	//		}
	//	}
	//	return solucao;
	//}

	TSSCPLEX::~TSSCPLEX()
	{
		//env.end();
	}
}
