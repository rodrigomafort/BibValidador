#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class VDBackTracking
	{
		private:
			Grafo* G;
			VetorRequisitos R;

			bool* vetSol;
			int minSize;

			void Recursao(int vId);
		public:
			set<set<Vertice>> solucoesEncontradas;
			VDBackTracking(Grafo* g, VetorRequisitos r);
			bool BuscarSolucoesInferiores(int tamSolucaoViavel);
			bool BuscarSolucoes();

			static set<Vertice> BuscarSolucao(Grafo* G, VetorRequisitos R, bool &res);

	};
}

