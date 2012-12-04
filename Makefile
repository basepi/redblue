seq:
	g++ redblue_seq.cpp -O3 -o seq -Wall
	
omp:
	g++ redblue_omp.cpp -xopenmp -O3 -o omp -Wall

pthread:
	g++ redblue_pthread.cpp -lpthread -O3 -o pthread -Wall
	
clean:
	rm seq
	rm omp
	rm pthread
