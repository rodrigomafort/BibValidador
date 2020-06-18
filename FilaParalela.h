#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	template <class T>
	class FilaParalela
	{
		queue<T> fila;
		condition_variable consumer;
		condition_variable producer;
		atomic_bool fim;
		atomic_ulong size;

		unsigned long maxSize;

	public:
		FilaParalela(unsigned long max = 1000000);
		mutex Hold;
		void Push(T value);
		bool Pop(T& ret);

		void FinalizarAdicao();
		bool AdicaoFinalizada();

		bool Empty();
		unsigned long Size();
	};
}