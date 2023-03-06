// Advent Of Code 2022
// Day 13
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

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif




class Tokenizer
{
public:
	enum TokenTypes
	{
		eol,

		l_sqbra,
		r_sqbra,
		comma,
		value,

		invalid = -1
	};

	struct Token
	{
		TokenTypes type;
		int        value;

		Token()
			: Token(TokenTypes::invalid, 0)
		{
		}
		Token(TokenTypes _type, int _value)
			: type(_type)
			, value(_value)
		{
		}
	};

public:
	Tokenizer(const std::string& _source)
		: source(_source)
	{
		iCurr = 0;
	}

	Token GetToken()
	{
		while (PeekCh() == ' ' || PeekCh() == '\n')
			GetCh();

		if (PeekCh() == '\0')
			return Token(TokenTypes::eol, 0);

		if (PeekCh() == '[' || PeekCh() == ']')
			return Token(GetCh() == '[' ? TokenTypes::l_sqbra : TokenTypes::r_sqbra, 0);

		if (PeekCh() == ',')
		{
			GetCh();
			return Token(TokenTypes::comma, 0);
		}

		if ('0' <= PeekCh() && PeekCh() <= '9')
		{
			std::string s;
			while ('0' <= PeekCh() && PeekCh() <= '9')
				s += GetCh();

			return Token(TokenTypes::value, std::atoi(s.c_str()));
		}

		return Token(TokenTypes::invalid, 0);
	}

private:
	bool IsEos() const { ASSERT(iCurr <= source.size()); return iCurr == source.size();  }

	std::string::value_type PeekCh() const
	{
		return (iCurr < source.size() ? source[iCurr] : '\0');
	}
	std::string::value_type GetCh()
	{
		if (iCurr >= source.size())
		{
			ASSERT(false);
			return '\0';
		}
		return source[iCurr++];
	}

	const std::string  source;
	size_t             iCurr;
};

class LexAnalyzer
{
	// Produzione:
	//
	//    Term :=   value | List
	//    List :=   [Term, Term, ....]     0 or more times

public:
	struct Term
	{
		enum Type { List, Value,   Invalid = -1 };

		Type               type;
		std::vector<Term>  list;
		int                value;

		Term(Type _type = Invalid, int _value = 0)
			: type(_type)
			, value(_value)
		{
		}
		Term(const Term& o)
			: type(o.type)
			, list(o.list)
			, value(o.value)
		{
		}
		Term& operator = (const Term& o)
		{
			type = o.type;
			list = o.list;
			value = o.value;

			return *this;
		}
	};

public:
	LexAnalyzer(const std::string& source)
		: tokenizer(source)
		, tok()
	{
		ASSERT(tok.type == Tokenizer::invalid);
	}

	Term Parse()
	{
		ASSERT(CurrToken().type == Tokenizer::TokenTypes::invalid);
		NextToken();
		Term res = ParseTerm();
		ASSERT(NextToken().type == Tokenizer::TokenTypes::eol);
		return res;
	}

private:
	Term ParseTerm()
	{
		Term res;

		if (CurrToken().type == Tokenizer::TokenTypes::value)
		{
			res = Term(Term::Type::Value, CurrToken().value);
		}
		else if (CurrToken().type == Tokenizer::TokenTypes::l_sqbra)
		{
			res = ParseList();
		}
		else
		{
			ASSERT(false);
		}

		NextToken();
		return res;
	}

	Term ParseList()
	{
		ASSERT(CurrToken().type == Tokenizer::TokenTypes::l_sqbra);
		NextToken();

		Term res(Term::Type::List);
		while (CurrToken().type != Tokenizer::TokenTypes::r_sqbra)
		{
			if (CurrToken().type == Tokenizer::TokenTypes::comma)
			{
				ASSERT(!res.list.empty());
				NextToken();
			}
			res.list.push_back(ParseTerm());
		}

		return res;
	}

	const Tokenizer::Token& CurrToken() { return tok;  }
	const Tokenizer::Token& NextToken() { tok = tokenizer.GetToken(); return tok; }

	Tokenizer          tokenizer;
	Tokenizer::Token   tok;
};





LexAnalyzer::Term Convert(const std::string& line)
{
	LexAnalyzer analyzer(line);

	auto res = analyzer.Parse();
	ASSERT(res.type == LexAnalyzer::Term::Type::List);

	return res;
}

void ConvertTermValueToListValue(LexAnalyzer::Term& t)
{
	ASSERT(t.type == LexAnalyzer::Term::Value);
	t.type = LexAnalyzer::Term::List;
	t.list.assign(1, LexAnalyzer::Term(LexAnalyzer::Term::Value, t.value));
	t.value = 0;
}

int CompareTerms(LexAnalyzer::Term tl, LexAnalyzer::Term tr)
{
	if (tl.type == LexAnalyzer::Term::Value && tr.type == LexAnalyzer::Term::Value)
		return tl.value - tr.value;

	if (tl.type == LexAnalyzer::Term::List && tr.type == LexAnalyzer::Term::List)
	{
		for (size_t i = 0; i < tl.list.size(); ++i)
		{
			if (i >= tr.list.size())
				return +1;  // if right list runs out of items first, the inputs are not in the right order. 

			int cmp = CompareTerms(tl.list[i], tr.list[i]);
			if (cmp != 0)
				return cmp;
		}

		if (tl.list.size() < tr.list.size())
			return -1;  // If the left list runs out of items first, the inputs are in the right order
		ASSERT(tl.list.size() == tr.list.size());
		return 0;  // If the lists are the same length and no comparison makes a decision about the order, continue checking the next part of the input.
	}

	ASSERT(tl.type == LexAnalyzer::Term::List && tr.type == LexAnalyzer::Term::Value
		|| tl.type == LexAnalyzer::Term::Value && tr.type == LexAnalyzer::Term::List);

	if (tl.type == LexAnalyzer::Term::Value)
		ConvertTermValueToListValue(tl);
	else if (tr.type == LexAnalyzer::Term::Value)
		ConvertTermValueToListValue(tr);

	ASSERT(tl.type == LexAnalyzer::Term::List && tr.type == LexAnalyzer::Term::List);
	return CompareTerms(tl, tr);
}

int PartOne(std::istream& in)
{
	int index_sum = 0;
	for (int index = 1; true; ++index)
	{
		int i;
		std::string line[3];
		for (i = 0; i < 3 && std::getline(in, line[i]); ++i)
		{
			ASSERT(i < 2 && !line[i].empty()
				|| i == 2 && line[i].empty());
		}

		if (i < 3)
			break;

		LexAnalyzer::Term term_a = Convert(line[0]);
		LexAnalyzer::Term term_b = Convert(line[1]);
		const int cmp = CompareTerms(term_a, term_b);
		if (cmp < 0)  // right order ?
			index_sum += index;
	}

	return index_sum;
}

int PartTwo(std::istream& in)
{
	std::vector<std::pair<std::string, LexAnalyzer::Term>> packets;

	const std::string distress[2] = { "[[2]]", "[[6]]" };
	for (int i = 0; i < 2; ++i)
		packets.push_back(std::make_pair(distress[i], Convert(distress[i])));

	for (std::string line; std::getline(in, line); )
	{
		if (line.empty())
			continue;

		packets.push_back(std::make_pair(line, Convert(line)));
	}

	std::sort(packets.begin(), packets.end(), [](const std::pair<std::string, LexAnalyzer::Term>& l, const std::pair<std::string, LexAnalyzer::Term>& r) {return CompareTerms(l.second, r.second) < 0; });

	int index[2] = { 0, 0 };
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < packets.size(); ++j)
			if (distress[i] == packets[j].first)
			{
				index[i] = j+1;
				break;
			}

	return index[0] * index[1];
}

int main()
{
#if 0
	std::istringstream in(
R"([1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9])");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int sum = PartOne(in);  // 5196
	std::cout << sum << std::endl;
#else
	int prod = PartTwo(in);  // 22134
	std::cout << prod << std::endl;
#endif
}
