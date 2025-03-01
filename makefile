default:
	gcc -o graphColoring.out -O3 main.c dsatur.c graph_constructor.c imp_color.c

clean:
	rm -f graphColoring.out


benchmark:
	gcc benchmark.cpp -o benchmark.out -O3
