#include <tuple>
#include <algorithm>
#include <map>

#include "params.h"

#define INFINITY 30000
#define INITIAL nullptr
#define NOOBJECT nullptr

typedef std::map<const char *, int> Hash;

void assert(bool __expr, const char *__str__ = "", Hash params = Hash()) {
	if(!__expr)	{
		fprintf(stderr, "\x1b[1;31mRuntime error: \x1b[0m");
		fprintf(stderr, "%s\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
		exit(1);
	}
}

void refute(bool __expr, const char *__str__ = "", Hash params = Hash()) {
	if(__expr)	{
		fprintf(stderr, "\x1b[1;31mRuntime error: \x1b[0m");
		fprintf(stderr, "%s\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
		exit(1);
	}
}

void note(bool __expr, const char *__str__ = "",  Hash params = Hash()) {
	if(!__expr)	
	{
		fprintf(stderr, "\x1b[1;33mNote: \x1b[0m");
		fprintf(stderr, "%s\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
	}
}

class Edge;

class Vertex
{
	Vertex *message_sender;		// От кого было получен кратчайший путь
	vector<Edge *> edges;		// Исходящие ребра
	int shortest_path;		// Величина этого пути
	bool visited;
	void receive_message(Vertex *sender, int size)
	{
		assert(visited == false, "Trying to send message to a visited vertex", 
			Hash{ {"Visited vertex", rank} , {"Sender", sender -> rank}, {"Message size", size}}
		);
		if(size < shortest_path)
		{
			shortest_path = size;
			message_sender = sender;
		}
	}
public:
	const int rank;	// номер вершины в графе

	Vertex(int _rank);
	void add_edge(Edge *_edge);
	void send_messages();
	void trace_path(int depth = 0);

	int get_shortest_path() { return shortest_path; }
	void set_shortest_path(int _shp) { shortest_path = _shp; }
	bool get_visited() { return visited; }
	void set_visited(bool _visited) { visited = _visited; }
	Vertex *ptr() { return this; }
};



class Edge
{
public:
	Vertex *origin, *target;	// указатели на начальную и конечную вершины
	const int length;	// его длина
	Edge(Vertex *start, Vertex *end, int _length): origin{start}, target{end}, length{_length}
	{
		assert(start != NOOBJECT && end != NOOBJECT, "Trying to add edge with NOOBJECT as one of the ends!", Hash{ {"Length", length} });
		assert(_length < INFINITY, "Trying to add an edge with length over infinity!", Hash{ {"Length", length} });
		assert(_length > 0, "Trying to add an edge with length below of = zero!", Hash{ {"Length", length} });
		note(start == end, "A cycle was added to graph! Did you really mean that?", Hash{ {"Vertex rank where the cycle was found", start -> rank} });
		note(length == 0, "A zero length edge was added to graph!", Hash{ {"Origin", start->rank}, {"Target", end->rank} });

		// записываем в начальную вершину адрес текущего объекта - нового ребра
		origin -> add_edge(this);	
	}
	Edge *ptr() { return this; }
};

Vertex::Vertex(int _rank): message_sender(INITIAL), edges(), shortest_path(INFINITY), visited(false), rank{_rank}
{
	assert(_rank > 0, "Rank is less than zero!", Hash{ {"Rank", rank} });
}
void Vertex::add_edge(Edge *_edge)
{
	assert(_edge != NOOBJECT, "Attempt to assign NOOBJECT as an edge!", Hash{ {"Vertex rank", rank} });
	for(const auto &edge: edges)
		assert(
			edge -> target != _edge -> target, 
			"Multiple edges lead to one vertex!", 
			Hash{ {"Current vertex rank", rank}, {"Leads to", _edge -> target -> rank} }
		);

	// в список ребер добавляем новую ссылку
	edges.push_back(_edge);
}
void Vertex::send_messages()
{
	if(edges.size() == 0)
		return;
	note(shortest_path == INFINITY, "Sending message of infinite length", Hash{ {"Length", shortest_path} });
	for(auto &edge:edges)
		if(!edge -> target -> get_visited())
			edge -> target -> receive_message(this, edge -> length + shortest_path);
	visited = true;
}
void Vertex::trace_path(int depth)
{
	if(message_sender == INITIAL && depth == 0)
		printf("v%d - Single\n", rank);
	else if(message_sender == INITIAL && depth > 0)
		printf("v%d => ", rank);
	else if(depth == 0)
	{
		message_sender -> trace_path(depth+1);
		printf("v%d - %d\n", rank, shortest_path);
	}
	else
	{
		message_sender -> trace_path(depth+1);
		printf("v%d => ", rank);
	}
}

class Graph
{
	vector<Vertex> vertices;
	vector<Edge> edges;

	Vertex *vertex_pointer(int rank)
	{
		assert(vertex_exists(rank), "Attempt to get a vertex that does not exists!", Hash{ {"Vertex rank", rank} });

		// поиск вершины с номером rank
		auto result = std::find_if(vertices.begin(), vertices.end(), [&](const Vertex & o) {
    		return o.rank == rank;
		});
		// функция поиска возвращает итератор, поэтому *
		return (*result).ptr();
	}
	Vertex *get_next_vertex()
	{
		// поиск вершины, которую еще не посетили и с минимальным весом
		int min = INFINITY;
		Vertex *result = NOOBJECT;
		for(auto &vertex: vertices)
			if(!vertex.get_visited() && vertex.get_shortest_path() < min)
				result = vertex.ptr();
		return result;
	}
	bool vertex_exists(int rank)
	{
		auto result = std::find(vertices.begin(), vertices.end(), [&](const Vertex & o) {
    		return o.rank == rank;
		});
	 	return result != std::end(vertices);
	}
	bool vertices_left(int rank)
	{
		return get_next_vertex() != NOOBJECT;
	}
	Vertex *create_vertex(int rank)
	{
		refute(vertex_exists(rank), "Vertex already exists", Hash{ {"Vertex", rank} });

		// записываем в массив саму вершину
		vertices.push_back(Vertex(rank));

		// возвращаем указатель на нее
		return vertices.back().ptr();
	}
	Edge *create_edge(Vertex *start, Vertex *finish, int length)
	{
		// записать новое ребро в массив
		edges.push_back(Edge(start, finish, length));

		// добавить указатель на ребро в стартовую вершину
		start -> add_edge(edges.back().ptr());
		return edges.back().ptr();
	}
	void add_path(int origin_index, int target_index, int edge_length)
	{
		refute(vertex_exists(origin_index) && vertex_exists(target_index) && !oriented, "Trying to add multiple edges", 
			Hash{ {"Origin", origin_index}, {"Target", target_index}, {"Length", edge_length} }
		);
		Vertex *origin = create_vertex(origin_index);
		Vertex *target = create_vertex(target_index);
		// (*origin).add_edge() - должен сделать внутри сам
		Edge *edge = create_edge(origin, target, edge_length);
		if(!oriented)
			edge = create_edge(target, origin, edge_length);
	}
public:
	Graph() 
	{
		assert(G.size() >= 3, "Graph is empty!");
		assert(G.size() % 3 == 0, "Wrong graph format!");
		for(int i = 0; i < G.size(); i+=3)
		{
			int start = G[i], finish = G[i+1], length = G[i+2];
			add_path(start, finish, length);

		}
	}
	void dijkstra_alg(int origin, int target)
	{
		assert(vertex_exists(origin) && vertex_exists(target), "Vertex does not exist!", Hash{ {"Origin", origin}, {"Target", target} });
		if(origin == target)
		Vertex *current_vertex = vertex_pointer(origin);
		current_vertex -> set_shortest_path(0);
		while(vertices_left())
		{
			current_vertex -> send_messages();
			current_vertex = get_next_vertex();
		}
		if(show_all)
			for(const auto &vertex:vertices)
				if(vertex.rank != origin && vertex.rank != target)
					vertex.trace_path();
		{
			printf("\n\nResult:\n");
			vertex_pointer(target) -> trace_path();
		}
	}
};

int main()
{
	Graph gr();
	gr.dijkstra_alg(start_point, end_point);
}