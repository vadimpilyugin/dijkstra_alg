#include <tuple>
#include <algorithm>

#include "params.h"

#define INFINITY 30000
#define INITIAL nullptr
#define NOOBJECT nullptr

typedef std::map<const char *, int> Hash

void assert(bool __expr, __str__ = "", Hash params = Hash()) {
	if(!__expr)	{
		fprintf(stderr, "\x1b[1;31mRuntime error: \x1b[0m")
		fprintf(stderr, "%s\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
		exit(1);
	}
}

void note(bool __expr, __str__ = "",  Hash params = Hash()) {
	if(!__expr)	
	{
		fprintf(stderr, "\x1b[1;33mNote: \x1b[0m", );
		fprintf(stderr, "%s\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
	}
}

class Vertex
{
	Vertex *message_sender;		// От кого было получен кратчайший путь
	vector<Edge *> edges;		// Исходящие ребра
	int shortest_path;		// Величина этого пути
	bool visited;
public:
	const int rank;	// номер вершины в графе
	Vertex(int _rank): shortest_path(INFINITY), message_sender(INITIAL), rank{_rank}, visited(false)
	{
		assert(_rank > 0, "Rank is less than zero!", Hash{ {"Rank", rank} });
	}
	void add_edge(Edge *_edge)
	{
		assert(_edge != NOOBJECT, "Attempt to assign NOOBJECT as an edge!", Hash{ {"Vertex rank", rank} });
		for(auto edge: edges)
			assert(
				edge.target != _edge.target, 
				"Multiple edges lead to one vertex!", 
				Hash{ {"Current vertex rank", rank}, {"Leads to", _edge.target.rank} }
			)

		// в список ребер добавляем новую ссылку
		edges << _edge;
	}
	int get_shortest_path() { return shortest_path; }
	void set_shortest_path(int _shp) { shortest_path = _shp; }
	void get_visited() { return visited; }
	void set_visited(bool _visited) { visited = _visited; }
};

class Edge
{
public:
	const Vertex *origin, *target;	// указатели на начальную и конечную вершины
	const int length;	// его длина
	Edge(Vertex *start, Vertex *end, int _length): origin{start}, target{end}, length{_length}
	{
		assert(start != NOOBJECT && end != NOOBJECT, "Trying to add edge with NOOBJECT as one of the ends!", Hash{ {"Length", length} });
		assert(_length < INFINITY, "Trying to add an edge with length over infinity!", Hash{ {"Length", length} });
		assert(_length > 0, "Trying to add an edge with length below of = zero!", Hash{ {"Length", length} });
		note(start == end, "A cycle was added to graph! Did you really mean that?", Hash{ {"Vertex rank where the cycle was found", (*start).rank} });

		// записываем в исток адрес текущего объекта - нового ребра
		(*origin).add_edge(&self);	
	}
};

class Graph
{
	vector<Vertex> vertices;
	vector<Edge> edges;

	Vertex *vertex_pointer(int rank)
	{
		assert(vertex_exists(rank), "Attempt to get a vertex that does not exists!", Hash{ {"Vertex rank", rank} });

		// поиск вершины с номером rank
		auto result = std::find_if(vertices.begin(), vertices.end(), [&](const Vertex & o) {
    		o.rank == rank;
		});
		// функция поиска возвращает итератор, поэтому *
		return &(*result));
	}
	Vertex *get_next_vertex()
	{
		// сначала узнаем, чему равен минимум по расстояниям
		int min = INFINITY;
		{
			auto min = std::min_element( vertices.begin(), vertices.end(),
                             []( const Vertex &a, const Vertex &b )
                             {
                                 return a.get_shortest_path() < b.get_shortest_path();
                             } ); 
		}

		// поиск вершины, которую еще не посетили и с минимальным весом
		auto result = std::find_if(vertices.begin(), vertices.end(), [&](const Vertex & o) {
    		o.get_visited() == false;
		});
		if(result == std::end(vertices))
			return NOOBJECT;
		else
			// функция поиска возвращает итератор, поэтому *
			return &(*result));
	}
	bool vertex_exists(int rank)
	{
		auto result = std::find(vertices.begin(), vertices.end(), [&](const Vertex & o) {
    		o.rank == rank;
		});
	 	return result != std::end(vertices);
	}
	Vertex *create_vertex(int rank)
	{
		if(!vertex_exists(rank))
		{
			// записываем в массив саму вершину
			vertices << Vertex(rank);
			// возвращаем указатель на нее
			return &vertices.back();
		}
		else
			return vertex_pointer(rank);
	}
	Edge *create_edge(Vertex *start, Vertex *finish, int length)
	{
		// записать новое ребро в массив
		edges << Edge(start, finish, length);
		// добавить указатель на ребро в стартовую вершину
		(*start).add_edge(&edges.back());
	}
public:
	Graph() {}
	Vertex *add_path(int origin_index, int target_index, int edge_length)
	{
		Vertex *origin = create_vertex(origin_index);
		Vertex *target = create_vertex(target_index);
		// (*origin).add_edge() - должен сделать внутри сам
		Edge *edge = create_edge(origin, target, edge_length);
		return origin;
	}
};

int main()
{
	return 0;
}