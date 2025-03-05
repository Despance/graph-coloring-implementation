import networkx as nx
import matplotlib.pyplot as plt
import argparse

def read_graph_file(graph_file_path):
    """
    Reads a graph file in the specified format and returns a networkx graph.

    Args:
        graph_file_path (str): Path to the graph file.

    Returns:
        networkx.Graph: A networkx graph object.
    """
    graph = nx.Graph()
    with open(graph_file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('p edge'):
                parts = line.split()
                num_nodes = int(parts[2])
                graph.add_nodes_from(range(1, num_nodes + 1)) # Nodes are numbered from 1 to num_nodes
            elif line.startswith('e'):
                parts = line.split()
                u, v = int(parts[1]), int(parts[2])
                graph.add_edge(u, v)
    return graph

def read_solution_file(solution_file_path):
    """
    Reads a solution file and returns a list of colors for each node.

    Args:
        solution_file_path (str): Path to the solution file.

    Returns:
        list: A list of colors, where index i is the color of node i+1.
              Returns None if the file cannot be read or is empty.
    """
    colors = []
    try:
        with open(solution_file_path, 'r') as f:
            for line in f:
                line = line.strip()
                if line: # Ignore empty lines
                    colors.append(int(line))
        return colors
    except FileNotFoundError:
        print(f"Solution file not found: {solution_file_path}")
        return None
    except ValueError:
        print(f"Invalid solution file format. Each line should contain an integer color.")
        return None

def visualize_graph(graph, colors=None, layout='spring'):
    """
    Visualizes the graph with optional coloring.

    Args:
        graph (networkx.Graph): The graph to visualize.
        colors (list, optional): A list of colors for each node. Defaults to None.
        layout (str, optional): Layout algorithm for graph visualization ('spring', 'circular', 'kamada_kawai', 'spectral', 'planar'). Defaults to 'spring'.
    """
    plt.figure(figsize=(12, 12)) # Adjust figure size for better visualization

    layout_options = {
        'spring': nx.spring_layout,
        'circular': nx.circular_layout,
        'kamada_kawai': nx.kamada_kawai_layout,
        'spectral': nx.spectral_layout,
        'planar': nx.planar_layout, # planar_layout requires that the graph is planar
    }

    if layout not in layout_options:
        print(f"Warning: Layout '{layout}' is not recognized. Using default 'spring' layout.")
        layout_function = layout_options['spring']
    else:
        layout_function = layout_options[layout]

    pos = layout_function(graph) # Calculate layout positions

    if colors:
        # Ensure the number of colors matches the number of nodes
        if len(colors) != len(graph.nodes):
            print("Warning: Number of colors in solution file does not match the number of nodes. Ignoring colors.")
            nx.draw(graph, pos, with_labels=True, node_size=500, node_color='skyblue', font_size=10)
        else:
            nx.draw(graph, pos, with_labels=True, node_color=colors, cmap=plt.cm.viridis, node_size=500, font_size=10) # Use viridis colormap
    else:
        nx.draw(graph, pos, with_labels=True, node_size=500, node_color='skyblue', font_size=10)

    plt.title("Graph Visualization")
    plt.show()


def main():
    parser = argparse.ArgumentParser(description="Visualize a graph from a file with optional coloring.")
    parser.add_argument("graph_file", help="Path to the graph file.")
    parser.add_argument("-s", "--solution_file", help="Path to the optional solution file for coloring.", required=False)
    parser.add_argument("-l", "--layout", help="Layout algorithm to use for visualization (spring, circular, kamada_kawai, spectral, planar). Default is spring.", default='spring')

    args = parser.parse_args()

    graph_file_path = args.graph_file
    solution_file_path = args.solution_file
    layout_option = args.layout

    graph = read_graph_file(graph_file_path)
    colors = None
    if solution_file_path:
        colors = read_solution_file(solution_file_path)

    visualize_graph(graph, colors, layout_option)

if __name__ == "__main__":
    main()