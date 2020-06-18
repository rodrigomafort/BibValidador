#pragma once
#include "BibGrafos.h"
using namespace BibGrafos;

namespace BibValidador
{
	typedef set<Vertice>(FuncaoExecutora)(Grafo* G, VetorRequisitos R, string &info, void* par);
	typedef bool(FuncaoAvaliadora)(Grafo* G, VetorRequisitos R, set<Vertice> S);
	typedef set<Vertice>(FuncaoCertificadora)(Grafo* G, VetorRequisitos R, bool &res);
}
