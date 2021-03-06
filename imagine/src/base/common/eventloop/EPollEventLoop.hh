#pragma once

#include <config.h>
#include <sys/epoll.h>
#include <base/eventLoopDefs.hh>

namespace Base
{

static const int POLLEV_IN = EPOLLIN, POLLEV_OUT = EPOLLOUT, POLLEV_ERR = EPOLLERR, POLLEV_HUP = EPOLLHUP;

struct EPollEventLoopFileSource
{
	PollEventDelegate callback;
	int fd_ = -1;

	constexpr EPollEventLoopFileSource() {}
};

using EventLoopFileSourceImpl = EPollEventLoopFileSource;

}
