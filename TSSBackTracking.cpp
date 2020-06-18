#include "BibValidador.h"
#include "TSSBackTracking.h"

TSSBackTracking::TSSBackTracking(Grafo* pG, VetorRequisitos pR): R(pR)
{
	G = new Grafo(pG);
	n = G->N();
	resposta = false;
}

void TSSBackTracking::Recursao(set<Vertice> NDom, vector<int> VetNR, vector<bool> MapaDom, vector<set<Vertice>> N, set<Vertice> S, int maxid, int tamMax)
{
	if (S.size() > tamMax)
	{
		return;
	}
	else
	{
		if (NDom.size() == 0)
		{
			//S domina o grafo
			if (S.size() < solucaoEncontrada.size() || resposta == false)
			{
				solucaoEncontrada = set<Vertice>(S);
				resposta = true;
			}
			return;
		}
		else
		{
			for (Vertice v : NDom)
			{
				if (v.Id() > maxid)
				{
					set<Vertice> S2(S);
					set<Vertice> NDom2(NDom);
					vector<int> VetNR2(VetNR);
					vector<bool> MapaDom2(MapaDom);
					vector<set<Vertice>> N2(N);

					queue<Vertice> fila;
					S2.insert(v);

					fila.push(v);
					MapaDom2[v.Indice()] = true;

					while (fila.empty() == false)
					{
						Vertice u = fila.front();
						fila.pop();

						for (Vertice z : N2[u.Indice()])
						{
							VetNR2[z.Indice()] = VetNR2[z.Indice()] + 1;
							if (VetNR2[z.Indice()] >= R[z] && MapaDom2[z.Indice()] == false)
							{
								fila.push(z);
								MapaDom2[z.Indice()] = true;
							}
							N2[z.Indice()].erase(u);
						}
						N2[u.Indice()].clear();

						NDom2.erase(u);
					}

					Recursao(NDom2, VetNR2, MapaDom2, N2, S2, v.Id(), tamMax);
				}
			}
		}
	}
}

bool TSSBackTracking::Executar(int tamMax)
{
	set<Vertice> S;
	set<Vertice> NDom(G->Vertices);
	vector<int> VetNR;
	vector<bool> MapaDom;
	vector<set<Vertice>> N;

	VetNR.resize(n,0);
	MapaDom.resize(n,false);
	N.resize(n);
	for (Vertice v: G->Vertices)
	{
		N[v.Indice()] = G->Adjacentes(v);
	}

	resposta = false;
	solucaoEncontrada = set<Vertice>();

	Recursao(NDom, VetNR, MapaDom, N, S, -1, tamMax);

	return resposta;
}
