#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	enum CPLEXResult { SOLUCAOENCONTRADA, SOLUCAOINTERROMPIDA, SOLUCAONAOENCONTRADA, ERRO };
	class TSSCPLEX
	{
		/*IloEnv env;
		IloOplErrorHandler handler;
		ModeloCPLEX modelo;
		std::istringstream streamModelo;
		IloOplModelSource modelSource;
		IloOplSettings settings;
		IloOplModelDefinition def;
		IloCplex cp;
		IloOplModel opl;
		IloCplex::Aborter aborter;*/
	public:
		Grafo* G;
		VetorRequisitos R;
		set<Vertice> solucaoEncontrada;
		TSSCPLEX(Grafo* g, VetorRequisitos R);
		~TSSCPLEX();

		CPLEXResult BuscarSolucao(int tamSolucaoViavel);
		CPLEXResult BuscarSolucao();		
		CPLEXResult BuscarSolucaoTempo(int tempoMax);

		IloCplex::SolveHandle BuscarSolucaoAsync();
		set<Vertice> RetornarSolucaoAsync(IloCplex::SolveHandle handle);

		static set<Vertice> BuscarSolucao(Grafo* G, VetorRequisitos R, bool &res);

	private:
		static const char* ObterModelo();
	};
}
