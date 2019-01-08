/* luailu.c
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
#include <IL/ilu.h>

#ifdef WITH_ILU

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_luailu(lua_State* L);
extern const char *get_str_il_enum(ILenum num);

#define ENUM_ERROR (ILenum)-2

typedef struct ilu_str_value {
  const char *str;
  ILenum value;
} ilu_str_value;

static const ilu_str_value ilu_str[] = {
    {"FILTER", 0x2600},
    {"NEAREST", 0x2601},
    {"LINEAR", 0x2602},
    {"BILINEAR", 0x2603},
    {"SCALE_BOX", 0x2604},
    {"SCALE_TRIANGLE", 0x2605},
    {"SCALE_BELL", 0x2606},
    {"SCALE_BSPLINE", 0x2607},
    {"SCALE_LANCZOS3", 0x2608},
    {"SCALE_MITCHELL", 0x2609},
    {"INVALID_ENUM", 0x0501},
    {"OUT_OF_MEMORY", 0x0502},
    {"INTERNAL_ERROR", 0x0504},
    {"INVALID_VALUE", 0x0505},
    {"ILLEGAL_OPERATION", 0x0506},
    {"INVALID_PARAM", 0x0509},
    {"PLACEMENT", 0x0700},
    {"LOWER_LEFT", 0x0701},
    {"LOWER_RIGHT", 0x0702},
    {"UPPER_LEFT", 0x0703},
    {"UPPER_RIGHT", 0x0704},
    {"CENTER", 0x0705},
    {"CONVOLUTION_MATRIX", 0x0710},
    {"ENGLISH", 0x0800},
    {"ARABIC", 0x0801},
    {"DUTCH", 0x0802},
    {"JAPANESE", 0x0803},
    {"SPANISH", 0x0804},
    {"GERMAN", 0x0805},
    {"FRENCH", 0x0806},
};

/* convert string to IL enum */
static ILenum get_enum(const char *str, int n)
{
   int i = 0;

   while(ilu_str[i].str != 0)
   {
      if(strncmp(str, ilu_str[i].str, n) == 0 && ilu_str[i].str[n] == 0)
         return ilu_str[i].value;

      i++;
   }
   return ENUM_ERROR;
}

/* fetch string from lua stack and convert to IL enum */
static ILenum get_ilu_enum(lua_State* L, int index)
{
    size_t len;
    const char *str = luaL_checklstring(L, 1, &len);
    return get_enum(str, len);
}

/* Lua bindings to ILU */

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

/* Alienify() -> boolean */
static int ilu_alienify(lua_State* L)
{
    return il_result_bool(L, iluAlienify());
}


/* BlurAvg(Iter) -> boolean */
static int ilu_bluravg(lua_State* L)
{
    return il_result_bool(L, iluBlurAvg(luaL_checkinteger(L, 1)));
}

/* BlurGaussian(Iter) -> boolean */
static int ilu_blurgaussian(lua_State* L)
{
    return il_result_bool(L, iluBlurGaussian(luaL_checkinteger(L, 1)));
}

/* BuildMipmaps() -> boolean */
static int ilu_buildmipmaps(lua_State* L)
{
    return il_result_bool(L, iluBuildMipmaps());
}

/* ColoursUsed() -> number */
static int ilu_coloursused(lua_State* L)
{
    return il_result_int(L, iluColoursUsed());
}

/* CompareImage(Comp) -> boolean */
static int ilu_compareimage(lua_State* L)
{
    return il_result_bool(L, iluCompareImage(luaL_checkinteger(L, 1)));
}

/* Contrast(Contrast) -> boolean */
static int ilu_contrast(lua_State* L)
{
    return il_result_bool(L, iluContrast((ILfloat)luaL_checknumber(L, 1)));
}

/* Crop(XOff, YOff, ZOff, Width, Height, Depth) -> boolean */
static int ilu_crop(lua_State* L)
{
    return il_result_bool(L, iluCrop(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                     luaL_checkinteger(L, 3), luaL_checkinteger(L, 4),
                                     luaL_checkinteger(L, 5), luaL_checkinteger(L, 6)));
}

/* EdgeDetectE() -> boolean */
static int ilu_edgedetecte(lua_State* L)
{
    return il_result_bool(L, iluEdgeDetectE());
}

/* EdgeDetectP() -> boolean */
static int ilu_edgedetectp(lua_State* L)
{
    return il_result_bool(L, iluEdgeDetectP());
}

/* EdgeDetectS() -> boolean */
static int ilu_edgedetects(lua_State* L)
{
    return il_result_bool(L, iluEdgeDetectS());
}

/* Emboss() -> boolean */
static int ilu_emboss(lua_State* L)
{
    return il_result_bool(L, iluEmboss());
}

/* EnlargeCanvas(Width, Height, Depth) -> boolean */
static int ilu_enlargecanvas(lua_State* L)
{
    return il_result_bool(L, iluEnlargeCanvas(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                              luaL_checkinteger(L, 3)));
}

/* EnlargeImage(XDim, YDim, ZDim) -> boolean */
static int ilu_enlargeimage(lua_State* L)
{
    return il_result_bool(L, iluEnlargeImage((ILfloat)luaL_checknumber(L, 1), (ILfloat)luaL_checknumber(L, 2),
                                             (ILfloat)luaL_checknumber(L, 3)));
}

/* Equalize() -> boolean */
static int ilu_equalize(lua_State* L)
{
    return il_result_bool(L, iluEqualize());
}

/* ErrorString(enum) -> string */
static int ilu_errorstring(lua_State* L)
{
    lua_pushstring(L, iluErrorString(get_ilu_enum(L, 1)));
    return 1;
}

/* Convolution(Matrix, scale, bias) -> boolean
Matrix:
    {(x-1 y-1), (x y-1), (x+1 y-1),
     (x-1 y),   (x y),   (x+1 y),
     (x-1 y+1), (x y+1), (x+1 y+1)}
*/
static int ilu_convolution(lua_State* L)
{
    ILint matrix[9];
    int i;
    for (i=0; i<9; i++)
    {
        lua_rawgeti(L, 1, i+1);
        matrix[i] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }

    return il_result_bool(L, iluConvolution(matrix, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)));
}

/* FlipImage() -> boolean */
static int ilu_flipimage(lua_State* L)
{
    return il_result_bool(L, iluFlipImage());
}

/* GammaCorrect(Gamma) -> boolean */
static int ilu_gammacorrect(lua_State* L)
{
    return il_result_bool(L, iluGammaCorrect((ILfloat)luaL_checknumber(L, 1)));
}

/* GetImageInfo() -> ILinfo */
static int ilu_getimageinfo(lua_State* L)
{
    ILinfo Info;
	Info.Id = -666;
    iluGetImageInfo(&Info);
	if (Info.Id == -666)
	{
		return il_result_bool(L, 0);
	}
    lua_newtable(L);
    lua_pushliteral(L, "id");
    lua_pushinteger(L, Info.Id);
    lua_settable(L, -3);
    lua_pushliteral(L, "data");
    lua_pushlstring(L, (const char*)Info.Data, Info.SizeOfData);
    lua_settable(L, -3);
    lua_pushliteral(L, "width");
    lua_pushinteger(L, Info.Width);
    lua_settable(L, -3);
    lua_pushliteral(L, "height");
    lua_pushinteger(L, Info.Height);
    lua_settable(L, -3);
    lua_pushliteral(L, "depth");
    lua_pushinteger(L, Info.Depth);
    lua_settable(L, -3);
    lua_pushliteral(L, "bpp");
    lua_pushinteger(L, Info.Bpp);
    lua_settable(L, -3);
    lua_pushliteral(L, "sizeofdata");
    lua_pushinteger(L, Info.SizeOfData);
    lua_settable(L, -3);
    lua_pushliteral(L, "format");
    lua_pushstring(L, get_str_il_enum(Info.Format));
    lua_settable(L, -3);
    lua_pushliteral(L, "type");
    lua_pushstring(L, get_str_il_enum(Info.Type));
    lua_settable(L, -3);
    lua_pushliteral(L, "origin");
    lua_pushstring(L, get_str_il_enum(Info.Origin));
    lua_settable(L, -3);
    lua_pushliteral(L, "palette");
    lua_pushlstring(L, (const char*)Info.Palette, Info.PalSize);
    lua_settable(L, -3);
    lua_pushliteral(L, "paltype");
    lua_pushstring(L, get_str_il_enum(Info.PalType));
    lua_settable(L, -3);
    lua_pushliteral(L, "palsize");
    lua_pushinteger(L, Info.PalSize);
    lua_settable(L, -3);
    lua_pushliteral(L, "cubeflags");
    lua_pushstring(L, get_str_il_enum(Info.CubeFlags));
    lua_settable(L, -3);
    lua_pushliteral(L, "numnext");
    lua_pushinteger(L, Info.NumNext);
    lua_settable(L, -3);
    lua_pushliteral(L, "nummips");
    lua_pushinteger(L, Info.NumMips);
    lua_settable(L, -3);
    lua_pushliteral(L, "numlayers");
    lua_pushinteger(L, Info.NumLayers);
    lua_settable(L, -3);
    return 1;
}

/* GetInteger(enum) -> number */
static int ilu_getinteger(lua_State* L)
{
    lua_pushinteger(L, iluGetInteger(get_ilu_enum(L, 1)));
    return 1;
}

/* GetString(enum) -> string */
static int ilu_getstring(lua_State* L)
{
    lua_pushstring(L, iluGetString(get_ilu_enum(L, 1)));
    return 1;
}

/* ImageParameter(enum, enum) -> none */
static int ilu_imageparameter(lua_State* L)
{
    iluImageParameter(get_ilu_enum(L, 1), get_ilu_enum(L, 2));
    return 0;
}

/* InvertAlpha() -> boolean */
static int ilu_invertalpha(lua_State* L)
{
    return il_result_bool(L, iluInvertAlpha());
}

/* LoadImage(FileName) -> number */
static int ilu_loadimage(lua_State* L)
{
    return il_result_int(L, iluLoadImage(luaL_checkstring(L, 1)));
}

/* Mirror() -> boolean */
static int ilu_mirror(lua_State* L)
{
    return il_result_bool(L, iluMirror());
}

/* Negative() -> boolean */
static int ilu_negative(lua_State* L)
{
    return il_result_bool(L, iluNegative());
}

/* Noisify(Tolerance) -> boolean */
static int ilu_noisify(lua_State* L)
{
	return il_result_bool(L, iluNoisify((ILclampf)luaL_checknumber(L, 1)));
}

/* Pixelize(PixSize) -> boolean */
static int ilu_pixelize(lua_State* L)
{
    return il_result_bool(L, iluPixelize(luaL_checkinteger(L, 1)));
}

/* Region({x1, y1}, {x2, y2}, ...) -> none */
static int ilu_region(lua_State* L)
{
    int i, n = lua_gettop(L);
    ILpointf* points = (ILpointf*)malloc(n*sizeof(ILpointf));
    if (!points)
    {
        lua_pushnil(L);
		lua_pushstring(L, get_str_il_enum(IL_OUT_OF_MEMORY));
        return 2;
    }
    for (i=0; i<n; i++)
    {
        lua_rawgeti(L, i+1, 1);
        points[i].x = (ILfloat)luaL_checknumber(L, -1);
        lua_rawgeti(L, i+1, 2);
        points[i].y = (ILfloat)luaL_checknumber(L, -1);
        lua_pop(L, 2);
    }
    iluRegionfv(points, n);
    free(points);
    return 0;
}

/* ReplaceColour(Red, Green, Blue, Tolerance) -> boolean */
static int ilu_replacecolor(lua_State* L)
{
    return il_result_bool(L, iluReplaceColor(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                             luaL_checkinteger(L, 3), (ILfloat)luaL_checknumber(L, 4)));
}

/* Rotate(Angle) -> boolean */
static int ilu_rotate(lua_State* L)
{
    return il_result_bool(L, iluRotate((ILfloat)luaL_checknumber(L, 1)));
}

/* Rotate3D(Angle) -> boolean */
static int ilu_rotate3d(lua_State* L)
{
    return il_result_bool(L, iluRotate3D((ILfloat)luaL_checknumber(L, 1), (ILfloat)luaL_checknumber(L, 2),
                                         (ILfloat)luaL_checknumber(L, 3), (ILfloat)luaL_checknumber(L, 4)));
}

/* Saturate1(Saturation) -> boolean */
static int ilu_saturate1(lua_State* L)
{
    return il_result_bool(L, iluSaturate1f((ILfloat)luaL_checknumber(L, 1)));
}

/* Saturate4(R, G, B, Saturation) -> boolean */
static int ilu_saturate4(lua_State* L)
{
    return il_result_bool(L, iluSaturate4f((ILfloat)luaL_checknumber(L, 1), (ILfloat)luaL_checknumber(L, 2),
                                           (ILfloat)luaL_checknumber(L, 3), (ILfloat)luaL_checknumber(L, 4)));
}

/* Scale(Width, Height, Depth) -> boolean */
static int ilu_scale(lua_State* L)
{
    return il_result_bool(L, iluScale(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                      luaL_checkinteger(L, 3)));
}

/* ScaleAlpha(scale) -> boolean */
static int ilu_scalealpha(lua_State* L)
{
    return il_result_bool(L, iluScaleAlpha((ILfloat)luaL_checknumber(L, 1)));
}

/* ScaleColours(R, G, B) -> boolean */
static int ilu_scalecolours(lua_State* L)
{
    return il_result_bool(L, iluScaleColours((ILfloat)luaL_checknumber(L, 1), (ILfloat)luaL_checknumber(L, 2),
                                             (ILfloat)luaL_checknumber(L, 3)));
}

/* SetLanguage(Language) -> boolean */
static int ilu_setlanguage(lua_State* L)
{
    return il_result_bool(L, iluSetLanguage(get_ilu_enum(L, 1)));
}

/* Sharpen(Factor, Iter) -> boolean */
static int ilu_sharpen(lua_State* L)
{
    return il_result_bool(L, iluSharpen((ILfloat)luaL_checknumber(L, 1), luaL_checkinteger(L, 2)));
}

/* SwapColours() -> boolean */
static int ilu_swapcolours(lua_State* L)
{
    return il_result_bool(L, iluSwapColours());
}

/* Wave() -> boolean */
static int ilu_wave(lua_State* L)
{
    return il_result_bool(L, iluWave((ILfloat)luaL_checknumber(L, 1)));
}

static const luaL_reg ilulib[] = {
    {"Alienify", ilu_alienify},
    {"BlurAvg", ilu_bluravg},
    {"BlurGaussian", ilu_blurgaussian},
    {"BuildMipmaps", ilu_buildmipmaps},
    {"ColoursUsed", ilu_coloursused},
    {"CompareImage", ilu_compareimage},
    {"Contrast", ilu_contrast},
    {"Crop", ilu_crop},
    /*{"DeleteImage", ilu_deleteimage}, depricated */
    {"EdgeDetectE", ilu_edgedetecte},
    {"EdgeDetectP", ilu_edgedetectp},
    {"EdgeDetectS", ilu_edgedetects},
    {"Emboss", ilu_emboss},
    {"EnlargeCanvas", ilu_enlargecanvas},
    {"EnlargeImage", ilu_enlargeimage},
    {"Equalize", ilu_equalize},
    {"ErrorString", ilu_errorstring},
    {"Convolution", ilu_convolution},
    {"FlipImage", ilu_flipimage},
    {"GammaCorrect", ilu_gammacorrect},
    /*{"GenImage", ilu_genimage}, depricated */
    {"GetImageInfo", ilu_getimageinfo},
    {"GetInteger", ilu_getinteger},
    /*{"GetIntegerv", ilu_getintegerv},*/
    {"GetString", ilu_getstring},
    {"ImageParameter", ilu_imageparameter},
    {"InvertAlpha", ilu_invertalpha},
    {"LoadImage", ilu_loadimage},
    {"Mirror", ilu_mirror},
    {"Negative", ilu_negative},
    {"Noisify", ilu_noisify},
    {"Pixelize", ilu_pixelize},
    {"Region", ilu_region},
    {"ReplaceColor", ilu_replacecolor},
    {"Rotate", ilu_rotate},
    {"Rotate3D", ilu_rotate3d},
    {"Saturate1", ilu_saturate1},
    {"Saturate4", ilu_saturate4},
    {"Scale", ilu_scale},
    {"ScaleAlpha", ilu_scalealpha},
    {"ScaleColours", ilu_scalecolours},
    {"SetLanguage", ilu_setlanguage},
    {"Sharpen", ilu_sharpen},
    {"SwapColours", ilu_swapcolours},
    {"Wave", ilu_wave},
    {NULL, NULL}
};

extern int luaopen_luailu(lua_State* L)
{
    luaL_openlib(L, "ilu", ilulib, 0);
    iluInit();
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* WITH_ILU */
