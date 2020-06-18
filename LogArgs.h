#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class LogArgs
	{
	public:
		string arquivoSaida;
		FilaParalela<string>* log;
		bool encerrarThread;

		bool logThreadEncerrada;

		LogArgs();
		LogArgs(const LogArgs & obj);
		void Inicializar(FilaParalela<string>* l, string nome);
		void Encerrar();
	};
}
