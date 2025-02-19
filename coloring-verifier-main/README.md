# Graph Vertex-Coloring Verifier

Coloring-verifier is a small program to verify solutions to graph vertex-coloring problems in
the context of the DIMACS Computational Challenges.  

Two input files are
required: the instance file in the ASCII DIMACS file format (see
http://mat.gsia.cmu.edu/COLOR04/ for details), and the solution file.

The solution file has to be an ASCII file consisting of a single column
of numbers. Each number represents the color assigned to the node
numbered as the corresponding row number. Both colors and nodes are
assumed to start at 1. 

A directory called "examples" is included with an
example of instance and two solutions: one feasible and another
infeasible for the classical graph vertex-coloring problem.

In case a solution is not feasible, the program returns the number of
constraint violations. This number is computed as number of conflicting
edges, that is, edges with ending nodes having the same
color. Furthermore, it also returns the number of nodes involved in at
least one conflict.

The verifier can be used for the following problems:

1. graph vertex-coloring 
2. list coloring
3. set coloring
4. T coloring
5. set-T-coloring
6. interval coloring
7. sum coloring.


## BUILDING

The code has been tested on Linux and MacOsX operating systems. For
compiling the source code in both environments a call to the
Makefile by the `make` command should suffice.



See `coloring-verifier --help` for more details.


---
Marco Chiarandini, 
University of Southern Denmark
email: marco@imada.sdu.dk