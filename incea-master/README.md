This repository is for sharing synthetic benchmark instances and 
binary file of InCEA algorithm. 

This study is submitted to IEEE Open Access Journal.

**USAGE OF THE BENCHMARK INSTANCES**

We created 150 benchmark instances synthetically. Each instance is named as
"INCEA%NodeNumber.%EdgeDensity.%GraphNumber.col".

There are 5 different number of vertexes N = {100, 200, 300, 400, 500} and 
6 different edge density values 10%, 25%, 45%, 60%, 75% and 90%.

For each N and each edge density value, we generated 5 different graph instances. 

The file format of InCEA graphs is as follows. 

*  The lines which start with "e", represent an edge between two vertexes and the 
line is written "e   vertex1   vertex2".

*  The lines which start with "n" represent weight value of the related vertex. 
The lines are sorted with from the first vertex to the last vertex and it is written 
as "n   vertexId    weight value".

* The minimum vertex Id must be 1. 

* Example: Assume that there are 5 vertices in the graph with their edges, then the file
can be like:

e   1   2 

e   1   3

e   1   4

e   2   4

e   3   5

n   1   2

n   2   3

n   3   1

n   4   4

n   5   1

Hence the file name is INCEA5.50.1.col since there are 5 vertices, 50% edge density and the first version of this type of graph given.

**USAGE OF InCEA ALGORITHM**

After the binary file is downloded, the file should be converted to an executable
file. For instance for Linux, "chmod 777 incea".

The program can be executed with 5 input arguments as follows.

./incea InputFile N Color IterationSize OutputFile

*  InputFile: Name of benchmark instance file that you want to solve
*  N: Number of vertexes in the benchmark instance that you give for InputFile
*  Color: Number of colors that you want to use for solving the benchmark.
*  IterationSize: How many iterations does InCEA need for the given graph (set between 1000 and 30000).
*  OutputFile: The output file where the solution will be written with 
its representation, fitness value and the set of the uncolor vertices.

Example command: ./incea instances/INCEA100.10.1.col 100 4 1000 INCEA_Output.txt