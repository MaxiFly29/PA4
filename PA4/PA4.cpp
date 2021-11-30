#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <optional>
#include <unordered_set>

using namespace std;

size_t getSizeMaxLocalClique(const vector<vector<size_t>>& graph, size_t cur_node,
	set<size_t> avaible_nodes)
{

	set<size_t> cur_avaible_nodes;
	ranges::set_intersection(graph[cur_node], avaible_nodes, inserter(cur_avaible_nodes, end(cur_avaible_nodes)));
	size_t max_res = 0;
	for (auto node : graph[cur_node])
	{
		if (avaible_nodes.contains(node))
		{
			cur_avaible_nodes.erase(node);
			max_res = max(getSizeMaxLocalClique(graph, node, cur_avaible_nodes), max_res);
		}
	}
	return max_res + 1;
}

optional<size_t> getNearNode(const vector<vector<size_t>>& graph, size_t node, size_t dist, const unordered_set<size_t>& visited)
{
	vector<size_t> possible_nodes = {node};
	unordered_set<size_t> local_visited;
	for (size_t i = 0, start_pos = 0; i < dist; i++)
	{
		size_t n = possible_nodes.size();
		for (size_t j = start_pos; j < n; j++)
		{
			auto cur_node = possible_nodes[j];
			for (auto next_node : graph[cur_node])
				if (!visited.contains(next_node))
					possible_nodes.push_back(next_node);
		}
		start_pos = n;
	}
	if (possible_nodes.size() == 1)
		return nullopt;
	else
		return possible_nodes[rand() % (possible_nodes.size() - 1) + 1];
}

bool checkIfCliqueConsistBeeAlg(const vector<vector<size_t>>& graph, size_t clique_size,
	size_t bees_spies, size_t bees_on_best_areas, size_t bees_on_another_areas,
	size_t best_areas, size_t another_areas, size_t size_areas)
{
	//Graph should be sorted!
	vector<pair<size_t, size_t>> node_and_res;
	unordered_set<size_t> visited_nodes;
	for (size_t i = 0; i < bees_spies; i++)
	{
		size_t node = rand() % graph.size();
		if (visited_nodes.contains(node))
			for (size_t j = 0; j < graph.size(); j++)
			{
				node++;
				node %= graph.size();
			}
		if (visited_nodes.contains(node))
			return false;
		node_and_res.emplace_back(node, 
			getSizeMaxLocalClique(graph, node, { graph[node].begin(), graph[node].end() }));
		visited_nodes.insert(node);
		if (node_and_res.back().second >= clique_size)
			return true;
	}
	ranges::sort(node_and_res, 
		[](const pair<size_t, size_t>& l, const pair<size_t, size_t>& r) {return l.second > r.second; });
	size_t last_visited_size = 0;
	while (last_visited_size != graph.size())
	{
		last_visited_size = graph.size();
		vector<pair<size_t, size_t>> cur_node_and_res;
		size_t i = 0;
		for (; i < best_areas && i < node_and_res.size(); i++)
		{
			for (size_t j = 0; j < bees_on_best_areas; j++) {
				auto [node, res] = node_and_res[i];
				auto near_node = getNearNode(graph, node, size_areas, visited_nodes);
				if (!near_node)
					continue;
				cur_node_and_res.emplace_back(*near_node,
					getSizeMaxLocalClique(graph, *near_node, { graph[*near_node].begin(), graph[*near_node].end() }));
				visited_nodes.insert(*near_node);
				if (cur_node_and_res.back().second >= clique_size)
					return true;
			}
		}
		for (; i < another_areas + best_areas && i < node_and_res.size(); i++)
		{
			for (size_t j = 0; j < bees_on_another_areas; j++) {
				auto [node, res] = node_and_res[i];
				auto near_node = getNearNode(graph, node, size_areas, visited_nodes);
				if (!near_node)
					continue;
				cur_node_and_res.emplace_back(*near_node,
					getSizeMaxLocalClique(graph, *near_node, { graph[*near_node].begin(), graph[*near_node].end() }));
				visited_nodes.insert(*near_node);
				if (cur_node_and_res.back().second >= clique_size)
					return true;
			}
		}
		node_and_res = move(cur_node_and_res);
		ranges::sort(node_and_res,
			[](const pair<size_t, size_t>& l, const pair<size_t, size_t>& r) {return l.second > r.second; });
	}
	return false;
}

vector<vector<size_t>> fromMatrixFormToListForm(const vector<vector<bool>>& matrix)
{
	vector<vector<size_t>> graph;
	for (const auto& ways : matrix)
	{
		vector<size_t> ws;
		for (size_t i = 0; i < ways.size(); i++)
			if (ways[i])
				ws.push_back(i);
		graph.push_back(move(ws));
	}
	return graph;
}

int main()
{
	auto graph = fromMatrixFormToListForm(
		{ {0, 1, 1, 0, 0, 0, 1, 0, 0, 1},
			{1, 0, 1, 0, 1, 1, 0, 0, 0, 1 },
			{1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
			{0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
			{0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
			{0, 1, 0, 1, 1, 0, 0, 0, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
			{0, 0, 1, 0, 0, 0, 1, 0, 1, 0},
			{0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
			{1, 1, 1, 1, 0, 1, 0, 0, 0, 0 }}
	);
	cout << checkIfCliqueConsistBeeAlg(graph, 5, 3, 3, 
			1, 1, 2, 1);
}