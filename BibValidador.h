#pragma once
#include <stdio.h>
#include<iostream>
using namespace std;

#pragma warning(disable: 4996)
#pragma warning(disable: 4661)
#pragma warning(disable: 4267)
#pragma warning(disable: 4244)

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <list>
#include <queue>
#include <string>
#include <random>
#include <algorithm>
#include <iterator>
#include <climits>
#include <chrono>
#include <atomic>
#include <thread>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream> 
#include <condition_variable>
#include <mutex>
#include<iomanip>

#include <ilopl/iloopl.h>
#include <sstream>

#ifndef DATADIR
#ifdef ILO_WINDOWS
#define DIRSEP "\\"
#else
#define DIRSEP "/"
#endif
#define DATADIR ".." DIRSEP ".."  DIRSEP ".." DIRSEP ".." DIRSEP "opl" DIRSEP
#endif

#include "BibGrafos.h"

using namespace std;
using namespace BibGrafos;

#include "Validador.h"
#include "FilaParalela.h"

#include "CPLEXDataSource.h"
#include "VDCPLEX.h"
#include "TSSCPLEX.h"

#include "VDBackTrackingLimitado.h"
#include "VDBackTracking.h"

#include "TSSBackTracking.h"

#include "LogArgs.h"
#include "ThreadPars.h"

#include "ValidadorAlgoritmosRandomico.h"
#include "ValidadorAlgoritmosExaustivo.h"
#include "ValidadorMetaheuristicaRandomico.h"

template class BibValidador::FilaParalela<BibValidador::ThreadPars>;
template class BibValidador::FilaParalela<BibValidador::ThreadPars*>;
template class BibValidador::FilaParalela<string>;

