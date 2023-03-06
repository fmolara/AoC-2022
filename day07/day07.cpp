// Advent Of Code 2022
// Day 07
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

#include "..\stuffs\Splitter.h"

#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(x)   assert(x)
#else
	#define ASSERT(x)   ((void)0)
#endif



class File;
class Directory;

class FS_Element
{
public:
	FS_Element(const std::string& _name, Directory* _pParent)
		: name(_name)
		, pParent(_pParent)
	{
		ASSERT(name.find('/') == std::string::npos);
		ASSERT( name.empty() && !pParent
		    || !name.empty() && pParent);
	}

	const std::string& Name() const { return name; }
	std::string FullPathName() const;

	Directory* Parent() const { return pParent; }

	virtual int Size() const = 0;
	virtual void Visit(std::function<void(const FS_Element*, int)> functor, int level = 0) const
	{
		functor(this, level);
	}

private:
	std::string name;
	Directory*  pParent;
};

class File
	: public FS_Element
{
public:
	File(const std::string& name, int size, Directory* pParent)
		: FS_Element(name, pParent)
		, file_size(size)
	{
	}

	virtual int Size() const override
	{
		return file_size;
	}

private:
	int file_size;
};

class Directory
	: public FS_Element
{
public:
	Directory(const std::string& name, Directory* pParent)
		: FS_Element(name, pParent)
	{
	}

	Directory* Child(const std::string& name)
	{
		for (const auto e : content)
		{
			Directory* d = dynamic_cast<Directory*>(e);
			if (d && d->Name() == name)
				return d;
		}
		return nullptr;
	}

	virtual void Visit(std::function<void(const FS_Element*, int)> functor, int level = 0) const override
	{
		FS_Element::Visit(functor, level);

		for (const auto e : content)
			e->Visit(functor, level + 1);
	}

	virtual int Size() const override
	{
		int total = 0;
		for (const auto e : content)
			total += e->Size();
		return total;
	}

	void AddFile(const std::string& name, int size) { content.push_back(new File(name, size, this)); }
	void AddDir(const std::string& name)            { content.push_back(new Directory(name, this)); }

private:
	std::vector<FS_Element*>  content;
};

std::string FS_Element::FullPathName() const { return (pParent ? pParent->FullPathName() : std::string()) + '/' + name; }





Directory ParseFS(std::istream& in)
{
	Directory root("", nullptr);
	Directory* pCurr_wd = nullptr;

	bool is_ls = false;

	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto parts = split(line, " ");
		if (parts[0] == "$")
		{
			is_ls = false;

			if (parts[1] == "cd")
			{
				ASSERT(parts.size() == 3 && !parts[2].empty());

				const auto& path = parts[2];
				if (path[0] == '/')
				{
					ASSERT(path == "/");  // solo per verificare se succede
					pCurr_wd = &root;
				}
				else if (path == "..")
				{
					if (pCurr_wd)
					{
						if (pCurr_wd->Parent())  // è verificato sperimentalmente che c'è il "cd .." mentre si è nella directory /
							pCurr_wd = pCurr_wd->Parent();
						else
							ASSERT(pCurr_wd == &root);
					}
					else
						ASSERT(false);
				}
				else
				{
					ASSERT('a' <= path[0] && path[0] <= 'z');
					ASSERT(path.find('/') == std::string::npos);

					ASSERT(pCurr_wd && pCurr_wd->Child(path));
					pCurr_wd = pCurr_wd->Child(path);
				}
			}
			else if (parts[1] == "ls")
			{
				ASSERT(parts.size() == 2);
				is_ls = true;
			}
			else
			{
				ASSERT(false);
				std::cout << parts[1] << std::endl;
			}
		}
		else if (parts[0] == "dir")
		{
			ASSERT(parts.size() == 2 && !parts[1].empty());

			ASSERT(is_ls);
			pCurr_wd->AddDir(parts[1]);
		}
		else
		{
			int value = std::stoi(parts[0]);
			ASSERT(parts[0] == std::to_string(value));

			ASSERT(parts.size() == 2 && !parts[1].empty());

			ASSERT(is_ls);
			pCurr_wd->AddFile(parts[1], value);
		}
	}

	if (0)
	{
		auto print = [](const FS_Element* e, int level) {

			std::string type;
			if (nullptr != dynamic_cast<const Directory*>(e))
				type = "dir";
			else if (nullptr != dynamic_cast<const File*>(e))
				type = "file";
			else
				ASSERT(false);

			std::cout << std::string(level*2, ' ') << "- " << e->Name() << " (" << type <<", size=" << e->Size() << ")" << std::endl;
		};

		root.Visit(print);
	}

	return std::move(root);
}

int PartOne(std::istream & in)
{
	Directory root = ParseFS(in);

	const int THRESHOLD = 100000;
	int total = 0;
	{
		auto summarize = [&](const FS_Element* e, int level) {

			if (nullptr == dynamic_cast<const Directory*>(e))
				return;

			int size = e->Size();
			if (size <= THRESHOLD)
				total += size;
		};

		root.Visit(summarize);
	}
	return total;
}

int PartTwo(std::istream& in)
{
	Directory root = ParseFS(in);

	const int TOTALDISK = 70000000;
	const int NEEDED =  30000000;

	const int totalFs = root.Size();
	const int available = TOTALDISK - totalFs;
	ASSERT(0 <= available && available < NEEDED);

	const Directory* pBestDir = nullptr;
	int bestSize = 0;
	{
		auto finder = [&](const FS_Element* e, int level) {

			if (nullptr == dynamic_cast<const Directory*>(e))
				return;

			int size = e->Size();
			if (available + size < NEEDED)
				return;

			if (!pBestDir || bestSize > size)
			{
				pBestDir = dynamic_cast<const Directory*>(e);
				bestSize = size;
			}
		};

		root.Visit(finder);
	}
	return bestSize;
}
int main()
{
#if 0
	std::istringstream in(
R"($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int size = PartOne(in);
	std::cout << size << std::endl;  // 1667443
#else
	int size = PartTwo(in);
	std::cout << size << std::endl;  // 8998590
#endif
}
