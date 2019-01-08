/* luailut.c
 *
 * author      : Alexander Marinov (alekmarinov@gmail.com)
 * project     : luadevil (http://luadevil.luaforge.net)
 * description : Binds DevIL to Lua
 * copyright   : The same as Lua license (http://www.lua.org/license.html)
 *
 *******************************************************************************************/

#include <string.h>
#include <malloc.h>
#include <lauxlib.h>
#include <IL/il.h>
#include <IL/ilut.h>

#ifdef WITH_ILUT

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_luailut(lua_State* L);
extern const char *get_str_il_enum(ILenum num);

#define ENUM_ERROR (ILenum)-2

typedef struct ilut_str_value {
    const char *str;
    ILenum value;
} ilut_str_value;

static const ilut_str_value ilut_mask[] = {
    {"OPENGL_BIT", 0x00000001},
    {"D3D_BIT", 0x00000002},
    {"ALL_ATTRIB_BITS", 0x000FFFFF},
};

static const ilut_str_value ilut_str[] = {
    {"INVALID_ENUM", 0x0501},
    {"OUT_OF_MEMORY", 0x0502},
    {"INVALID_VALUE", 0x0505},
    {"ILLEGAL_OPERATION", 0x0506},
    {"INVALID_PARAM", 0x0509},
    {"COULD_NOT_OPEN_FILE", 0x050A},
    {"STACK_OVERFLOW", 0x050E},
    {"STACK_UNDERFLOW", 0x050F},
    {"BAD_DIMENSIONS", 0x0511},
    {"NOT_SUPPORTED", 0x0550},
    {"PALETTE_MODE", 0x0600},
    {"OPENGL_CONV", 0x0610},
    {"D3D_MIPLEVELS", 0x0620},
    {"MAXTEX_WIDTH", 0x0630},
    {"MAXTEX_HEIGHT", 0x0631},
    {"MAXTEX_DEPTH", 0x0632},
    {"GL_USE_S3TC", 0x0634},
    {"D3D_USE_DXTC", 0x0634},
    {"GL_GEN_S3TC", 0x0635},
    {"D3D_GEN_DXTC", 0x0635},
    {"S3TC_FORMAT", 0x0705},
    {"DXTC_FORMAT", 0x0705},
    {"D3D_POOL", 0x0706},
    {"D3D_ALPHA_KEY_COLOR", 0x0707},
    {"D3D_ALPHA_KEY_COLOUR", 0x0707},
    {"FORCE_INTEGER_FORMAT", 0x0636},
    {"GL_AUTODETECT_TEXTURE_TARGET", 0x0807},
    {"OPENGL", 0},
    {"ALLEGRO", 1},
    {"WIN32", 2},
    {"DIRECT3D8", 3},
    {"DIRECT3D9", 4},
    {"X11", 5},
    {"DIRECT3D10", 6},
};

/* convert string to IL enum */
static ILenum get_enum_from(const ilut_str_value* enums, const char *str, int n)
{
    int i = 0;

    while(enums[i].str != 0)
    {
        if(strncmp(str, enums[i].str, n) == 0 && enums[i].str[n] == 0)
            return enums[i].value;

        i++;
    }
    return ENUM_ERROR;
}

/* fetch string from lua stack and convert to IL enum */
static ILenum get_ilut_enum(lua_State* L, int index)
{
    size_t len;
    const char *str = luaL_checklstring(L, 1, &len);
    return get_enum_from(ilut_str, str, len);
}

/* fetch string from lua stack and convert to IL bit mask */
static ILuint get_ilut_mask(lua_State *L, int index)
{
    unsigned int i;
    size_t slen;
    const char *str = luaL_checklstring(L, index, &slen);
    ILenum temp = 0, ret = 0;

    for(i = 0; i < slen; i++)
    {
        if(str[i] == ',')
        {
            temp = get_enum_from(ilut_mask, str, i);
            if(temp != ENUM_ERROR)
                ret |= temp;

            str += i+1;
            i = 0;
        }
    }
    temp = get_enum_from(ilut_mask, str, strlen(str));

    if(temp == ENUM_ERROR)
    {
        if(ret == 0)
            return ENUM_ERROR;
        return ret;
    }

    return ret | temp;
}

/* Lua bindings to ILUT */

/* GetError() -> string */
static int il_geterror(lua_State* L) 
{
    const char* error = get_str_il_enum(ilGetError());
    if (!error)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "UNKNOWN_ERROR");
        return 2;
    }
    lua_pushstring(L, error);
    return 1;
}

static int il_result_bool(lua_State* L, ILboolean rc)
{
	if (!rc)
	{
		lua_pushnil(L);
		il_geterror(L);
		return 2;
	}
    lua_pushboolean(L, 1);
    return 1;
}

static int il_result_int(lua_State* L, int rc)
{
	if (!rc)
	{
		lua_pushnil(L);
		il_geterror(L);
		return 2;
	}
    lua_pushinteger(L, rc);
    return 1;
}


/* Disable(enum) -> boolean */
static int ilut_disable(lua_State* L) 
{
    lua_pushboolean(L, ilutDisable(get_ilut_enum(L, 1)));
    return 1;
}

/* Enable(enum) -> boolean */
static int ilut_enable(lua_State* L) 
{
    lua_pushboolean(L, ilutEnable(get_ilut_enum(L, 1)));
    return 1;
}

/* GetBoolean(enum) -> boolean */
static int ilut_getboolean(lua_State* L) 
{
    lua_pushboolean(L, ilutGetBoolean(get_ilut_enum(L, 1)));
    return 1;
}

/* GetInteger(enum) -> number */
static int ilut_getinteger(lua_State* L) 
{
    lua_pushinteger(L, ilutGetInteger(get_ilut_enum(L, 1)));
    return 1;
}

/* GetString(enum) -> string */
static int ilut_getstring(lua_State* L) 
{
    lua_pushstring(L, ilutGetString(get_ilut_enum(L, 1)));
    return 1;
}

/* IsDisabled(enum) -> boolean */
static int ilut_isdisabled(lua_State* L) 
{
    lua_pushboolean(L, ilutIsDisabled(get_ilut_enum(L, 1)));
    return 1;
}

/* IsEnabled(enum) -> boolean */
static int ilut_isenabled(lua_State* L) 
{
    lua_pushboolean(L, ilutIsEnabled(get_ilut_enum(L, 1)));
    return 1;
}

/* PopAttrib() -> none */
static int ilut_popattrib(lua_State* L) 
{
    ilutPopAttrib();
    return 0;
}

/* PushAttrib(mask) -> none */
static int ilut_pushattrib(lua_State* L) 
{
    ilutPushAttrib(get_ilut_mask(L, 1));
    return 0;
}

/* SetInteger(enum, number) -> none */
static int ilut_setinteger(lua_State* L) 
{
    ilutSetInteger(get_ilut_enum(L, 1), luaL_checkinteger(L, 2));
    return 0;
}

/* Renderer(enum) -> none */
static int ilut_renderer(lua_State* L) 
{
    return il_result_bool(L, ilutRenderer(get_ilut_enum(L, 1)));
}

#ifdef ILUT_USE_OPENGL

/* GLBindTexImage() -> Number */
static int ilut_glbindteximage(lua_State* L) 
{
    return il_result_int(L, ilutGLBindTexImage());
}

/* GLBindMipmaps() -> Number */
static int ilut_glbindmipmaps(lua_State* L) 
{
    return il_result_int(L, ilutGLBindMipmaps());
}

/* GLBuildMipmaps() -> boolean */
static int ilut_glbuildmipmaps(lua_State* L) 
{
    return il_result_bool(L, ilutGLBuildMipmaps());
}

/* GLLoadImage(FileName) -> Number */
static int ilut_glloadimage(lua_State* L) 
{
    return il_result_int(L, ilutGLLoadImage((ILstring)luaL_checkstring(L, 1)));
}

/* GLScreen() -> boolean */
static int ilut_glscreen(lua_State* L) 
{
    return il_result_bool(L, ilutGLScreen());
}

/* GLScreenie() -> boolean */
static int ilut_glscreenie(lua_State* L) 
{
    return il_result_bool(L, ilutGLScreenie());
}

/* GLSaveImage(FileName, TexID) -> boolean */
static int ilut_glsaveimage(lua_State* L) 
{
    return il_result_bool(L, ilutGLSaveImage((ILstring)luaL_checkstring(L, 1), luaL_checkinteger(L, 2)));
}

/* GLSubTex2D(TexID, XOff, YOff) -> boolean */
static int ilut_glsubtex2d(lua_State* L) 
{
    return il_result_bool(L, ilutGLSubTex2D(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                            luaL_checkinteger(L, 3)));
}

/* GLSubTex3D(TexID, XOff, YOff, ZOff) -> boolean */
static int ilut_glsubtex3d(lua_State* L) 
{
    return il_result_bool(L, ilutGLSubTex3D(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                            luaL_checkinteger(L, 3), luaL_checkinteger(L, 4)));
}

/* GLSetTex2D(TexID) -> boolean */
static int ilut_glsettex2d(lua_State* L) 
{
    return il_result_bool(L, ilutGLSetTex2D(luaL_checkinteger(L, 1)));
}

/* GLSetTex3D(TexID) -> boolean */
static int ilut_glsettex3d(lua_State* L) 
{
    return il_result_bool(L, ilutGLSetTex3D(luaL_checkinteger(L, 1)));
}

/* GLTexImage(Level) -> boolean */
static int ilut_glteximage(lua_State* L) 
{
    return il_result_bool(L, ilutGLTexImage(luaL_checkinteger(L, 1)));
}

#endif

#ifdef ILUT_USE_ALLEGRO

/* AllegLoadImage(FileName) -> BITMAP* */
static int ilut_allegloadimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutAllegLoadImage(luaL_checkstring(L, 1)));
    return 1;
}

/* ConvertToAlleg(PALETTE) -> BITMAP* */
static int ilut_converttoalleg(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutConvertToAlleg((PALETTE)lua_topointer(L, 1)));
    return 1;
}

#endif

#ifdef ILUT_USE_SDL

/* ConvertToSDLSurface(flags) -> SDL_Surface* */
static int ilut_converttosdlsurface(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutConvertToSDLSurface(luaL_checkinteger(L, 1)));
    return 1;
}

/* SDLSurfaceLoadImage(FileName) -> SDL_Surface* */
static int ilut_sdlsurfaceloadimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutSDLSurfaceLoadImage(luaL_checkstring(L, 1)));
    return 1;
}

/* SDLSurfaceFromBitmap(SDL_Surface*) -> boolean */
static int ilut_sdlsurfacefrombitmap(lua_State* L) 
{
    lua_pushboolean(L, ilutSDLSurfaceFromBitmap(lua_topointer(L, 1)));
    return 1;
}

#endif

#ifdef ILUT_USE_BEOS

/* ConvertToBBitmap(none) -> BBitmap */
static int ilut_converttobbitmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutConvertToBBitmap());
    return 1;
}

#endif

#ifdef ILUT_USE_WIN32

/* ConvertToHBitmap(hDC) -> HBITMAP */
static int ilut_converttohbitmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutConvertToHBitmap((HDC)lua_topointer(L, 1)));
    return 1;
}

/* ConvertSliceToHBitmap(hDC, slice) -> HBITMAP */
static int ilut_convertslicetohbitmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutConvertSliceToHBitmap((HDC)lua_topointer(L, 1), luaL_checkinteger(L, 2)));
    return 1;
}

/* FreePaddedData(Data) -> none */
static int ilut_freepaddeddata(lua_State* L) 
{
    ilutFreePaddedData((ILubyte *)lua_topointer(L, 1));
    return 0;
}

/* GetBmpInfo() -> {size, width, height, planes,
                    bitcount, compression, sizeimage,
                    xpelspermeter, ypelspermeter, clrused,
                    clrimportant} */
static int ilut_getbmpinfo(lua_State* L) 
{
    BITMAPINFO Info;
	Info.bmiHeader.biSize = -666;
    ilutGetBmpInfo(&Info);
	if (Info.bmiHeader.biSize == -666)
	{
		return il_result_bool(L, 0);
	}

    lua_newtable(L);
    lua_pushliteral(L, "size");
    lua_pushinteger(L, Info.bmiHeader.biSize);
    lua_settable(L, -3);
    lua_pushliteral(L, "width");
    lua_pushinteger(L, Info.bmiHeader.biWidth);
    lua_settable(L, -3);
    lua_pushliteral(L, "height");
    lua_pushinteger(L, Info.bmiHeader.biHeight);
    lua_settable(L, -3);
    lua_pushliteral(L, "planes");
    lua_pushinteger(L, Info.bmiHeader.biPlanes);
    lua_settable(L, -3);
    lua_pushliteral(L, "bitcount");
    lua_pushinteger(L, Info.bmiHeader.biBitCount);
    lua_settable(L, -3);
    lua_pushliteral(L, "compression");
    lua_pushinteger(L, Info.bmiHeader.biCompression);
    lua_settable(L, -3);
    lua_pushliteral(L, "sizeimage");
    lua_pushinteger(L, Info.bmiHeader.biSizeImage);
    lua_settable(L, -3);
    lua_pushliteral(L, "xpelspermeter");
    lua_pushinteger(L, Info.bmiHeader.biXPelsPerMeter);
    lua_settable(L, -3);
    lua_pushliteral(L, "ypelspermeter");
    lua_pushinteger(L, Info.bmiHeader.biYPelsPerMeter);
    lua_settable(L, -3);
    lua_pushliteral(L, "clrused");
    lua_pushinteger(L, Info.bmiHeader.biClrUsed);
    lua_settable(L, -3);
    lua_pushliteral(L, "clrimportant");
    lua_pushinteger(L, Info.bmiHeader.biClrImportant);
    lua_settable(L, -3);
    return 1;
}

/* GetHPal() -> PALETTE */
static int ilut_gethpal(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutGetHPal());
    return 1;
}

/* GetPaddedData() -> Data */
static int ilut_getpaddeddata(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutGetPaddedData());
    return 1;
}

/* GetWinClipboard() -> boolean */
static int ilut_getwinclipboard(lua_State* L) 
{
    return il_result_bool(L, ilutGetWinClipboard());
}

/* LoadResource(hInst, ID, ResourceType, Type) -> boolean */
static int ilut_loadresource(lua_State* L) 
{
    return il_result_bool(L, ilutLoadResource((HINSTANCE)lua_topointer(L, 1), luaL_checkinteger(L, 2),
                                              (ILstring)luaL_checkstring(L, 3), get_ilut_enum(L, 4)));
}

/* SetHBitmap(Bitmap) -> boolean */
static int ilut_sethbitmap(lua_State* L) 
{
    return il_result_bool(L, ilutSetHBitmap((HBITMAP)lua_topointer(L, 1)));
}

/* SetHPal(Bitmap) -> boolean */
static int ilut_sethpal(lua_State* L) 
{
    return il_result_bool(L, ilutSetHPal((HPALETTE)lua_topointer(L, 1)));
}

/* SetWinClipboard() -> boolean */
static int ilut_setwinclipboard(lua_State* L) 
{
    return il_result_bool(L, ilutSetWinClipboard());
}

/* SetWinClipboard(FileName, hDC) -> HBITMAP */
static int ilut_winloadimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutWinLoadImage((ILstring)luaL_checkstring(L, 1), (HDC)lua_topointer(L, 2)));
    return 1;
}

/* WinLoadUrl(Url) -> boolean */
/*
static int ilut_winloadurl(lua_State* L) 
{
    return il_result_bool(L, ilutWinLoadUrl((ILstring)luaL_checkstring(L, 1)));
}
*/

/* WinPrint(XPos, YPos, Width, Height, hDC) -> boolean */
static int ilut_winprint(lua_State* L) 
{
    return il_result_bool(L, ilutWinPrint(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
                                          luaL_checkinteger(L, 4), (HDC)lua_topointer(L, 5)));
}

/* WinSaveImage(FileName, Bitmap) -> boolean */
static int ilut_winsaveimage(lua_State* L) 
{
    return il_result_bool(L, ilutWinSaveImage((ILstring)luaL_checkstring(L, 1), (HBITMAP)lua_topointer(L, 2)));
}

#endif

#ifdef ILUT_USE_DIRECTX8
static int ilut_d3d8texture(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8volumetexture(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8texfromfile(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8voltexfromfile(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8texfromfileinmemory(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8voltexfromfileinmemory(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8texfromfilehandle(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8voltexfromfilehandle(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8texfromresource(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8voltexfromresource(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d8loadsurface(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

#endif

#ifdef ILUT_USE_DIRECTX9

static int ilut_d3d9texture(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9volumetexture(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9cubetexture(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9cubetexfromfile(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9cubetexfromfileinmemory(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9cubetexfromfilehandle(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9cubetexfromresource(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9texfromfile(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9voltexfromfile(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9texfromfileinmemory(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9voltexfromfileinmemory(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9texfromfilehandle(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9voltexfromfilehandle(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9texfromresource(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9voltexfromresource(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d9loadsurface(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

#endif

#ifdef ILUT_USE_DIRECTX10

static int ilut_d3d10texture(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d10texfromfile(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d10texfromfileinmemory(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d10texfromresource(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

static int ilut_d3d10texfromfilehandle(lua_State* L) 
{
    luaL_error(L, "not supported");
    return 0;
}

#endif

#ifdef ILUT_USE_X11

static int ilut_xcreateimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXCreateImage((Display*)lua_topointer(L, 1)));
    return 1;
}

static int ilut_xcreatepixmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXCreatePixmap((Display*)lua_topointer(L, 1), (Drawable)lua_topointer(L, 2)));
    return 1;
}

static int ilut_xloadimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXLoadImage((Display*)lua_topointer(L, 1), luaL_checkstring(L, 2)));
    return 1;
}

static int ilut_xloadpixmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXLoadPixmap((Display*)lua_topointer(L, 1), (Drawable)lua_topointer(L, 2),
                                             luaL_checkstring(L, 3)));
    return 1;
}

#ifdef ILUT_USE_XSHM

static int ilut_xshmcreateimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXShmCreateImage((Display*)lua_topointer(L, 1), (XShmSegmentInfo*)lua_topointer(L, 2)));
    return 1;
}

static int ilut_xshmdestroyimage(lua_State* L) 
{
    ilutXShmDestroyImage((Display*)lua_topointer(L, 1), (XImage*)lua_topointer(L, 2),
                         (XShmSegmentInfo*)lua_topointer(L, 3));
    return 0;
}

static int ilut_xshmcreatepixmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXShmCreatePixmap((Display*)lua_topointer(L, 1), (Drawable)lua_topointer(L, 2),
                                                  (XShmSegmentInfo*)lua_topointer(L, 3)));
    return 1;
}

static int ilut_xshmfreepixmap(lua_State* L) 
{
    ilutXShmFreePixmap((Display*)lua_topointer(L, 1), (Pixmap)lua_topointer(L, 2),
                       (XShmSegmentInfo*)lua_topointer(L, 3));
    return 0;
}

static int ilut_xshmloadimage(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXShmLoadImage((Display*)lua_topointer(L, 1), luaL_checkstring(L, 2),
                                               (XShmSegmentInfo*)lua_topointer(L, 3)));
    return 1;
}

static int ilut_xshmloadpixmap(lua_State* L) 
{
    lua_pushlightuserdata(L, ilutXShmLoadImage((Display*)lua_topointer(L, 1), (Drawable)lua_topointer(L, 2),
                                               luaL_checkstring(L, 3), (XShmSegmentInfo*)lua_topointer(L, 4)));
    return 1;
}

#endif
#endif

static const luaL_reg ilutlib[] = {
    {"Disable", ilut_disable},
    {"Enable", ilut_enable},
    {"GetBoolean", ilut_getboolean},
    /*{"GetBooleanv", ilut_getbooleanv},*/
    {"GetInteger", ilut_getinteger},
    /*{"GetIntegerv", ilut_getintegerv},*/
    {"GetString", ilut_getstring},
    {"IsDisabled", ilut_isdisabled},
    {"IsEnabled", ilut_isenabled},
    {"PopAttrib", ilut_popattrib},
    {"PushAttrib", ilut_pushattrib},
    {"SetInteger", ilut_setinteger},
    {"Renderer", ilut_renderer},
#ifdef ILUT_USE_OPENGL
    {"GLBindTexImage", ilut_glbindteximage},
    {"GLBindMipmaps", ilut_glbindmipmaps},
    {"GLBuildMipmaps", ilut_glbuildmipmaps},
    {"GLLoadImage", ilut_glloadimage},
    {"GLScreen", ilut_glscreen},
    {"GLScreenie", ilut_glscreenie},
    {"GLSaveImage", ilut_glsaveimage},
    {"GLSubTex2D", ilut_glsubtex2d},
    {"GLSubTex3D", ilut_glsubtex3d},
    {"GLSetTex2D", ilut_glsettex2d},
    {"GLSetTex3D", ilut_glsettex3d},
    {"GLTexImage", ilut_glteximage},
    /* 
	   {"GLSubTex", ilut_glsubtex},
       {"GLSetTex", ilut_glsettex},
       {"GLSubTex", ilut_glsubtex}, */
#endif

#ifdef ILUT_USE_ALLEGRO
    {"AllegLoadImage", ilut_allegloadimage},
    {"ConvertToAlleg", ilut_converttoalleg},
#endif

#ifdef ILUT_USE_SDL
    {"ConvertToSDLSurface", ilut_converttosdlsurface},
    {"SDLSurfaceLoadImage", ilut_sdlsurfaceloadimage},
    {"SDLSurfaceFromBitmap", ilut_sdlsurfacefrombitmap},
#endif

#ifdef ILUT_USE_BEOS
    {"ConvertToBBitmap", ilut_converttobbitmap},
#endif

#ifdef ILUT_USE_WIN32
    {"ConvertToHBitmap", ilut_converttohbitmap},
    {"ConvertSliceToHBitmap", ilut_convertslicetohbitmap},
    {"FreePaddedData", ilut_freepaddeddata},
    {"GetBmpInfo", ilut_getbmpinfo},
    {"GetHPal", ilut_gethpal},
    {"GetPaddedData", ilut_getpaddeddata},
    {"GetWinClipboard", ilut_getwinclipboard},
    {"LoadResource", ilut_loadresource},
    {"SetHBitmap", ilut_sethbitmap},
    {"SetHPal", ilut_sethpal},
    {"SetWinClipboard", ilut_setwinclipboard},
    {"WinLoadImage", ilut_winloadimage},
    /*{"WinLoadUrl", ilut_winloadurl},*/
    {"WinPrint", ilut_winprint},
    {"WinSaveImage", ilut_winsaveimage},
#endif

#ifdef ILUT_USE_DIRECTX8
    {"D3D8Texture", ilut_d3d8texture},
    {"D3D8VolumeTexture", ilut_d3d8volumetexture},
    {"D3D8TexFromFile", ilut_d3d8texfromfile},
    {"D3D8VolTexFromFile", ilut_d3d8voltexfromfile},
    {"D3D8TexFromFileInMemory", ilut_d3d8texfromfileinmemory},
    {"D3D8VolTexFromFileInMemory", ilut_d3d8voltexfromfileinmemory},
    {"D3D8TexFromFileHandle", ilut_d3d8texfromfilehandle},
    {"D3D8VolTexFromFileHandle", ilut_d3d8voltexfromfilehandle},
    {"D3D8TexFromResource", ilut_d3d8texfromresource},
    {"D3D8VolTexFromResource", ilut_d3d8voltexfromresource},
    {"D3D8LoadSurface", ilut_d3d8loadsurface},
#endif

#ifdef ILUT_USE_DIRECTX9
    {"D3D9Texture", ilut_d3d9texture},
    {"D3D9VolumeTexture", ilut_d3d9volumetexture},
    {"D3D9CubeTexture", ilut_d3d9cubetexture},
    {"D3D9CubeTexFromFile", ilut_d3d9cubetexfromfile},
    {"D3D9CubeTexFromFileInMemory", ilut_d3d9cubetexfromfileinmemory},
    {"D3D9CubeTexFromFileHandle", ilut_d3d9cubetexfromfilehandle},
    {"D3D9CubeTexFromResource", ilut_d3d9cubetexfromresource},
    {"D3D9TexFromFile", ilut_d3d9texfromfile},
    {"D3D9VolTexFromFile", ilut_d3d9voltexfromfile},
    {"D3D9TexFromFileInMemory", ilut_d3d9texfromfileinmemory},
    {"D3D9VolTexFromFileInMemory", ilut_d3d9voltexfromfileinmemory},
    {"D3D9TexFromFileHandle", ilut_d3d9texfromfilehandle},
    {"D3D9VolTexFromFileHandle", ilut_d3d9voltexfromfilehandle},
    {"D3D9TexFromResource", ilut_d3d9texfromresource},
    {"D3D9VolTexFromResource", ilut_d3d9voltexfromresource},
    {"D3D9LoadSurface", ilut_d3d9loadsurface},
#endif

#ifdef ILUT_USE_DIRECTX10
    {"D3D10Texture", ilut_d3d10texture},
    {"D3D10TexFromFile", ilut_d3d10texfromfile},
    {"D3D10TexFromFileInMemory", ilut_d3d10texfromfileinmemory},
    {"D3D10TexFromResource", ilut_d3d10texfromresource},
    {"D3D10TexFromFileHandle", ilut_d3d10texfromfilehandle},
#endif

#ifdef ILUT_USE_X11
    {"XCreateImage", ilut_xcreateimage},
    {"XCreatePixmap", ilut_xcreatepixmap},
    {"XLoadImage", ilut_xloadimage},
    {"XLoadPixmap", ilut_xloadpixmap},
#ifdef ILUT_USE_XSHM
    {"XShmCreateImage", ilut_xshmcreateimage},
    {"XShmDestroyImage", ilut_xshmdestroyimage},
    {"XShmCreatePixmap", ilut_xshmcreatepixmap},
    {"XShmFreePixmap", ilut_xshmfreepixmap},
    {"XShmLoadImage", ilut_xshmloadimage},
    {"XShmLoadPixmap", ilut_xshmloadpixmap},
#endif
#endif
    {NULL, NULL}
};

extern int luaopen_luailut(lua_State* L)
{
    luaL_openlib(L, "ilut", ilutlib, 0);
    ilutInit();
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* WITH_ILUT */
