/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <util/string/generic.h>
#include <util/ansiTypes.h>
#include <string.h>
#include <assert.h>
#include <mem/interface.h>

static const char pathSeparator[] = { '/'
#ifdef CONFIG_BASE_WIN32
		, '\\'
#endif
};
static const uint numPathSeparators = sizeofArray(pathSeparator);

template <class T>
static T *dirNameCutoffPoint(T *path)
{
	T *cutoffPoint = nullptr;
	for(uint i = 0; i < numPathSeparators; i++)
	{
		T *possibleCutoff = strrchr(path, pathSeparator[i]);
		if(possibleCutoff > cutoffPoint)
			cutoffPoint = possibleCutoff;
	}
	return cutoffPoint;
}

void dirNameInPlace(char *path)
{
	char *cutoffPoint = dirNameCutoffPoint(path);

	if(cutoffPoint != nullptr)
		*cutoffPoint = 0;
	else strcpy(path, ".");
}

void dirName(const char *path, char *pathOut)
{
	const char *cutoffPoint = dirNameCutoffPoint(path);

	if(cutoffPoint != nullptr)
	{
		size_t cpySize = cutoffPoint - path;
		memcpy(pathOut, path, cpySize);
		pathOut[cpySize] = 0;
	}
	else strcpy(pathOut, ".");
}

static char *dirNameCpy(char *path)
{
	char *cutoffPoint = dirNameCutoffPoint(path);
	char *pathOut;
	if(cutoffPoint != nullptr)
	{
		size_t cpySize = cutoffPoint - path;
		pathOut = (char*)mem_alloc(cpySize + 1);
		memcpy(pathOut, path, cpySize);
		pathOut[cpySize] = 0;
	}
	else
	{
		pathOut = (char*)mem_alloc(2);
		strcpy(pathOut, ".");
	}
	return pathOut;
}

template <class T>
T baseNamePos(T path)
{
	T pos = path;
	for(uint i = 0; i < numPathSeparators; i++)
	{
		T possiblePos = strrchr(path, pathSeparator[i]);
		if(possiblePos > pos)
			pos = possiblePos+1;
	}
	return pos;
}

template char* baseNamePos<char*>(char* path);
template const char* baseNamePos<const char*>(const char* path);

void baseNameInPlace(char *path)
{
	char *copyPoint = baseNamePos(path);
	if(copyPoint != nullptr)
		strcpy(path, copyPoint);
}

void baseName(const char *path, char *pathOut)
{
	const char *cutoffPoint = baseNamePos(path);

	assert(*cutoffPoint != 0); // TODO: other cases
	strcpy(pathOut, cutoffPoint);
}
