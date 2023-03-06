// Advent Of Code 2022
// Day 22
//
// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <queue>

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif



typedef std::vector<std::string> board_t;

void ParseBoard(std::istream& in, board_t& board, std::string & path)
{
	board.clear();

	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		board.push_back(line);
	}

	if (!std::getline(in, path))
		ASSERT(false);
}

void FindStartingTile(const board_t& board, size_t& ir, size_t& ic)
{
	for (ir = ic = 0; ic < board[ir].size(); ++ic)
	{
		if (board[ir][ic] == '.')
			return;
	}

	ASSERT(false);
}

const struct Moves
{
	int dx, dy;
}
moves[] =
{
	{ +1,  0 },  // >  0 == right
	{  0, +1 },  // v  1 == down
	{ -1,  0 },  // <  2 == left
	{  0, -1 },  // ^  3 == up
};

void StepY_2D(const board_t& board, size_t& ir, const size_t ic, int dy)
{
	ASSERT(0 <= ir && ir < board.size());
	ASSERT(0 <= ic && ic < board[ir].size());
	ASSERT(board[ir][ic] == '.');

	size_t new_ir = (ir + board.size() + dy) % board.size();
	while (ic >= board[new_ir].size() || board[new_ir][ic] == ' ')
		new_ir = (new_ir + board.size() + dy) % board.size();

	if (board[new_ir][ic] == '#')
		return;

	ASSERT(board[new_ir][ic] == '.');
	ir = new_ir;
}

void StepX_2D(const board_t& board, const size_t ir, size_t& ic, int dx)
{
	ASSERT(0 <= ir && ir < board.size());
	ASSERT(0 <= ic && ic < board[ir].size());
	ASSERT(board[ir][ic] == '.');

	size_t new_ic = (ic + board[ir].size() + dx) % board[ir].size();
	while (board[ir][new_ic] == ' ')
		new_ic = (new_ic + board[ir].size() + dx) % board[ir].size();

	if (board[ir][new_ic] == '#')
		return;

	ASSERT(board[ir][new_ic] == '.');
	ic = new_ic;
}

void Move_2D(const board_t& board, const size_t order, size_t& ir, size_t& ic, size_t& dir, int steps)
{
	const auto& move = moves[dir];

	if (move.dx != 0)
	{
		ASSERT(move.dy == 0);
		for (; steps > 0; --steps)
			StepX_2D(board, ir, ic, move.dx);
	}
	else
	{
		ASSERT(move.dx == 0);
		for (; steps > 0; --steps)
			StepY_2D(board, ir, ic, move.dy);
	}
}

enum Edge
{
	Top = 0,
	Right = 1,
	Bottom = 2,
	Left = 3,

	Edge_Count
};

struct RowCol
{
	int64_t ir, ic;

	RowCol(int64_t _ir = 0, int64_t _ic = 0)
		: ir(_ir), ic(_ic)
	{
	}

	auto operator <=> (const RowCol&) const = default;
};

struct Vertex
{
	// Valid vertex of cube are:
	//
	//          X  Y  Z
	//
	//          1, 1, 0
	//          1, 0, 0
	//          0, 1, 0
	//          0, 0, 0
	//          1, 1, 1
	//          1, 0, 1
	//          0, 1, 1
	//          0, 0, 1

	int coo[3];

	Vertex(int x = 0, int y = 0, int z = 0)
	{
		coo[0] = x;
		coo[1] = y;
		coo[2] = z;
	}

	bool IsValid() const
	{
		for (int i = 0; i < _countof(coo); ++i)
			if (coo[i] < 0 || 1 < coo[i])
				return false;
		return true;
	}

	auto operator <=> (const Vertex&) const = default;
};

void AssignVerticies(
	Vertex vertices[5][5],
	const RowCol src[4],
	const RowCol dst[2])
{
	for (int i = 0; i < 4; ++i)
	{
		const int64_t ir = src[i].ir;
		const int64_t ic = src[i].ic;
		ASSERT(vertices[ir][ic].IsValid());
	}

	for (int i = 0; i < 2; ++i)
	{
		const int64_t ir = dst[i].ir;
		const int64_t ic = dst[i].ic;
		ASSERT(!vertices[ir][ic].IsValid());
	}

	// Find plane (totally invarint coo)
	bool plane[3] = { true, true, true };
	for (int j0 = 0; j0 < 4; ++j0)
	{
		const int64_t ir0 = src[j0].ir;
		const int64_t ic0 = src[j0].ic;
		for (int j1 = j0+1; j1 < 4; ++j1)
		{
			const int64_t ir1 = src[j1].ir;
			const int64_t ic1 = src[j1].ic;
			for (int i = 0; i < _countof(vertices[ir0][ic0].coo); ++i)
				if (vertices[ir0][ic0].coo[i] != vertices[ir1][ic1].coo[i])
					plane[i] = false;
		}
	}

	int i_plane;
	for (i_plane = 0; i_plane < _countof(plane); ++i_plane)
		if (plane[i_plane])
			break;

	if (i_plane >= _countof(plane))
	{
		ASSERT(false);  // the four vertex in src[] belongs to the same face, so they must share a plane
		return;
	}

	for (int j = i_plane+1; j < _countof(plane); ++j)
		ASSERT(!plane[j]);

	// Assign destination vertecies, keeping non-planar coordinates
	for (int i = 0; i < 2; ++i)
	{
		const int64_t ir_src = src[i+2].ir;
		const int64_t ic_src = src[i+2].ic;
		const int64_t ir_dst = dst[i].ir;
		const int64_t ic_dst = dst[i].ic;

		vertices[ir_dst][ic_dst] = vertices[ir_src][ic_src];
		vertices[ir_dst][ic_dst].coo[i_plane] = (vertices[ir_dst][ic_dst].coo[i_plane] == 0 ? 1 : 0);
	}
}

inline
void GetFace4x4_3D(const board_t& board, const size_t order, const int64_t ir, const int64_t ic, int64_t& face_ir, int64_t& face_ic)
{
	face_ir = ir / order;
	face_ic = ic / order;
}

inline
void GetCoo4x4_3D(const board_t& board, const size_t order, const int64_t face_ir, const int64_t face_ic, int64_t& ir, int64_t& ic)
{
	ir = face_ir * order;
	ic = face_ic * order;
}

inline
bool IsValidCoo4x4_3D(const board_t& board, const size_t order, const int64_t ir, const int64_t ic)
{
	if (ir < 0 || board.size() <= (size_t)ir)
		return false;

	if (ic < 0 || board[ir].size() <= (size_t)ic)
		return false;

	if (board[ir][ic] == ' ')
		return false;

	return true;
}

inline
bool IsValidFace4x4_3D(const board_t& board, const size_t order, const int64_t face_ir, const int64_t face_ic)
{
	int64_t ir, ic;
	GetCoo4x4_3D(board, order, face_ir, face_ic, ir, ic);

	return IsValidCoo4x4_3D(board, order, ir, ic);
}

bool UnfoldVerticies_3D(const board_t& board, const size_t order, Vertex vertices[5][5], int64_t from_face_ir, int64_t from_face_ic, int64_t delta_face_ir, int64_t delta_face_ic)
{
	ASSERT(IsValidFace4x4_3D(board, order, from_face_ir, from_face_ic));
	ASSERT(delta_face_ir == 0 && (delta_face_ic == -1 || delta_face_ic == +1) ||
	       delta_face_ic == 0 && (delta_face_ir == -1 || delta_face_ir == +1));

	int64_t face_ir = from_face_ir + delta_face_ir;
	int64_t face_ic = from_face_ic + delta_face_ic;
	if (!IsValidFace4x4_3D(board, order, face_ir, face_ic))
		return false;

	if (delta_face_ir == -1)
	{
		RowCol src[4] = { {from_face_ir+1, from_face_ic+0}, {from_face_ir+1, from_face_ic+1},
		                  {from_face_ir+0, from_face_ic+0}, {from_face_ir+0, from_face_ic+1} };
		RowCol dst[2] = { {from_face_ir-1, from_face_ic+0}, {from_face_ir-1, from_face_ic+1} };
		AssignVerticies(vertices, src, dst);
	}
	else if (delta_face_ir == +1)
	{
		RowCol src[4] = { {from_face_ir+0, from_face_ic+0}, {from_face_ir+0, from_face_ic+1},
		                  {from_face_ir+1, from_face_ic+0}, {from_face_ir+1, from_face_ic+1} };
		RowCol dst[2] = { {from_face_ir+2, from_face_ic+0}, {from_face_ir+2, from_face_ic+1} };
		AssignVerticies(vertices, src, dst);
	}
	else if (delta_face_ic == -1)
	{
		RowCol src[4] = { {from_face_ir+0, from_face_ic+1}, {from_face_ir+1, from_face_ic+1},
		                  {from_face_ir+0, from_face_ic+0}, {from_face_ir+1, from_face_ic+0} };
		RowCol dst[2] = { {from_face_ir+0, from_face_ic-1}, {from_face_ir+1, from_face_ic-1} };
		AssignVerticies(vertices, src, dst);
	}
	else if (delta_face_ic == +1)
	{
		RowCol src[4] = { {from_face_ir+0, from_face_ic+0}, {from_face_ir+1, from_face_ic+0},
		                  {from_face_ir+0, from_face_ic+1}, {from_face_ir+1, from_face_ic+1} };
		RowCol dst[2] = { {from_face_ir+0, from_face_ic+2}, {from_face_ir+1, from_face_ic+2} };
		AssignVerticies(vertices, src, dst);
	}
	else
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void UnfoldVerticies_3D(const board_t& board, const size_t order, Vertex vertices[5][5])
{
	for (int64_t ir = 0; ir < 5; ++ir)
		for (int64_t ic = 0; ic < 5; ++ic)
			vertices[ir][ic] = Vertex(-1, -1, -1);

	bool assigned[4][4];
	for (int64_t ir = 0; ir < 4; ++ir)
		for (int64_t ic = 0; ic < 4; ++ic)
			assigned[ir][ic] = false;

	std::queue<RowCol> todo;

	{
		// Find first face
		int64_t face_ir = 0, face_ic;
		for (face_ic = 0; face_ic < 4; ++face_ic)
			if (IsValidFace4x4_3D(board, order, face_ir, face_ic))
				break;

		// Assign first face's vertices
		for (int y = 0; y <= 1; ++y)
			for (int x = 0; x <= 1; ++x)
				vertices[face_ir+y][face_ic+x] = Vertex(x, (y == 0 ? 1 : 0));

		todo.push(RowCol(face_ir, face_ic));
		assigned[face_ir][face_ic] = true;
	}

	int assigned_face_cnt = 1;
	while (!todo.empty())
	{
		const int64_t from_face_ir = todo.front().ir;
		const int64_t from_face_ic = todo.front().ic;
		todo.pop();

		for (int i = 0; i < _countof(moves); ++i)
		{
			const auto& move = moves[i];

			const int64_t face_ir = from_face_ir+move.dy;
			const int64_t face_ic = from_face_ic+move.dx;

			if (!IsValidFace4x4_3D(board, order, face_ir, face_ic))
				continue;

			if (assigned[face_ir][face_ic])
				continue;
			assigned[face_ir][face_ic] = true;
			++assigned_face_cnt;

			if (UnfoldVerticies_3D(board, order, vertices, from_face_ir, from_face_ic, move.dy, move.dx))
			{
				todo.push(RowCol(face_ir, face_ic));
			}
		}
	}
	ASSERT(assigned_face_cnt = 6);
}

inline
bool TestEdgeCW_3D(const Vertex vertices[5][5], const RowCol& v0, const RowCol& v1, const RowCol& _v0, const RowCol& _v1)
{
	if (_v1.ir < 0 || 5 <= _v1.ir)
		return false;
	if (_v1.ic < 0 || 5 <= _v1.ic)
		return false;
	if (vertices[_v0.ir][_v0.ic] != vertices[v0.ir][v0.ic])
		return false;
	if (vertices[_v1.ir][_v1.ic] != vertices[v1.ir][v1.ic])
		return false;
	if (v0 == _v0 && v1 == _v1)  // not the same edge
		return false;
	return true;
}

bool FindMatchingEdgeCW_3D(const Vertex vertices[5][5], const RowCol& v0, const RowCol& v1, RowCol& _v0, RowCol& _v1)
{
	for (_v0.ir = 0; _v0.ir < 5; ++_v0.ir)
	{
		for (_v0.ic = 0; _v0.ic < 5; ++_v0.ic)
		{
			if (vertices[_v0.ir][_v0.ic] != vertices[v0.ir][v0.ic])
				continue;

			for (int i = 0; i < _countof(moves); ++i)
			{
				const auto& move = moves[i];
				if (TestEdgeCW_3D(vertices, v0, v1, _v0, _v1 = RowCol(_v0.ir + move.dy, _v0.ic + move.dx)))
					return true;
			}
		}
	}
	return false;
}

void RowColToFace(const board_t& board, const size_t order, const RowCol& v0, const RowCol& v1, int64_t& face_ir, int64_t& face_ic, Edge& edge)
{
	ASSERT(v0.ir == v1.ir && std::abs(v0.ic - v1.ic) == 1 ||
	       v0.ic == v1.ic && std::abs(v0.ir - v1.ir) == 1);

	if (v0.ir == v1.ir)
	{
		face_ic = std::min(v0.ic, v1.ic);

		ASSERT( IsValidFace4x4_3D(board, order, v0.ir, face_ic) && !IsValidFace4x4_3D(board, order, v0.ir-1, face_ic) ||
		       !IsValidFace4x4_3D(board, order, v0.ir, face_ic) &&  IsValidFace4x4_3D(board, order, v0.ir-1, face_ic));

		if (IsValidFace4x4_3D(board, order, v0.ir, face_ic))
		{
			face_ir = v0.ir; edge = Top;
		}
		else
		{
			face_ir = v0.ir-1; edge = Bottom;
		}
	}
	else
	{
		face_ir = std::min(v0.ir, v1.ir);

		ASSERT( IsValidFace4x4_3D(board, order, face_ir, v0.ic) && !IsValidFace4x4_3D(board, order, face_ir, v0.ic-1) ||
		       !IsValidFace4x4_3D(board, order, face_ir, v0.ic) &&  IsValidFace4x4_3D(board, order, face_ir, v0.ic-1));

		if (IsValidFace4x4_3D(board, order, face_ir, v0.ic))
		{
			face_ic = v0.ic; edge = Left;
		}
		else
		{
			face_ic = v0.ic-1; edge = Right;
		}
	}
}

void WrapAroundEdgeCW_3D(const board_t& board, const size_t order, const Vertex vertices[5][5], size_t& ir, size_t& ic, size_t& dir, Edge edge)
{
	ASSERT(IsValidCoo4x4_3D(board, order, ir, ic));

	int64_t face_ir, face_ic;
	GetFace4x4_3D(board, order, ir, ic, face_ir, face_ic);

	const int64_t offs = (edge == Left || edge == Right ? ir-face_ir*order
	                                                    : ic-face_ic*order);
	ASSERT(0 <= offs && (size_t)offs < order);

	RowCol v0, v1;
	switch (edge)
	{
	case Top:    v0 = RowCol(face_ir+0, face_ic+0); v1 = RowCol(face_ir+0, face_ic+1); break;
	case Right:  v0 = RowCol(face_ir+0, face_ic+1); v1 = RowCol(face_ir+1, face_ic+1); break;
	case Bottom: v0 = RowCol(face_ir+1, face_ic+0); v1 = RowCol(face_ir+1, face_ic+1); break;
	case Left:   v0 = RowCol(face_ir+0, face_ic+0); v1 = RowCol(face_ir+1, face_ic+0); break;
	default: ASSERT(false);
	}

	int64_t _face_ir, _face_ic;
	Edge _edge;
	{
		RowCol _v0, _v1;
		if (!FindMatchingEdgeCW_3D(vertices, v0, v1, _v0, _v1))
		{
			ASSERT(false);
			return;
		}
		RowColToFace(board, order, _v0, _v1, _face_ir, _face_ic, _edge);
	}

	bool invert_offs[Edge_Count][Edge_Count] = {
		/*            Top     Right   Bottom  Left  */
		/*Top    */ { true,   true,   false,  false },
		/*Right  */ { true,   true,   false,  false },
		/*Bottom */ { false,  false,  true,   true  },
		/*Left   */ { false,  false,  true,   true  },
	};

	int64_t _ir, _ic;
	GetCoo4x4_3D(board, order, _face_ir, _face_ic, _ir, _ic);

	switch (_edge)
	{
	case Right:  _ic += order-1; break;
	case Bottom: _ir += order-1; break;
	}

	switch (_edge)
	{
	case Right:
	case Left:
		_ir += (invert_offs[edge][_edge] ? order-1-offs : offs); break;

	case Bottom:
	case Top:
		_ic += (invert_offs[edge][_edge] ? order-1-offs : offs); break;
	}

	ir = _ir;
	ic = _ic;
	dir = (_edge+1)%4;  // there is a +1 gap between Edge and moves[]
}

void TryStep_3D(const board_t& board, size_t& ir, size_t& ic, size_t& dir, size_t new_ir, size_t new_ic, size_t new_dir)
{
	if (board[new_ir][new_ic] == '#')
		return;

	ASSERT(board[new_ir][new_ic] == '.');
	ir = new_ir;
	ic = new_ic;
	dir = new_dir;
}

inline
void TryWrapAroundEdgeCW_3D(const board_t& board, const size_t order, const Vertex vertices[5][5], size_t& ir, size_t& ic, size_t& dir, Edge edge)
{
	size_t new_ir = ir, new_ic = ic, new_dir = dir;
	WrapAroundEdgeCW_3D(board, order, vertices, new_ir, new_ic, new_dir, edge);
	TryStep_3D(board, ir, ic, dir, new_ir, new_ic, new_dir);
}

void StepY_3D(const board_t& board, const size_t order, const Vertex vertices[5][5], size_t& ir, size_t& ic, size_t& dir)
{
	ASSERT(0 <= ir && ir < board.size());
	ASSERT(0 <= ic && ic < board[ir].size());
	ASSERT(board[ir][ic] == '.');

	const auto& move = moves[dir];
	ASSERT(move.dx == 0 && move.dy != 0);

	int64_t face_ir, face_ic;
	GetFace4x4_3D(board, order, ir, ic, face_ir, face_ic);
	const int64_t offs = ic - face_ic*order;
	ASSERT(0 <= offs && (size_t)offs < order);

	if (move.dy < 0 && (ir == 0 || board[ir-1][ic] == ' '))
	{
		TryWrapAroundEdgeCW_3D(board, order, vertices, ir, ic, dir, Top);
	}
	else if (move.dy > 0 && (ir+1 == board.size() || ic >= board[ir+1].size() || board[ir+1][ic] == ' '))
	{
		TryWrapAroundEdgeCW_3D(board, order, vertices, ir, ic, dir, Bottom);
	}
	else
	{
		size_t new_ir = (ir + board.size() + move.dy) % board.size();
		TryStep_3D(board, ir, ic, dir, new_ir, ic, dir);
	}
}

void StepX_3D(const board_t& board, const size_t order, const Vertex vertices[5][5], size_t& ir, size_t& ic, size_t& dir)
{
	ASSERT(0 <= ir && ir < board.size());
	ASSERT(0 <= ic && ic < board[ir].size());
	ASSERT(board[ir][ic] == '.');

	const auto& move = moves[dir];
	ASSERT(move.dx != 0 && move.dy == 0);

	int64_t face_ir, face_ic;
	GetFace4x4_3D(board, order, ir, ic, face_ir, face_ic);
	const int64_t offs = ir - face_ir*order;
	ASSERT(0 <= offs && (size_t)offs < order);

	if (move.dx < 0 && (ic == 0 || board[ir][ic-1] == ' '))
	{
		TryWrapAroundEdgeCW_3D(board, order, vertices, ir, ic, dir, Left);
	}
	else if (move.dx > 0 && (ic+1 == board[ir].size() || board[ir][ic+1] == ' '))
	{
		TryWrapAroundEdgeCW_3D(board, order, vertices, ir, ic, dir, Right);
	}
	else
	{
		size_t new_ic = (ic + board[ir].size() + move.dx) % board[ir].size();
		TryStep_3D(board, ir, ic, dir, ir, new_ic, dir);
	}
}

void Move_3D(const board_t& board, const size_t order, size_t& ir, size_t& ic, size_t& dir, int steps)
{
	static Vertex vertices[5][5];
	static bool one_time_init = false;
	if (!one_time_init)
	{
		UnfoldVerticies_3D(board, order, vertices);
		one_time_init = true;
	}

	for (; steps > 0; --steps)
	{
		const auto& move = moves[dir];

		if (move.dx != 0)
		{
			ASSERT(move.dy == 0);
			StepX_3D(board, order, vertices, ir, ic, dir);
		}
		else
		{
			ASSERT(move.dx == 0);
			StepY_3D(board, order, vertices, ir, ic, dir);
		}
	}
}

typedef void (*tMover)(const board_t& board, size_t order, size_t& ir, size_t& ic, size_t& dir, int steps);

int PartCommon(std::istream& in, tMover mover, const size_t order)
{
	board_t board;
	std::string path;
	ParseBoard(in, board, path);

	size_t ir, ic;
	FindStartingTile(board, ir, ic);

	size_t dir = 0;
	for (size_t i_beg = 0, i_end; i_beg < path.size(); i_beg = i_end)
	{
		if (std::isdigit(path[i_beg]))
		{
			char* pEnd;
			const long long steps = std::strtol(path.c_str() + i_beg, &pEnd, 10);
			i_end = pEnd - path.c_str();

			(*mover)(board, order, ir, ic, dir, (int)steps);
		}
		else
		{
			ASSERT(path[i_beg] == 'L' || path[i_beg] == 'R');

			dir = (dir + 4 + (path[i_end] == 'L' ? -1 : +1))%4;
			i_end = i_beg+1;
		}
	}

	return 1000*(int)(ir+1) + 4*(int)(ic+1) + (int)dir;
}

int PartOne(std::istream& in)               { return PartCommon(in, &Move_2D,     0); }
int PartTwo(std::istream& in, size_t order) { return PartCommon(in, &Move_3D, order); }

int main()
{
#if 0
	const size_t order = 4;
	std::istringstream in(
R"(        ...#
        .#..
        #...
        ....
...#.......#
........#...
..#....#....
..........#.
        ...#....
        .....#..
        .#......
        ......#.

10R5L5R10L4R5L5)");
#else
	const size_t order = 50;
	std::ifstream in("input1.txt");
#endif

#if 0
	int final_password = PartOne(in);  // 76332
	std::cout << final_password << std::endl;
#else
	int final_password = PartTwo(in, order);  // 144012
	std::cout << final_password << std::endl;
#endif
}
