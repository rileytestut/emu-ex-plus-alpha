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

#include "SurfaceTextureBufferImage.hh"
#include "../GLStateCache.hh"
#include "../private.hh"
#include <base/android/private.hh>

namespace Gfx
{

void SurfaceTextureBufferImage::init(int tid, IG::Pixmap &pixmap)
{
	using namespace Base;
	using namespace Gfx;

	desc.tid = tid;
	new(&pix) IG::Pixmap(pixmap.format);
	pix.init(nullptr, pixmap.x, pixmap.y);
	logMsg("creating SurfaceTexture with id %d", tid);
	auto jEnv = eEnv();
	surfaceTex = jEnv->NewObject(surfaceTextureConf.jSurfaceTextureCls, surfaceTextureConf.jSurfaceTexture.m, tid);
	assert(surfaceTex);
	surfaceTex = Base::jniThreadNewGlobalRef(jEnv, surfaceTex);
	//jEnv->CallVoidMethod(surfaceTex, jSetDefaultBufferSize.m, x, y);

	surface = jEnv->NewObject(surfaceTextureConf.jSurfaceCls, surfaceTextureConf.jSurface.m, surfaceTex);
	assert(surface);
	surface = Base::jniThreadNewGlobalRef(jEnv, surface);

	// ANativeWindow_fromSurfaceTexture was removed from Android 4.1
	//nativeWin = surfaceTextureConf.ANativeWindow_fromSurfaceTexture(eEnv(), surfaceTex);
	nativeWin = ANativeWindow_fromSurface(jEnv, surface);
	assert(nativeWin);
	logMsg("got native window %p from Surface %p", nativeWin, surface);
	replace(pixmap, 0);
}

void SurfaceTextureBufferImage::write(IG::Pixmap &p, uint hints)
{
	IG::Pixmap *texturePix = lock(0, 0, p.x, p.y);
	if(!texturePix)
	{
		logWarn("unable to lock texture");
		return;
	}
	p.copy(0, 0, 0, 0, *texturePix, 0, 0);
	unlock();
}

void SurfaceTextureBufferImage::write(IG::Pixmap &p, uint hints, uint alignment)
{
	write(p, hints);
}

void SurfaceTextureBufferImage::replace(IG::Pixmap &pixmap, uint hints)
{
	int winFormat = pixelFormatToDirectAndroidFormat(pixmap.format);
	assert(winFormat);
	if(ANativeWindow_setBuffersGeometry(nativeWin, pixmap.x, pixmap.y, winFormat) < 0)
	{
		logErr("error in ANativeWindow_setBuffersGeometry");
	}
}

IG::Pixmap *SurfaceTextureBufferImage::lock(uint x, uint y, uint xlen, uint ylen, IG::Pixmap *fallback)
{
	ANativeWindow_Buffer buffer;
	//ARect rect = { x, y, xlen, ylen };
	ANativeWindow_lock(nativeWin, &buffer, 0/*&rect*/);
	pix.pitch = buffer.stride * pix.format.bytesPerPixel;
	pix.data = (char*)buffer.bits;
	//logMsg("locked buffer %p with pitch %d", buffer.bits, buffer.stride);
	return &pix;
}

void SurfaceTextureBufferImage::unlock(IG::Pixmap *pix, uint hints)
{
	using namespace Base;
	ANativeWindow_unlockAndPost(nativeWin);
	surfaceTextureConf.jUpdateTexImage(eEnv(), surfaceTex);
	// texture implicitly bound in updateTexImage()
	glState.bindTextureState.GL_TEXTURE_EXTERNAL_OES_state = desc.tid;
}

void SurfaceTextureBufferImage::deinit()
{
	using namespace Base;
	using namespace Gfx;
	logMsg("deinit SurfaceTexture, releasing window");
	ANativeWindow_release(nativeWin);
	auto jEnv = eEnv();
	surfaceTextureConf.jSurfaceRelease(jEnv, surface);
	Base::jniThreadDeleteGlobalRef(jEnv, surface);
	surfaceTextureConf.jSurfaceTextureRelease(jEnv, surfaceTex);
	Base::jniThreadDeleteGlobalRef(jEnv, surfaceTex);
	freeTexRef(desc.tid);
	desc.tid = 0;

	if(surfaceTextureConf.texture2dBindingHack)
	{
		GLint realTexture = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &realTexture);
		if(glState.bindTextureState.GL_TEXTURE_2D_state != (GLuint)realTexture)
		{
			logMsg("setting GL_TEXTURE_2D binding state to %d, should be %d", realTexture, glState.bindTextureState.GL_TEXTURE_2D_state);
		}
		glState.bindTextureState.GL_TEXTURE_2D_state = realTexture;
	}
}

}
