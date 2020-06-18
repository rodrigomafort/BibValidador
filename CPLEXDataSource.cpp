#include "BibValidador.h"
#pragma once

namespace BibValidador
{

	ILOSTLBEGIN
	void CPLEXDataSource::read() const
	{
		IloOplDataHandler handler = getDataHandler();
		handler.startElement("N");
		handler.addIntItem(G->N());
		handler.endElement();

		handler.startElement("MAX_S");
		handler.addIntItem(maxSize);
		handler.endElement();

		handler.startElement("ADJ");
		handler.startArray();


		for (int i = 1; i <= G->N(); i++)
		{
			Vertice a = G->VerticePorId(i);
			handler.startArray();
			for (int j = 1; j <= G->N(); j++)
			{
				Vertice b = G->VerticePorId(j);
				if (G->AdjList[a].count(b))
				{
					handler.addIntItem(1);
				}
				else
				{
					handler.addIntItem(0);
				}
			}
			handler.endArray();
		}
		handler.endArray();
		handler.endElement();

		handler.startElement("R");
		handler.startArray();
		for (int i = 1; i <= G->N(); i++)
		{
			int fv = R->ObterRequisito(G->VerticePorId(i));
			handler.addIntItem(fv);
		}
		handler.endArray();
		handler.endElement();
	}

	CPLEXDataSource::CPLEXDataSource(IloEnv& env, Grafo* g, VetorRequisitos* r) : IloOplDataSourceBaseI(env)
	{
		G = g;
		maxSize = G->N();
		R = r;
	};

	CPLEXDataSource::CPLEXDataSource(IloEnv& env, Grafo* g, VetorRequisitos* r, int max) : IloOplDataSourceBaseI(env)
	{
		G = g;
		maxSize = max;
		R = r;
	};
}
