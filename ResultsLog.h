#include "BibValidador.h"
#pragma once

namespace BibValidador
{
	class ResultsLog
	{
		atomic_flag lock_stream = ATOMIC_FLAG_INIT;
		stringstream stream;

	public:
		void appendLog(string log) {
			while (lock_stream.test_and_set()) {}
			stream << log;
			lock_stream.clear();
		}

		string flushContent()
		{
			while (lock_stream.test_and_set()) {}
			string result = stream.str();
			stream.str("");
			lock_stream.clear();
			return result;
		}
	};
}