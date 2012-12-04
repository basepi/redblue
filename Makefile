seq:
	g++ redblue_seq.cpp -O3 -o seq -Wall
	
omp:
	CC redblue_omp.cpp -xopenmp -O3 -o omp -Wall

pthread:
	g++ redblue_pthread.cpp -lpthread -O3 -o pthread -Wall
	
clean:
	rm seq -f
	rm omp -f
	rm pthread -f