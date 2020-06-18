#include "BibValidador.h"
namespace BibValidador
{
	void LogArgs::Inicializar(FilaParalela<string>* l, string nome)
	{
		arquivoSaida = nome;
		log = l;
		encerrarThread = false;
		logThreadEncerrada = false;
	}

	LogArgs::LogArgs()
	{
		encerrarThread = false;
		logThreadEncerrada = false;
	}

	void LogArgs::Encerrar()
	{
		log->FinalizarAdicao();
		encerrarThread = false;
	}

	LogArgs::LogArgs(const LogArgs & obj)
	{
		encerrarThread = false;
		logThreadEncerrada = false;
	}
}
