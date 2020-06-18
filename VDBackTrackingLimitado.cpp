#include "BibValidador.h"
namespace BibValidador
{
	VDBackTrackingLimitado::VDBackTrackingLimitado(Grafo* g, VetorRequisitos r)
	{
		G = g;
		R = r;
	}


	bool VDBackTrackingLimitado::BuscarSolucaoInferior(int tamSolucaoViavel)
	{
		vector<Vertice> sol;
		solucaoEncontrada = sol;

		map<Vertice, bool> S;

		//Preprocessamento: Todos os vértices cujo threshold seja maior do que o número de vizinhos são adicionados ao conjunto
		for (Vertice v : G->Vertices)
		{
			if (R[v] > G->Grau(v))
			{
				S.insert(pair<Vertice, int>(v, 1));
			}
			else
			{
				S.insert(pair<Vertice, int>(v, 0));
			}
		}
		return Recursao(S, 1, tamSolucaoViavel);
	}

	bool VDBackTrackingLimitado::AvaliarSolucao(map<Vertice, bool> vet)
	{
		vector<Vertice> sol;
		for (pair<Vertice, bool> el : vet)
		{
			if (el.second)
			{
				sol.push_back(el.first);
			}
		}

		return R.VDAvaliarDominacao(sol, G);
	}

	vector<Vertice> VDBackTrackingLimitado::ObterConjunto(map<Vertice, bool> S)
	{
		vector<Vertice> conjunto;
		for (pair<Vertice, bool> par : S)
		{
			if (par.second) conjunto.push_back(par.first);
		}
		return conjunto;
	}

	bool VDBackTrackingLimitado::Recursao(map<Vertice, bool> &S, int i, int tamSolucaoViavel)
	{
		if (ObterConjunto(S).size() >= tamSolucaoViavel || i > G->N())
		{
			return false;
		}
		else
		{
			if (AvaliarSolucao(S))
			{
				//Vet é solução
				solucaoEncontrada = ObterConjunto(S);
				return true;
			}
			else
			{
				//Testar casos em que V[i] pertence e não pertence a solução
				Vertice v = G->VerticePorId(i);
				S[v] = true;
				if (Recursao(S, i + 1, tamSolucaoViavel))
				{
					return true;
				}
				else
				{
					S[v] = false;
					return Recursao(S, i + 1, tamSolucaoViavel);
				}
			}
		}

	}
}