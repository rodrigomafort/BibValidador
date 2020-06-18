#include "BibValidador.h"
namespace BibValidador
{

	template <class T>
	FilaParalela<T>::FilaParalela(unsigned long max)
	{
		fim = false;
		size = 0;
		maxSize = max;
	}

	template <class T>
	void FilaParalela<T>::Push(T value)
	{
		std::unique_lock<std::mutex> lock(Hold);

		while (size >= maxSize)
		{
			producer.wait(lock);
		}

		fila.push(value);
		size = size + 1;

		lock.unlock();
		consumer.notify_one();
	}

	template <class T>
	bool FilaParalela<T>::Pop(T& ret)
	{

		std::unique_lock<std::mutex> lock(Hold);
		while (fila.empty() && fim == false)
		{
			consumer.wait_for(lock, std::chrono::milliseconds(500));
		}

		if (fim == true && fila.empty())
		{
			lock.unlock();
			producer.notify_one();
			return false;
		}
		else
		{
			ret = fila.front();
			fila.pop();
			size = size - 1;
			lock.unlock();
			producer.notify_one();
			return true;
		}
	}


	template <class T>
	bool FilaParalela<T>::Empty()
	{
		return size == 0;
	}

	template <class T>
	unsigned long FilaParalela<T>::Size()
	{
		return size;
	}



	template <class T>
	void FilaParalela<T>::FinalizarAdicao()
	{
		fim = true;
	}

	template <class T>
	bool FilaParalela<T>::AdicaoFinalizada()
	{
		return fim;
	}
}
