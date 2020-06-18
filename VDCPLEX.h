#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class VDCPLEX
	{
	public:
		Grafo* G;
		VetorRequisitos R;
		set<Vertice> solucaoEncontrada;
		VDCPLEX(Grafo* g, VetorRequisitos r);
		bool BuscarSolucaoInferior(int tamSolucaoViavel);
		bool BuscarSolucao();

		static set<Vertice> BuscarSolucao(Grafo* G, VetorRequisitos R, bool &res);

	private:
		static char* ObterModelo();
	};
}
