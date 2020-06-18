#include "BibValidador.h"
namespace BibValidador
{

	VDCPLEX::VDCPLEX(Grafo * g, VetorRequisitos r)
	{
		G = g;
		R = r;		
	}

	char* VDCPLEX::ObterModelo() {
		return (char*)"\
		int N = ...;\
		int MAX_S = ...;\
		range V = 1..N;\
		int ADJ[v in V][u in V] = ...;\
		int R[1..N] = ...;\
		dvar int S[v in V] in 0 .. 1;\
		dexpr int custo = sum(v in V) S[v];\
		minimize custo;\
		subject to{\
			custo <= MAX_S;\
			forall(v in V)\
			(S[v] * R[v]) + (sum(u in V) ADJ[v,u] * S[u]) >= R[v];\
		}\
		execute DISPLAY{\
			for (var v in V)\
			writeln(v,\"\t\",S[v]);\
		}\
	";
	}

	bool VDCPLEX::BuscarSolucao()
	{
		IloEnv env;
		bool result = false;
		int status = 127;
		try
		{
			IloOplErrorHandler handler(env, cout);
			std::istringstream in(ObterModelo());
			IloOplModelSource modelSource(env, in, "IntervalSet");
			IloOplSettings settings(env, handler);
			IloOplModelDefinition def(modelSource, settings);
			IloCplex cp(env);
			//cp.setParam(IloCplex::TiLim, 30);
			cp.setParam(IloCplex::MIPDisplay, 3);
			cp.setParam(IloCplex::TiLim, 5);
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
				status = 0;
			}
			else {
				cout << "No solution!" << endl;
				status = 1;
			}
		}
		catch (IloOplException & e) {
			cout << "### OPL exception: " << e.getMessage() << endl;
		}
		catch (IloException & e) {
			cout << "### exception: ";
			e.print(cout);
			status = 2;
		}
		catch (...) {
			cout << "### UNEXPECTED ERROR ..." << endl;
			status = 3;
		}

		env.end();

		return status == 0;
	}

	set<Vertice> VDCPLEX::BuscarSolucao(Grafo * G, VetorRequisitos R, bool & res)
	{
		VDCPLEX cplex(G, R);
		res = cplex.BuscarSolucao();
		if (res == true)
		{
			return set<Vertice>(cplex.solucaoEncontrada);
		}
		else
		{
			return set<Vertice>();
		}
	}

	bool VDCPLEX::BuscarSolucaoInferior(int tamSolucaoViavel)
	{
		IloEnv env;
		bool result = false;
		int status = 127;
		try
		{
			IloOplErrorHandler handler(env, cout);
			std::istringstream in(ObterModelo());
			IloOplModelSource modelSource(env, in, "IntervalSet");
			IloOplSettings settings(env, handler);
			IloOplModelDefinition def(modelSource, settings);
			IloCplex cp(env);
			//cp.setParam(IloCplex::TiLim, 30);
			cp.setParam(IloCplex::MIPDisplay, 3);
			cp.setParam(IloCplex::TiLim, 5);
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
				if (solucaoEncontrada.size() < tamSolucaoViavel)
				{
					result = true;
				}
				status = 0;
			}
			else {
				cout << "No solution!" << endl;
				status = 1;
			}
		}
		catch (IloOplException & e) {
			cout << "### OPL exception: " << e.getMessage() << endl;
		}
		catch (IloException & e) {
			cout << "### exception: ";
			e.print(cout);
			status = 2;
		}
		catch (...) {
			cout << "### UNEXPECTED ERROR ..." << endl;
			status = 3;
		}

		env.end();
		return result;
	}
}
