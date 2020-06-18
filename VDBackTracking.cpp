#include "BibValidador.h"

namespace BibValidador
{
	VDBackTracking::VDBackTracking(Grafo* g, VetorRequisitos r)
	{
		G = g;
		R = r;

		vetSol = NULL;
		minSize = G->N();
	}

	bool VDBackTracking::BuscarSolucoesInferiores(int tamSolucaoViavel)
	{
		return false;
	}

	bool VDBackTracking::BuscarSolucoes()
	{
		solucoesEncontradas.clear();
		if (vetSol != NULL) free(vetSol);
		vetSol = (bool*) malloc (G->N() * sizeof(bool));
		for (int i = 0; i < G->N(); i++) vetSol[i] = false;

		Recursao(1);
		if (solucoesEncontradas.size() > 0) return true;
		else return false;
	}

	void VDBackTracking::Recursao(int vId)
	{
		if (vId > G->N())
		{
			//Avaliar se vetSol é solução
			set<Vertice> S;
			for (Vertice v : G->Vertices)
				if (vetSol[v.Indice()])
					S.insert(v);

			if (R.VDAvaliarDominacao(S,G))
			{
				//Em caso afirmativo, verificar se o tamanho:
				int minSize = (*solucoesEncontradas.begin()).size();
				if (S.size() < minSize || solucoesEncontradas.size() == 0)
				{
					//Se menor do que as soluções em solucoesEncontradas -> limpar solucoesEncontradas e inserir a nova
					solucoesEncontradas.clear();
					solucoesEncontradas.insert(S);
				}
				else
				{
					if (S.size() == minSize)
					{
						//Se igual -> adicionar a solucoesEncontradas
						solucoesEncontradas.insert(S);
					}
					//Se maior -> ignorar
				}
			}
		}
		else
		{
			vetSol[vId-1] = false;
			Recursao(vId+1);
			vetSol[vId-1] = true;
			Recursao(vId+1);
			vetSol[vId-1] = false;
		}
	}

	set<Vertice> VDBackTracking::BuscarSolucao(Grafo* G, VetorRequisitos R, bool &res)
	{
		VDBackTracking VD(G,R);
		if (VD.BuscarSolucoes())
		{
			res = true;
			set<Vertice> S = *(VD.solucoesEncontradas.begin());
			return S;
		}
		else
		{
			res = false;
			return set<Vertice>();
		}
	}

}
