#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class VDBackTrackingLimitado
	{
	public:
		Grafo* G;
		VetorRequisitos R;
		vector<Vertice> solucaoEncontrada;
		VDBackTrackingLimitado(Grafo* g, VetorRequisitos r);
		bool BuscarSolucaoInferior(int tamSolucaoViavel);
	private:
		bool Recursao(map<Vertice, bool> &S, int pos, int tamSolucaoViavel);
		bool AvaliarSolucao(map<Vertice, bool> vet);
		vector<Vertice> ObterConjunto(map<Vertice, bool> S);
	};
}