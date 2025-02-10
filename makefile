default:
	gcc -o graphColoring -O3 main.c dsatur.c enhanced_dsatur.c graph_constructor.c

clean:
	rm -f graphColoring



