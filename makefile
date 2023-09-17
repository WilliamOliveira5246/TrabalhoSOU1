all: auxiliar sequencial

auxiliar:
	gcc auxiliar.c -o auxiliar
	./auxiliar

sequencial:
	gcc sequencial.c -o sequencial
	./sequencial matrizA.txt matrizB.txt

processos:
	gcc processos.c -o processos
	./processos matrizA.txt matrizB.txt

clean:
	del auxiliar.exe
	del sequencial.exe
#linux
#rm -f auxiliar
#rm -f sequencial
