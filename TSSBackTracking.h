#pragma once
#include "BibValidador.h"

class TSSBackTracking
{
	Grafo* G;
	VetorRequisitos R;
	bool resposta;
	int n;

	void Recursao(set<Vertice> NDom, vector<int> VetNR, vector<bool> MapaDom, vector<set<Vertice>> N, set<Vertice> S, int maxid, int tamMax);
public:
	TSSBackTracking(Grafo* G, VetorRequisitos R);
	bool Executar(int tamMax);

	bool ExecutarMPI(int tamMax, int id, int cores);


	set<Vertice> solucaoEncontrada;
};
