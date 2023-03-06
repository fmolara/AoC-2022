// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#pragma once

#include "DirectedGraph.h"
#include <limits>
#include <vector>
#include <queue>
#include <set>

template <typename TNode, typename TEdge>
void Dijkstra(const DirectedGraph<TNode, TEdge>& g, size_t startNodeIndex, size_t endNodeIndex, std::vector<size_t>& p, std::vector<TEdge>& d)
{
    using graph_type = DirectedGraph<TNode, TEdge>;
    using pair_type = std::pair<TEdge, size_t>;

    const TEdge max_edge = std::numeric_limits<TEdge>::max();

    // Optimization: use priority-queues for speed-up minimum distance retrieval
    std::priority_queue<pair_type, std::vector<pair_type>, std::greater<pair_type>> pq;

    // STEP 1: initialization
    std::set<size_t> V;
    p.resize(g.NodeCount());
    d.resize(g.NodeCount());
    for (size_t i = 0; i < g.NodeCount(); ++i)
    {
        p[i] = graph_type::npos;
        d[i] = max_edge;
    }
    d[startNodeIndex] = 0;
    pq.push(std::make_pair(0, startNodeIndex));

    while (true)
    {
        // STEP 2: select "visiting" node (find node with minumun d[j])
        size_t j = graph_type::npos;
        for (; !pq.empty(); pq.pop())
        {
            if (V.contains(pq.top().second))  // skip already "visited" nodes
                continue;
            j = pq.top().second;
            pq.pop();
            break;
        }

        // STEP 3: termination condition (disconnected graph)
        if (j == graph_type::npos || d[j] == max_edge)
            break;

        // STEP 4: visit of j node (update of connected nodes)
        V.insert(j);
        for (auto e : g.NodeOutEdges(j))
        {
            size_t k = e.to;
            if (V.contains(k))  // skip already "visited" nodes
                continue;
            TEdge newCost = d[j] + e.value;
            if (newCost < d[k])
            {
                d[k] = newCost;
                p[k] = j;
                pq.push(std::make_pair(newCost, k));
            }
        }

        // STEP 5: termination condition (target node reached or all nodes marked as "visited")
        if (j == endNodeIndex || V.size() == g.NodeCount())
            break;
    }
}

