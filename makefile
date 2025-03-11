default:
	gcc -o graphColoring.out -O3 main.c dsatur.c enhanced_dsatur.c graph_constructor.c

clean:
	rm -f graphColoring.out


benchmark:
	gcc -pg benchmark.cpp -o benchmark.out -O3
