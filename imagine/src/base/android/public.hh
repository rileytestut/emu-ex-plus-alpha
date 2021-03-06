#pragma once

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

#include "sdk.hh"
#include <util/time/sys.hh>

namespace Gfx
{

#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
bool supportsAndroidDirectTexture();
bool supportsAndroidDirectTextureWhitelisted();
const char* androidDirectTextureError();
bool useAndroidDirectTexture();
void setUseAndroidDirectTexture(bool on);
#endif

bool supportsAndroidSurfaceTexture();
bool supportsAndroidSurfaceTextureWhitelisted();
bool useAndroidSurfaceTexture();
void setUseAndroidSurfaceTexture(bool on);

}

namespace Base
{

void setProcessPriority(int nice);
int processPriority();
bool apkSignatureIsConsistent();
const char *androidBuildDevice();
bool hasTrackball();
bool packageIsInstalled(const char *name);
TimeSys lastOrientationEventTime();

}

namespace Audio
{

bool hasLowLatency();

}

namespace Input
{

void initMOGA(bool notify);
void deinitMOGA();
bool mogaSystemIsActive();

}

extern bool glSyncHackBlacklisted, glSyncHackEnabled;
extern bool glPointerStateHack, glBrokenNpot;
