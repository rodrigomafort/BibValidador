CPPFLAGS = -w -std=gnu++0x -O3 -mfpmath=sse -march=native -g -m64 -fexceptions -ftree-vectorize -I/opt/ibm/ILOG/CPLEX_Studio129/opl/include -I ../BibGrafos -DILOUSEMT -D_REENTRANT -DIL_STD
COMP = g++

TARGET = libBibValidador.a
OBJ = BibValidador.o Validador.o FilaParalela.o LogArgs.o ThreadPars.o ValidadorAlgoritmosExaustivo.o ValidadorAlgoritmosRandomico.o ValidadorMetaheuristicaRandomico.o VDBackTrackingLimitado.o CPLEXDataSource.o VDCPLEX.o TSSCPLEX.o VDBackTracking.o TSSBackTracking.o

all: $(TARGET)

$(TARGET): $(OBJ) 
	ar rcs $@ $(OBJ)

Validador.o: Validador.cpp Validador.h
	$(COMP) $(CPPFLAGS) -c $< 

FilaParalela.o: FilaParalela.cpp FilaParalela.h
	$(COMP) $(CPPFLAGS) -c $< 

LogArgs.o: LogArgs.cpp LogArgs.h FilaParalela.o
	$(COMP) $(CPPFLAGS) -c $< 

ThreadPars.o: ThreadPars.cpp ThreadPars.h FilaParalela.o
	$(COMP) $(CPPFLAGS) -c $< 

ValidadorAlgoritmosExaustivo.o: ValidadorAlgoritmosExaustivo.cpp ValidadorAlgoritmosExaustivo.h FilaParalela.o Validador.o
	$(COMP) $(CPPFLAGS) -c $< 

ValidadorAlgoritmosRandomico.o: ValidadorAlgoritmosRandomico.cpp ValidadorAlgoritmosRandomico.h FilaParalela.o Validador.o
	$(COMP) $(CPPFLAGS) -c $< 

ValidadorMetaheuristicaRandomico.o: ValidadorMetaheuristicaRandomico.cpp ValidadorMetaheuristicaRandomico.h FilaParalela.o Validador.o
	$(COMP) $(CPPFLAGS) -c $< 

VDBackTrackingLimitado.o: VDBackTrackingLimitado.cpp VDBackTrackingLimitado.h
	$(COMP) $(CPPFLAGS) -c $< 

CPLEXDataSource.o: CPLEXDataSource.cpp CPLEXDataSource.h
	$(COMP) $(CPPFLAGS) -c $< 

VDCPLEX.o: VDCPLEX.cpp VDCPLEX.h CPLEXDataSource.o
	$(COMP) $(CPPFLAGS) -c $< 

TSSCPLEX.o: TSSCPLEX.cpp TSSCPLEX.h CPLEXDataSource.o
	$(COMP) $(CPPFLAGS) -c $< 

VDBackTracking.o: VDBackTracking.cpp VDBackTracking.h
	$(COMP) $(CPPFLAGS) -c $< 
	
TSSBackTracking.o: TSSBackTracking.cpp TSSBackTracking.h
	$(COMP) $(CPPFLAGS) -c $<

BibValidador.o: BibValidador.cpp BibValidador.h Validador.o FilaParalela.o LogArgs.o ThreadPars.o ValidadorAlgoritmosExaustivo.o ValidadorAlgoritmosRandomico.o ValidadorMetaheuristicaRandomico.o VDBackTrackingLimitado.o CPLEXDataSource.o VDCPLEX.o TSSCPLEX.o VDBackTracking.o TSSBackTracking.o
	$(COMP) $(CPPFLAGS) -c $< 


clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: libBibGrafos.a
libBibGrafos.a:
	$(MAKE) -C ../BibGrafos/