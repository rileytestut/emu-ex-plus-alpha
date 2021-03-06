#pragma once
#include <io/api/stdio.hh>

namespace VorbisIOAPI
{
	static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource)
	{
		return fread(ptr, size, nmemb, (Io*)datasource);
	}

	static int seek(void *datasource, ogg_int64_t offset, int whence)
	{
		return fseek((Io*)datasource, offset, whence);
	}

	static int close(void *datasource)
	{
		return fclose((Io*)datasource);
	}

	static long tell(void *datasource)
	{
		return ftell((Io*)datasource);
	}
}

static ov_callbacks imagineVorbisIO =
{
	VorbisIOAPI::read, VorbisIOAPI::seek, VorbisIOAPI::close, VorbisIOAPI::tell
};

static ov_callbacks imagineVorbisIONoClose =
{
	VorbisIOAPI::read, VorbisIOAPI::seek, nullptr, VorbisIOAPI::tell
};
