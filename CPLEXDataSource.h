#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class CPLEXDataSource : public IloOplDataSourceBaseI {
		Grafo* G;
		VetorRequisitos* R;
		int maxSize;
	public:
		CPLEXDataSource(IloEnv& env, Grafo* G, VetorRequisitos* R);
		CPLEXDataSource(IloEnv& env, Grafo* G, VetorRequisitos* R, int max);
		void read() const;
	};
}
