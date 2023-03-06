// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#pragma once

#include <vector>
#include <ranges>

template <typename T>
class Edge
{
public:
    size_t from;
    size_t to;
    T value;

    Edge(size_t _from, size_t _to, T _value)
        : from(_from)
        , to(_to)
        , value(_value)
    {
    }
};

template <typename TNode, typename TEdge>
class DirectedGraph
{
public:
    using node_type = TNode;
    using edge_type = TEdge;
    static constexpr auto npos{ static_cast<size_t>(-1) };

public:
    DirectedGraph()
    {
    }

    void Clear()
    {
        _nodes.clear();
        _edges.clear();
    }

    size_t AddNode(TNode value)
    {
        _nodes.push_back(value);
        return _nodes.size()-1;
    }
    void RemoveNode(size_t nodeIndex)
    {
        _nodes.erase(_nodes.begin()+nodeIndex);
        for (auto it = _edges.begin(); it != _edges.end(); ++it)
        {
            // elimina archi relativi al nodo
            if (it->from == nodeIndex || it->to == nodeIndex)
            {
                it = _edges.erase(it)-1;
                continue;
            }
            // aggiorna estremi degli altri archi
            if (it->from > nodeIndex)
                it->from--;
            if (it->to > nodeIndex)
                it->to--;
        }
    }

    size_t NodeCount() const { return _nodes.size(); }

    TNode GetNodeValue(size_t nodeIndex) const { return _nodes[nodeIndex]; }
    TNode operator [] (size_t nodeIndex) const { return _nodes[nodeIndex]; }

    size_t GetNodeIndex(TNode node) const
    {
        for (size_t i = 0; i < _nodes.size(); ++i)
            if (_nodes[i] == node)
                return i;
        return npos;
    }

    auto Nodes() const
    {
        auto pred = [](const TNode& n) { return true; };

        return _nodes | std::views::filter(pred);
    }

    size_t GetNodeInDegree(size_t nodeIndex) const
    {
        size_t res = 0;
        for (auto it = _edges.cbegin(); it != _edges.cend(); ++it)
            if (it->to == nodeIndex)
                ++res;
        return res;
    }
    size_t GetNodeOutDegree(size_t nodeIndex) const
    {
        size_t res = 0;
        for (auto it = _edges.cbegin(); it != _edges.cend(); ++it)
            if (it->from == nodeIndex)
                ++res;
        return res;
    }
    auto NodeInEdges(size_t nodeIndex) const
    {
        auto pred = [nodeIndex](const Edge<TEdge>& e) { return e.to == nodeIndex; };

        return _edges | std::views::filter(pred);
    }
    auto NodeOutEdges(size_t nodeIndex) const
    {
        auto pred = [nodeIndex](const Edge<TEdge>& e) { return e.from == nodeIndex; };

        return _edges | std::views::filter(pred);
    }



    virtual void AddEdge(size_t nodeIndexFrom, size_t nodeIndexTo, TEdge value)
    {
        if (nodeIndexFrom < 0 || _nodes.size() <= nodeIndexFrom)
            throw;
        if (nodeIndexTo < 0 || _nodes.size() <= nodeIndexTo)
            throw;
        _edges.push_back(Edge<TEdge>(nodeIndexFrom, nodeIndexTo, value));
    }
    void RemoveEdgeAt(size_t edgeIndex)
    {
        if (edgeIndex < 0 || _edges.size() <= edgeIndex)
            throw;
        _edges.erase(_edges.begin()+edgeIndex);
    }
    virtual void RemoveEdge(size_t nodeIndexFrom, size_t nodeIndexTo)
    {
        for (auto it = _edges.begin(); it != _edges.end(); ++it)
            if (it->from == nodeIndexFrom && it->to == nodeIndexTo)
                it = _edges.erase(it)-1;
    }
    size_t EdgeCount() const { return _edges.Count; }

private:
    std::vector<TNode>        _nodes;
    std::vector<Edge<TEdge>>  _edges;
};


template <typename TNode, typename TEdge>
class UndirectedGraph : public DirectedGraph<TNode, TEdge>
{
public:
    virtual void AddEdge(size_t nodeIndexFrom, size_t nodeIndexTo, TEdge value) override
    {
        DirectedGraph<TNode, TEdge>::AddEdge(nodeIndexFrom, nodeIndexTo, value);
        DirectedGraph<TNode, TEdge>::AddEdge(nodeIndexTo, nodeIndexFrom, value);
    }
    virtual void RemoveEdge(size_t nodeIndexFrom, size_t nodeIndexTo) override
    {
        DirectedGraph<TNode, TEdge>::RemoveEdge(nodeIndexFrom, nodeIndexTo);
        DirectedGraph<TNode, TEdge>::RemoveEdge(nodeIndexTo, nodeIndexFrom);
    }
};
