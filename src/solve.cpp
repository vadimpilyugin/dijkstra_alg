#include <tuple>
#include <algorithm>

#include "params.h"
#include "assert.h"

#define _INFINITY_ 30000
#define INITIAL nullptr
#define NOOBJECT nullptr

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
		debug(true, "Received message", Hash{{"Sender", sender -> rank}, {"Message", size}, {"Shortest path", shortest_path}});
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
	bool is_incident(int rank);

	int get_shortest_path() { return shortest_path; }
	void set_shortest_path(int _shp) { shortest_path = _shp; }
	bool get_visited() { return visited; }
	void set_visited(bool _visited) { visited = _visited; }
	const vector<Edge *> get_edges() { return edges; }
	Vertex *ptr() { return this; }
};



class Edge
{
public:
	Vertex *origin, *target;	// указатели на начальную и конечную вершины
	const int length;	// его длина
	Edge(Vertex *start, Vertex *end, int _length): origin(start), target(end), length{_length}
	{
		assert(start != NOOBJECT && end != NOOBJECT, "Trying to add edge with NOOBJECT as one of the ends!", Hash{ {"Length", length} });
		assert(length < _INFINITY_, "Trying to add an edge with length over infinity!", Hash{ {"Length", length} });
		assert(length > 0, "Trying to add an edge with length less than zero", Hash{ {"Length", length} });
		note(start == end, "A cycle was added to graph! Did you really mean that?", Hash{ {"Start", origin -> rank}, {"End", target -> rank} });
		note(length == 0, "A zero length edge was added to graph!", Hash{ {"Origin", start->rank}, {"Target", end->rank} });
	}
	Edge *ptr() { return this; }
};

Vertex::Vertex(int _rank): message_sender(INITIAL), edges(0), shortest_path(_INFINITY_), visited(false), rank{_rank}
{
	assert(_rank >= 0, "Rank is less than zero!", Hash{ {"Rank", rank} });
	assert(edges.size() == 0, "Didn't clean up the edges!", Hash{{"Edges count", edges.size()}});
}
void Vertex::add_edge(Edge *_edge)
{
	assert(_edge != NOOBJECT, "Attempt to assign NOOBJECT as an edge!", Hash{ {"Vertex rank", rank} });
	const int edge_target_rank = _edge -> target -> rank;
	for(auto &edge: edges)
		assert(
			(edge -> target) -> rank != edge_target_rank, 
			"Multiple edges lead to one vertex!", 
			Hash{ {"Current vertex rank", rank}, {"Leads to", edge_target_rank} });

	// в список ребер добавляем новую ссылку
	edges.push_back(_edge);
}
void Vertex::send_messages()
{
	visited = true;
	if(edges.size() == 0)
		return;
	note(shortest_path >= _INFINITY_, "Sending message of infinite length", Hash{ {"Length", shortest_path} });
	for(auto &edge:edges)
		if(!edge -> target -> get_visited())
		{
			debug(true, "Sending message", Hash{ {"Current vertex", rank}, {"To", edge -> target -> rank}, {"Message size", edge -> length + shortest_path} });
			edge -> target -> receive_message(this, edge -> length + shortest_path);
		}
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
bool Vertex::is_incident(int rank)
{
	for(auto &edge:edges)
		if(edge -> target -> rank == rank)
			return true;
	return false;
}

class Graph
{
	Vertex **vertices;
	Edge **edges;
	int vertices_size, edges_size;
	int vertices_cnt, edges_cnt;

	Vertex *vertex_pointer(int rank)
	{
		assert(vertex_exists(rank), "Attempt to get a vertex that does not exists!", Hash{ {"Vertex rank", rank} });

		Vertex *result = NOOBJECT;
	 	for(int i = 0; i < vertices_cnt; i++)
	 	{
	 		if(vertices[i] -> rank == rank)
	 		{
	 			result = vertices[i];
	 			break;
	 		}
	 	}
	 	assert(result != NOOBJECT, "Vertex not found, while it exists!", Hash{ {"Rank", rank} });
	 	return result;

	}
	bool vertex_exists(int rank)
	{
	 	Vertex *result = NOOBJECT;
	 	for(int i = 0; i < vertices_cnt; i++)
	 	{
	 		if(vertices[i] -> rank == rank)
	 		{
	 			result = vertices[i];
	 			break;
	 		}
	 	}
	 	return result != NOOBJECT;
	}
	Vertex *get_next_vertex(bool check = false)
	{
		// поиск вершины, которую еще не посетили и с минимальным весом
		int min = _INFINITY_;
		Vertex *result = NOOBJECT;
		for(int i = 0; i < vertices_cnt; i++)
			if(!vertices[i] -> get_visited() && vertices[i] -> get_shortest_path() < min)
			{
				result = vertices[i];
				min = result -> get_shortest_path();
			}
		if(result != NOOBJECT &&!check)
		{
			note(min >= _INFINITY_, "Shortest path is infinite!", Hash{ {"Rank", result -> rank}, {"Shortest path", min} });
			debug(true, "Got next vertex", Hash{{"Rank", result -> rank}, {"Shortest path", min}});
		}
		return result;
	}
	bool vertices_left()
	{
		return get_next_vertex(true) != NOOBJECT;
	}
	Vertex *create_vertex(int rank)
	{
		// Мы можем хотеть записать уже имеющююся вершину
		// refute(vertex_exists(rank), "Vertex already exists", Hash{ {"Vertex", rank} });

		if(vertex_exists(rank))
			return vertex_pointer(rank);
		else
		{
			Vertex *a = new Vertex(rank);
			vertices[vertices_cnt++] = a;
			assert(vertices[vertices_cnt-1] -> get_edges().size() < _INFINITY_, "Too many edges!", 
				Hash{ {"Count", vertices[vertices_cnt-1] -> get_edges().size()} });
			return a;
		}

	}
	Edge *create_edge(Vertex *start, Vertex *finish, int length)
	{

		Edge *a = new Edge(start, finish, length);
		edges[edges_cnt++] = a;
		start -> add_edge(edges[edges_cnt-1]);
		return a;
	}
	void add_path(int origin_index, int target_index, int edge_length)
	{
		if(vertex_exists(origin_index) && vertex_exists(target_index) && !oriented)
			refute(vertex_pointer(origin_index) -> is_incident(target_index), "Trying to add multiple edges", 
				Hash{ {"Origin", origin_index}, {"Target", target_index}, {"Length", edge_length} }
			);

		Vertex *origin = create_vertex(origin_index);
		Vertex *target = create_vertex(target_index);
		debug(true, "Created vertices", Hash{ {"Origin", origin -> rank}, {"Target", target -> rank}, {"Number of vertices", vertices_cnt}});
		// (*origin).add_edge() - должен сделать внутри сам
		Edge *edge = create_edge(origin, target, edge_length);
		debug(true, "Created edge", 
			Hash{ {"Origin", edge -> origin -> rank}, {"Target", edge -> target -> rank} , {"Length", edge -> length}, {"Number of edges", edges_cnt}}
		);
		// if(!oriented)
		// 	edge = create_edge(target, origin, edge_length);
	}
public:
	Graph() 
	{
		assert(G.size() >= 3, "Graph is empty!");
		assert(G.size() % 3 == 0, "Wrong graph format!");

		vertices_size = oriented ? G.size()/3 * 2 : G.size()/3 * 4;
		edges_size = oriented ? G.size()/3 : G.size()/3 * 2;	
		vertices_cnt = 0;
		edges_cnt = 0;

		vertices = new Vertex* [vertices_size];
		edges = new Edge* [edges_size];
		for(int i = 0; i < G.size(); i+=3)
		{
			int start = G[i], finish = G[i+1], length = G[i+2];
			debug(true, "Adding new path", Hash{ {"Origin", start}, {"Target", finish} , {"Length", length}});
			add_path(start, finish, length);
			if(!oriented)
			{
				start = G[i+1], finish = G[i], length = G[i+2];
				debug(true, "Adding new path", Hash{ {"Origin", start}, {"Target", finish} , {"Length", length}});
				add_path(start, finish, length);
			}
		}
	}
	~Graph()
	{
		for(int i = 0; i < vertices_cnt; i++)
			delete vertices[i];
		delete [] vertices;
		for(int i = 0; i < edges_cnt; i++)
			delete edges[i];
		delete [] edges;
	}
	void dijkstra_alg(int origin, int target)
	{
		assert(vertex_exists(origin) && vertex_exists(target), "Vertex does not exist!", Hash{ {"Origin", origin}, {"Target", target} });
		if(origin == target)
		{
			printf("v%d ==> v%d - 0", origin, origin);
			return;
		}
		Vertex *current_vertex;
		vertex_pointer(origin) -> set_shortest_path(0);
		while(vertices_left())
		{
			current_vertex = get_next_vertex();
			current_vertex -> send_messages();
		}
		for(int i = 0; i < vertices_cnt; i++)
			if(vertices[i] -> rank != origin && vertices[i] -> rank != target)
				vertices[i] -> trace_path();
		{
			printf("Result:\n");
			vertex_pointer(target) -> trace_path();
		}
	}
};

int main()
{
	Graph gr = Graph();
	gr.dijkstra_alg(start_point, end_point);
}