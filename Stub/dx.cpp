#include "dx_stub.h"
#include "miniwin_sdl.h"

#include "../types.h"
#include "stubs.h"

#ifndef NO_GLOBALS
Screen *gpBuffer;

IDirectDraw *lpDDInterface;
IDirectDrawSurface *lpDDSPrimary;
IDirectDrawSurface *lpDDSBackBuf;
IDirectDrawPalette *lpDDPalette;

char gbBackBuf; // unread
char gbEmulate; // unread
#endif

SDL_Window *sdl_window;
SDL_Renderer *sdl_renderer;
SDL_Texture *sdl_render_texture;

/** 32-bit in-memory backbuffer surface */
SDL_Surface *sdl_surface;

/** 8-bit surface wrapper around #gpBuffer */
SDL_Surface *sdl_pal_surface;
/** Currently active palette */
SDL_Palette *sdl_palette;

/**
 * Is #sdl_pal_surface dirty?
 *
 * This is required so the front buffer would not be updated twice per game loop in unlock_buf_priv()
 * which causes the cursor to flicker.
 */
bool surface_dirty;

//
// DirectDraw COM interface stub implementations
//

#define METHOD virtual __stdcall

class StubSurface : public IDirectDrawSurface
{
	METHOD HRESULT QueryInterface(REFIID refiid, LPVOID *lpvoid) { UNIMPLEMENTED(); };
	METHOD ULONG AddRef() { UNIMPLEMENTED(); };
	METHOD ULONG Release() { UNIMPLEMENTED(); };

	METHOD HRESULT AddAttachedSurface(LPDIRECTDRAWSURFACE lpDDSAttachedSurface) { UNIMPLEMENTED(); }
	METHOD HRESULT AddOverlayDirtyRect(LPRECT lpRect) { UNIMPLEMENTED(); }
	METHOD HRESULT Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
	                   LPDDBLTFX lpDDBltFx)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT BltBatch(LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) { UNIMPLEMENTED(); }

	METHOD HRESULT BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans)
	{
		DUMMY_ONCE();

		assert(lpDDSrcSurface == lpDDSBackBuf);

		int w = lpSrcRect->right - lpSrcRect->left + 1;
		int h = lpSrcRect->bottom - lpSrcRect->top + 1;

		SDL_Rect src_rect = {lpSrcRect->left, lpSrcRect->top, w, h};
		SDL_Rect dst_rect = {(int)dwX, (int)dwY, w, h};

		// Convert from 8-bit to 32-bit
		SDL_CHECK(SDL_BlitSurface(sdl_pal_surface, &src_rect, sdl_surface, &dst_rect));

		surface_dirty = true;
		return S_OK;
	}

	METHOD HRESULT DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface) { UNIMPLEMENTED(); }
	METHOD HRESULT EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT Flip(LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags) { UNIMPLEMENTED(); }
	METHOD HRESULT GetAttachedSurface(LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE *lplpDDAttachedSurface)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT GetBltStatus(DWORD dwFlags) { UNIMPLEMENTED(); }
	METHOD HRESULT GetCaps(LPDDSCAPS lpDDSCaps) { UNIMPLEMENTED(); }
	METHOD HRESULT GetClipper(LPDIRECTDRAWCLIPPER *lplpDDClipper) { UNIMPLEMENTED(); }
	METHOD HRESULT GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey) { UNIMPLEMENTED(); }

	METHOD HRESULT GetDC(HDC *lphDC)
	{
		DUMMY_ONCE();
		return S_OK;
	}

	METHOD HRESULT GetFlipStatus(DWORD dwFlags) { UNIMPLEMENTED(); }
	METHOD HRESULT GetOverlayPosition(LPLONG lplX, LPLONG lplY) { UNIMPLEMENTED(); }
	METHOD HRESULT GetPalette(LPDIRECTDRAWPALETTE *lplpDDPalette) { UNIMPLEMENTED(); }
	METHOD HRESULT GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat) { UNIMPLEMENTED(); }
	METHOD HRESULT GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc) { UNIMPLEMENTED(); }
	METHOD HRESULT Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc) { UNIMPLEMENTED(); }

	METHOD HRESULT IsLost()
	{
		DUMMY_ONCE();
		return S_OK;
	}

	METHOD HRESULT Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
	{
		UNIMPLEMENTED();
	}

	METHOD HRESULT ReleaseDC(HDC hDC)
	{
		DUMMY_ONCE();
		return S_OK;
	}

	METHOD HRESULT Restore() { UNIMPLEMENTED(); }
	METHOD HRESULT SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper) { UNIMPLEMENTED(); }
	METHOD HRESULT SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey) { UNIMPLEMENTED(); }
	METHOD HRESULT SetOverlayPosition(LONG lX, LONG lY) { UNIMPLEMENTED(); }
	METHOD HRESULT SetPalette(LPDIRECTDRAWPALETTE lpDDPalette) { UNIMPLEMENTED(); }
	METHOD HRESULT Unlock(LPVOID lpSurfaceData) { UNIMPLEMENTED(); }
	METHOD HRESULT UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect,
	                             DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT UpdateOverlayDisplay(DWORD dwFlags) { UNIMPLEMENTED(); }
	METHOD HRESULT UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference) { UNIMPLEMENTED(); }
};

class StubPalette : public IDirectDrawPalette
{
	METHOD HRESULT QueryInterface(REFIID refiid, LPVOID *lpvoid) { UNIMPLEMENTED(); };
	METHOD ULONG AddRef() { UNIMPLEMENTED(); };
	METHOD ULONG Release() { UNIMPLEMENTED(); };

	METHOD HRESULT GetCaps(LPDWORD lpdwCaps) { UNIMPLEMENTED(); };
	METHOD HRESULT GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries)
	{
		UNIMPLEMENTED();
	};
	METHOD HRESULT Initialize(LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable) { UNIMPLEMENTED(); };
	METHOD HRESULT SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
	{
		UNIMPLEMENTED();
	};
};

class StubDraw : public IDirectDraw
{
	METHOD HRESULT QueryInterface(REFIID refiid, LPVOID *lpvoid) { UNIMPLEMENTED(); };
	METHOD ULONG AddRef() { UNIMPLEMENTED(); };
	METHOD ULONG Release() { UNIMPLEMENTED(); };

	METHOD HRESULT Compact() { UNIMPLEMENTED(); }
	METHOD HRESULT CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette,
	                             IUnknown *pUnkOuter)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface,
	                             IUnknown *pUnkOuter)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface, LPDIRECTDRAWSURFACE *lplpDupDDSurface)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext,
	                                LPDDENUMMODESCALLBACK lpEnumModesCallback)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC lpDDSD, LPVOID lpContext,
	                            LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
	{
		UNIMPLEMENTED();
	}
	METHOD HRESULT FlipToGDISurface() { UNIMPLEMENTED(); }
	METHOD HRESULT GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps) { UNIMPLEMENTED(); }
	METHOD HRESULT GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc) { UNIMPLEMENTED(); }
	METHOD HRESULT GetFourCCCodes(LPDWORD lpNumCodes, LPDWORD lpCodes) { UNIMPLEMENTED(); }
	METHOD HRESULT GetGDISurface(LPDIRECTDRAWSURFACE *lplpGDIDDSurface) { UNIMPLEMENTED(); }
	METHOD HRESULT GetMonitorFrequency(LPDWORD lpdwFrequency) { UNIMPLEMENTED(); }
	METHOD HRESULT GetScanLine(LPDWORD lpdwScanLine) { UNIMPLEMENTED(); }
	METHOD HRESULT GetVerticalBlankStatus(BOOL *lpbIsInVB) { UNIMPLEMENTED(); }
	METHOD HRESULT Initialize(GUID *lpGUID) { UNIMPLEMENTED(); }
	METHOD HRESULT RestoreDisplayMode() { UNIMPLEMENTED(); }
	METHOD HRESULT SetCooperativeLevel(HWND hWnd, DWORD dwFlags) { UNIMPLEMENTED(); }
	METHOD HRESULT SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP) { UNIMPLEMENTED(); }
	METHOD HRESULT WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
	{
		DUMMY();
		return S_OK;
	}
};

static StubDraw stub_draw;
static StubSurface stub_surface;
static StubPalette stub_palette;

//
// Main functions
//

void sdl_init_video()
{
	SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));

	sdl_window =
	    SDL_CreateWindow("devil-test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	assert(sdl_window);

	// Hack since ShowCursor is called before dx_init()
	SDL_ShowCursor(SDL_DISABLE);

	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
	assert(sdl_renderer);

	// FUTURE: Use SDL_CreateRGBSurfaceWithFormat with SDL_PIXELFORMAT_RGBA8888
	sdl_surface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	assert(sdl_surface);

	sdl_render_texture =
	    SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	assert(sdl_render_texture);

	sdl_palette = SDL_AllocPalette(256);
	assert(sdl_palette);
}

void __fastcall dx_init(HWND hWnd)
{
	DUMMY();

	sdl_init_video();

	gbActive = TRUE;

	lpDDInterface = &stub_draw;
	lpDDSPrimary = &stub_surface;
	lpDDSBackBuf = &stub_surface;
	lpDDPalette = &stub_palette;
}

/** Copy the palette surface to the main backbuffer */
void sdl_update_entire_surface()
{
	assert(sdl_surface && sdl_pal_surface);

	SDL_Rect src_rect = {64, 160, 640, 480};
	SDL_CHECK(SDL_BlitSurface(sdl_pal_surface, &src_rect, sdl_surface, NULL));
}

void sdl_present_surface()
{
	assert(!SDL_MUSTLOCK(sdl_surface));
	SDL_CHECK(SDL_UpdateTexture(sdl_render_texture, NULL, sdl_surface->pixels, sdl_surface->pitch));

	SDL_CHECK(SDL_RenderCopy(sdl_renderer, sdl_render_texture, NULL, NULL));
	SDL_RenderPresent(sdl_renderer);

	surface_dirty = false;
}

void __cdecl lock_buf_priv()
{
	const int pitch = 640 + 64 + 64;

	if (!gpBuffer) {
		gpBuffer = (Screen *)malloc(sizeof(Screen));
		gpBufEnd += (unsigned int)gpBuffer;

		sdl_pal_surface = SDL_CreateRGBSurfaceFrom(gpBuffer, pitch, 160 + 480 + 16, 8, pitch, 0, 0, 0, 0);
		assert(sdl_pal_surface);

		SDL_CHECK(SDL_SetSurfacePalette(sdl_pal_surface, sdl_palette));
	}
}

void __cdecl unlock_buf_priv()
{
	gpBufEnd -= (unsigned int)gpBufEnd;

	if (!surface_dirty) {
		return;
	}

	sdl_present_surface();
}

void __cdecl dx_reinit()
{
	UNIMPLEMENTED();
}

//
// Storm functions
//

BOOL STORMAPI SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4)
{
	DUMMY_PRINT("first: %d num: %d", firstentry, numentries);

	assert(firstentry == 0);
	assert(numentries == 256);

	SDL_Color colors[256];
	for (int i = firstentry; i < numentries; i++) {
		SDL_Color *c = &colors[i];
		PALETTEENTRY *p = &pPalEntries[i];
		c->r = p->peRed;
		c->g = p->peGreen;
		c->b = p->peBlue;
		c->a = SDL_ALPHA_OPAQUE;
	}

	assert(sdl_palette);
	SDL_CHECK(SDL_SetPaletteColors(sdl_palette, colors, firstentry, numentries));

	if (sdl_pal_surface) {
		sdl_update_entire_surface();
		sdl_present_surface();
	}

	return TRUE;
}

//
// Windows API functions
//

WINBOOL WINAPI SetCursorPos(int X, int Y)
{
	DUMMY();
	assert(sdl_window);
	SDL_WarpMouseInWindow(sdl_window, X, Y);
	return TRUE;
}

int WINAPI ShowCursor(WINBOOL bShow)
{
	DUMMY_PRINT("%d", bShow);
	if (sdl_window) {
		SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
	}
	return bShow;
}

WINBOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
	DUMMY_ONCE();

	assert(sdl_window);
	SDL_SetWindowTitle(sdl_window, lpString);

	return TRUE;
}
