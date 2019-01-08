/* luail.c
 *
 * author      : Alexander Marinov (alekmarinov@gmail.com)
 * project     : luadevil (http://luadevil.luaforge.net)
 * description : Binds DevIL to Lua
 * copyright   : The same as Lua license (http://www.lua.org/license.html)
 *
 *******************************************************************************************/

#if defined _WIN32 || defined __CYGWIN__
#  ifndef LUAIL_API
#    ifdef DLL_EXPORTS
#      define LUAIL_DLL
#    endif
#    ifdef LUAIL_DLL
#      ifdef __GNUC__
#        define LUAIL_API __attribute__((dllexport))
#      else
         // Note: actually gcc seems to also supports this syntax.
#        define LUAIL_API __declspec(dllexport)
#      endif
#    else
#      define LUAIL_API extern
#    endif
#  endif
#endif

#include <string.h>
#include <malloc.h>
#include <lauxlib.h>

#include <IL/il.h>

#ifdef __cplusplus
extern "C" {
#endif

/* devil_internal_exports.h */
ILAPI ILubyte ILAPIENTRY ilGetBppFormat(ILenum Format);
ILAPI ILubyte ILAPIENTRY ilGetBpcType(ILenum Type);

LUAIL_API int luaopen_luail(lua_State* L);
const char *get_str_il_enum(ILenum num);

#ifdef WITH_ILU
extern int luaopen_luailu(lua_State* L);
#endif

#ifdef WITH_ILUT
extern int luaopen_luailut(lua_State* L);
#endif

#define ENUM_ERROR (ILenum)-2

typedef struct il_str_value {
  const char *str;
  ILenum value;
} il_str_value;

static const il_str_value il_str[] = {
    {"COLOUR_INDEX", 0x1900},
    {"COLOR_INDEX", 0x1900},
    {"ALPHA", 0x1906},
    {"RGB", 0x1907},
    {"RGBA", 0x1908},
    {"BGR", 0x80E0},
    {"BGRA", 0x80E1},
    {"LUMINANCE", 0x1909},
    {"LUMINANCE_ALPHA", 0x190A},
    {"BYTE", 0x1400},
    {"UNSIGNED_BYTE", 0x1401},
    {"SHORT", 0x1402},
    {"UNSIGNED_SHORT", 0x1403},
    {"INT", 0x1404},
    {"UNSIGNED_INT", 0x1405},
    {"FLOAT", 0x1406},
    {"DOUBLE", 0x140A},
    {"HALF", 0x140B},
    {"VENDOR", 0x1F00},
    {"LOAD_EXT", 0x1F01},
    {"SAVE_EXT", 0x1F02},
    {"ORIGIN_BIT", 0x00000001},
    {"FILE_BIT", 0x00000002},
    {"PAL_BIT", 0x00000004},
    {"FORMAT_BIT", 0x00000008},
    {"TYPE_BIT", 0x00000010},
    {"COMPRESS_BIT", 0x00000020},
    {"LOADFAIL_BIT", 0x00000040},
    {"FORMAT_SPECIFIC_BIT", 0x00000080},
    {"ALL_ATTRIB_BITS", 0x000FFFFF},
    {"PAL_NONE", 0x0400},
    {"PAL_RGB24", 0x0401},
    {"PAL_RGB32", 0x0402},
    {"PAL_RGBA32", 0x0403},
    {"PAL_BGR24", 0x0404},
    {"PAL_BGR32", 0x0405},
    {"PAL_BGRA32", 0x0406},
    {"TYPE_UNKNOWN", 0x0000},
    {"BMP", 0x0420},
    {"CUT", 0x0421},
    {"DOOM", 0x0422},
    {"DOOM_FLAT", 0x0423},
    {"ICO", 0x0424},
    {"JPG", 0x0425},
    {"JFIF", 0x0425},
    {"ILBM", 0x0426},
    {"PCD", 0x0427},
    {"PCX", 0x0428},
    {"PIC", 0x0429},
    {"PNG", 0x042A},
    {"PNM", 0x042B},
    {"SGI", 0x042C},
    {"TGA", 0x042D},
    {"TIF", 0x042E},
    {"CHEAD", 0x042F},
    {"RAW", 0x0430},
    {"MDL", 0x0431},
    {"WAL", 0x0432},
    {"LIF", 0x0434},
    {"MNG", 0x0435},
    {"JNG", 0x0435},
    {"GIF", 0x0436},
    {"DDS", 0x0437},
    {"DCX", 0x0438},
    {"PSD", 0x0439},
    {"EXIF", 0x043A},
    {"PSP", 0x043B},
    {"PIX", 0x043C},
    {"PXR", 0x043D},
    {"XPM", 0x043E},
    {"HDR", 0x043F},
    {"ICNS", 0x0440},
    {"JP2", 0x0441},
    {"EXR", 0x0442},
    {"WDP", 0x0443},
    {"VTF", 0x0444},
    {"WBMP", 0x0445},
    {"SUN", 0x0446},
    {"IFF", 0x0447},
    {"TPL", 0x0448},
    {"FITS", 0x0449},
    {"DICOM", 0x044A},
    {"IWI", 0x044B},
    {"BLP", 0x044C},
    {"FTX", 0x044D},
    {"ROT", 0x044E},
    {"TEXTURE", 0x044F},
    {"DPX", 0x0450},
    {"UTX", 0x0451},
    {"MP3", 0x0452},
    {"JASC_PAL", 0x0475},
    {"NO_ERROR", 0x0000}, /* Be careful! Conflicts with TYPE_UNKNOWN */
    {"INVALID_ENUM", 0x0501},
    {"OUT_OF_MEMORY", 0x0502},
    {"FORMAT_NOT_SUPPORTED", 0x0503},
    {"INTERNAL_ERROR", 0x0504},
    {"INVALID_VALUE", 0x0505},
    {"ILLEGAL_OPERATION", 0x0506},
    {"ILLEGAL_FILE_VALUE", 0x0507},
    {"INVALID_FILE_HEADER", 0x0508},
    {"INVALID_PARAM", 0x0509},
    {"COULD_NOT_OPEN_FILE", 0x050A},
    {"INVALID_EXTENSION", 0x050B},
    {"FILE_ALREADY_EXISTS", 0x050C},
    {"OUT_FORMAT_SAME", 0x050D},
    {"STACK_OVERFLOW", 0x050E},
    {"STACK_UNDERFLOW", 0x050F},
    {"INVALID_CONVERSION", 0x0510},
    {"BAD_DIMENSIONS", 0x0511},
    {"FILE_READ_ERROR", 0x0512},
    {"FILE_WRITE_ERROR", 0x0512},
    {"LIB_GIF_ERROR", 0x05E1},
    {"LIB_JPEG_ERROR", 0x05E2},
    {"LIB_PNG_ERROR", 0x05E3},
    {"LIB_TIFF_ERROR", 0x05E4},
    {"LIB_MNG_ERROR", 0x05E5},
    {"LIB_JP2_ERROR", 0x05E6},
    {"LIB_EXR_ERROR", 0x05E7},
    {"UNKNOWN_ERROR", 0x05FF},
    {"ORIGIN_SET", 0x0600},
    {"ORIGIN_LOWER_LEFT", 0x0601},
    {"ORIGIN_UPPER_LEFT", 0x0602},
    {"ORIGIN_MODE", 0x0603},
    {"FORMAT_SET", 0x0610},
    {"FORMAT_MODE", 0x0611},
    {"TYPE_SET", 0x0612},
    {"TYPE_MODE", 0x0613},
    {"FILE_OVERWRITE", 0x0620},
    {"FILE_MODE", 0x0621},
    {"CONV_PAL", 0x0630},
    {"DEFAULT_ON_FAIL", 0x0632},
    {"USE_KEY_COLOUR", 0x0635},
    {"USE_KEY_COLOR", 0x0635},
    {"BLIT_BLEND", 0x0636},
    {"SAVE_INTERLACED", 0x0639},
    {"INTERLACE_MODE", 0x063A},
    {"QUANTIZATION_MODE", 0x0640},
    {"WU_QUANT", 0x0641},
    {"NEU_QUANT", 0x0642},
    {"NEU_QUANT_SAMPLE", 0x0643},
    {"MAX_QUANT_INDEXS", 0x0644},
    {"MAX_QUANT_INDICES", 0x0644},
    {"FASTEST", 0x0660},
    {"LESS_MEM", 0x0661},
    {"DONT_CARE", 0x0662},
    {"MEM_SPEED_HINT", 0x0665},
    {"USE_COMPRESSION", 0x0666},
    {"NO_COMPRESSION", 0x0667},
    {"COMPRESSION_HINT", 0x0668},
    {"NVIDIA_COMPRESS", 0x0670},
    {"SQUISH_COMPRESS", 0x0671},
    {"SUB_NEXT", 0x0680},
    {"SUB_MIPMAP", 0x0681},
    {"SUB_LAYER", 0x0682},
    {"COMPRESS_MODE", 0x0700},
    {"COMPRESS_NONE", 0x0701},
    {"COMPRESS_RLE", 0x0702},
    {"COMPRESS_LZO", 0x0703},
    {"COMPRESS_ZLIB", 0x0704},
    {"TGA_CREATE_STAMP", 0x0710},
    {"JPG_QUALITY", 0x0711},
    {"PNG_INTERLACE", 0x0712},
    {"TGA_RLE", 0x0713},
    {"BMP_RLE", 0x0714},
    {"SGI_RLE", 0x0715},
    {"TGA_ID_STRING", 0x0717},
    {"TGA_AUTHNAME_STRING", 0x0718},
    {"TGA_AUTHCOMMENT_STRING", 0x0719},
    {"PNG_AUTHNAME_STRING", 0x071A},
    {"PNG_TITLE_STRING", 0x071B},
    {"PNG_DESCRIPTION_STRING", 0x071C},
    {"TIF_DESCRIPTION_STRING", 0x071D},
    {"TIF_HOSTCOMPUTER_STRING", 0x071E},
    {"TIF_DOCUMENTNAME_STRING", 0x071F},
    {"TIF_AUTHNAME_STRING", 0x0720},
    {"JPG_SAVE_FORMAT", 0x0721},
    {"CHEAD_HEADER_STRING", 0x0722},
    {"PCD_PICNUM", 0x0723},
    {"PNG_ALPHA_INDEX", 0x0724},
    {"JPG_PROGRESSIVE", 0x0725},
    {"VTF_COMP", 0x0726},
    {"DXTC_FORMAT", 0x0705},
    {"DXT1", 0x0706},
    {"DXT2", 0x0707},
    {"DXT3", 0x0708},
    {"DXT4", 0x0709},
    {"DXT5", 0x070A},
    {"DXT_NO_COMP", 0x070B},
    {"KEEP_DXTC_DATA", 0x070C},
    {"DXTC_DATA_FORMAT", 0x070D},
    {"3DC", 0x070E},
    {"RXGB", 0x070F},
    {"ATI1N", 0x0710},
    {"DXT1A", 0x0711},
    {"CUBEMAP_POSITIVEX", 0x00000400},
    {"CUBEMAP_NEGATIVEX", 0x00000800},
    {"CUBEMAP_POSITIVEY", 0x00001000},
    {"CUBEMAP_NEGATIVEY", 0x00002000},
    {"CUBEMAP_POSITIVEZ", 0x00004000},
    {"CUBEMAP_NEGATIVEZ", 0x00008000},
    {"SPHEREMAP", 0x00010000},
    {"VERSION_NUM", 0x0DE2},
    {"IMAGE_WIDTH", 0x0DE4},
    {"IMAGE_HEIGHT", 0x0DE5},
    {"IMAGE_DEPTH", 0x0DE6},
    {"IMAGE_SIZE_OF_DATA", 0x0DE7},
    {"IMAGE_BPP", 0x0DE8},
    {"IMAGE_BYTES_PER_PIXEL", 0x0DE8},
    {"IMAGE_BPP", 0x0DE8},
    {"IMAGE_BITS_PER_PIXEL", 0x0DE9},
    {"IMAGE_FORMAT", 0x0DEA},
    {"IMAGE_TYPE", 0x0DEB},
    {"PALETTE_TYPE", 0x0DEC},
    {"PALETTE_SIZE", 0x0DED},
    {"PALETTE_BPP", 0x0DEE},
    {"PALETTE_NUM_COLS", 0x0DEF},
    {"PALETTE_BASE_TYPE", 0x0DF0},
    {"NUM_FACES", 0x0DE1},
    {"NUM_IMAGES", 0x0DF1},
    {"NUM_MIPMAPS", 0x0DF2},
    {"NUM_LAYERS", 0x0DF3},
    {"ACTIVE_IMAGE", 0x0DF4},
    {"ACTIVE_MIPMAP", 0x0DF5},
    {"ACTIVE_LAYER", 0x0DF6},
    {"ACTIVE_FACE", 0x0E00},
    {"CUR_IMAGE", 0x0DF7},
    {"IMAGE_DURATION", 0x0DF8},
    {"IMAGE_PLANESIZE", 0x0DF9},
    {"IMAGE_BPC", 0x0DFA},
    {"IMAGE_OFFX", 0x0DFB},
    {"IMAGE_OFFY", 0x0DFC},
    {"IMAGE_CUBEFLAGS", 0x0DFD},
    {"IMAGE_ORIGIN", 0x0DFE},
    {"IMAGE_CHANNELS", 0x0DFF},
};

typedef struct
{
    int fCloseRProcRef, fEofProcRef, fGetcProcRef,
        fOpenRProcRef, fReadProcRef, fSeekRProcRef, fTellRProcRef;
    lua_State* L;
} read_callbacks_t;

typedef struct
{
    int fCloseWProcRef, fOpenWProcRef, fPutcProcRef,
        fSeekWProcRef, fTellWProcRef, fWriteProcRef;
    lua_State* L;
} write_callbacks_t;

typedef struct
{
    int LoadProcRef;
    lua_State* L;
} load_callback_t;

typedef struct
{
    int SaveProcRef;
    lua_State* L;
} save_callback_t;


static read_callbacks_t read_callbacks = {0};
static write_callbacks_t write_callbacks = {0};
static load_callback_t load_callback = {0};
static save_callback_t save_callback = {0};

/* convert string to IL enum */
static ILenum get_enum(const char *str, int n)
{
   int i = 0;

   while(il_str[i].str != 0)
   {
      if(strncmp(str, il_str[i].str, n) == 0 && il_str[i].str[n] == 0)
         return il_str[i].value;

      i++;
   }
   return ENUM_ERROR;
}

/* fetch string from lua stack and convert to IL enum */
static ILenum get_il_enum(lua_State* L, int index)
{
    size_t len;
    const char *str = luaL_checklstring(L, index, &len);
    return get_enum(str, len);
}

/* fetch string from lua stack and convert to IL bit mask */
static ILuint get_il_mask(lua_State *L, int index)
{
   unsigned int i;
   size_t slen;
   const char *str = luaL_checklstring(L, index, &slen);
   ILenum temp = 0, ret = 0;

   for(i = 0; i < slen; i++)
   {
      if(str[i] == ',')
      {
         temp = get_enum(str, i);
         if(temp != ENUM_ERROR)
            ret |= temp;

         str += i+1;
         i = 0;
      }
   }
   temp = get_enum(str, strlen(str));

   if(temp == ENUM_ERROR)
   {
      if(ret == 0)
         return ENUM_ERROR;
      return ret;
   }

   return ret | temp;
}

/* convert IL enum to string */
const char *get_str_il_enum(ILenum num)
{
   unsigned int i = 0;

   while(il_str[i].str != 0)
   {
      if(num == il_str[i].value)
         return il_str[i].str;

      i++;
   }
   return NULL;
}


/* file reading callbacks intermediating with lua funcs */
static void ILAPIENTRY luafCloseRProc(ILHANDLE stream)
{
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fCloseRProcRef);
    lua_pushlightuserdata(read_callbacks.L, stream);
    lua_call(read_callbacks.L, 1, 0);
}

static ILboolean ILAPIENTRY luafEofProc(ILHANDLE stream)
{
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fEofProcRef);
    lua_pushlightuserdata(read_callbacks.L, stream);
    lua_call(read_callbacks.L, 1, 1);
    return lua_toboolean(read_callbacks.L, -1);
}

static ILint ILAPIENTRY luafGetcProc(ILHANDLE stream)
{
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fGetcProcRef);
    lua_pushlightuserdata(read_callbacks.L, stream);
    lua_call(read_callbacks.L, 1, 1);
    return lua_tointeger(read_callbacks.L, -1);
}

static ILHANDLE ILAPIENTRY luafOpenRProc(ILconst_string filename)
{
    ILHANDLE stream;
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fOpenRProcRef);
    lua_pushstring(read_callbacks.L, filename);
    lua_call(read_callbacks.L, 1, 1);
    stream = (ILHANDLE)lua_topointer(read_callbacks.L, -1);
    return stream;
}

static ILint ILAPIENTRY luafReadProc(void* data, ILuint size, ILuint count, ILHANDLE stream)
{
    const char* readbytes;
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fReadProcRef);
    lua_pushlightuserdata(read_callbacks.L, stream);
    lua_pushinteger(read_callbacks.L, size * count);
    lua_call(read_callbacks.L, 2, 1);
    readbytes = lua_tostring(read_callbacks.L, -1);
    if (readbytes)
    {
        size_t len = lua_strlen(read_callbacks.L, -1);
        memcpy(data, readbytes, len);
        return len;
    }
    return 0;
}

static ILint ILAPIENTRY luafSeekRProc(ILHANDLE stream, ILint offset, ILint origin)
{
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fSeekRProcRef);
    lua_pushlightuserdata(read_callbacks.L, stream);
    lua_pushinteger(read_callbacks.L, offset);
    switch (origin)
    {
    case SEEK_CUR: lua_pushliteral(read_callbacks.L, "SEEK_CUR"); break;
    case SEEK_END: lua_pushliteral(read_callbacks.L, "SEEK_END"); break;
    default:       lua_pushliteral(read_callbacks.L, "SEEK_SET"); break;
    }
    lua_call(read_callbacks.L, 3, 1);
    return lua_tointeger(read_callbacks.L, -1);
}

static ILint ILAPIENTRY luafTellRProc(ILHANDLE stream)
{
    lua_rawgeti(read_callbacks.L, LUA_REGISTRYINDEX, read_callbacks.fTellRProcRef);
    lua_pushlightuserdata(read_callbacks.L, stream);
    lua_call(read_callbacks.L, 1, 1);
    return lua_tointeger(read_callbacks.L, -1);
}

/* file writing callbacks intermediating with lua funcs */
static void ILAPIENTRY luafCloseWProc(ILHANDLE stream)
{
    lua_rawgeti(write_callbacks.L, LUA_REGISTRYINDEX, write_callbacks.fCloseWProcRef);
    lua_pushlightuserdata(write_callbacks.L, stream);
    lua_call(write_callbacks.L, 1, 0);
}

static ILHANDLE ILAPIENTRY luafOpenWProc(ILconst_string filename)
{
    ILHANDLE stream;
    lua_rawgeti(write_callbacks.L, LUA_REGISTRYINDEX, write_callbacks.fOpenWProcRef);
    lua_pushstring(write_callbacks.L, filename);
    lua_call(write_callbacks.L, 1, 1);
    stream = (ILHANDLE)lua_topointer(write_callbacks.L, -1);
    return stream;
}

static ILint ILAPIENTRY luafPutcProc(ILubyte byte, ILHANDLE stream)
{
    lua_rawgeti(write_callbacks.L, LUA_REGISTRYINDEX, write_callbacks.fPutcProcRef);
    lua_pushlightuserdata(write_callbacks.L, stream);
    lua_pushinteger(write_callbacks.L, byte);
    lua_call(write_callbacks.L, 2, 1);
    return lua_tointeger(write_callbacks.L, -1);
}

static ILint ILAPIENTRY luafSeekWProc(ILHANDLE stream, ILint offset, ILint origin)
{
    lua_rawgeti(write_callbacks.L, LUA_REGISTRYINDEX, write_callbacks.fSeekWProcRef);
    lua_pushlightuserdata(write_callbacks.L, stream);
    lua_pushinteger(write_callbacks.L, offset);
    switch (origin)
    {
    case SEEK_CUR: lua_pushliteral(write_callbacks.L, "SEEK_CUR"); break;
    case SEEK_END: lua_pushliteral(write_callbacks.L, "SEEK_END"); break;
    default:       lua_pushliteral(write_callbacks.L, "SEEK_SET"); break;
    }
    lua_call(write_callbacks.L, 3, 1);
    return lua_tointeger(write_callbacks.L, -1);
}

static ILint ILAPIENTRY luafTellWProc(ILHANDLE stream)
{
    lua_rawgeti(write_callbacks.L, LUA_REGISTRYINDEX, write_callbacks.fTellWProcRef);
    lua_pushlightuserdata(write_callbacks.L, stream);
    lua_call(write_callbacks.L, 1, 1);
    return lua_tointeger(write_callbacks.L, -1);
}

static ILint ILAPIENTRY luafWriteProc(const void* data, ILuint size, ILuint count, ILHANDLE stream)
{
    lua_rawgeti(write_callbacks.L, LUA_REGISTRYINDEX, write_callbacks.fWriteProcRef);
    lua_pushlightuserdata(write_callbacks.L, stream);
    lua_pushlstring(write_callbacks.L, (const char*)data, size*count);
    lua_call(write_callbacks.L, 2, 1);
    return lua_tointeger(write_callbacks.L, -1);
}

/* save/load registered format procedures intermediating with lua funcs */
static ILenum ILAPIENTRY load_proc_cb(ILconst_string filename)
{
    lua_rawgeti(load_callback.L, LUA_REGISTRYINDEX, load_callback.LoadProcRef);
    lua_pushstring(load_callback.L, filename);
    lua_call(load_callback.L, 1, 1);
    return get_il_enum(load_callback.L, -1);
}

static ILenum ILAPIENTRY save_proc_cb(ILconst_string filename)
{
    lua_rawgeti(save_callback.L, LUA_REGISTRYINDEX, save_callback.SaveProcRef);
    lua_pushstring(save_callback.L, filename);
    lua_call(save_callback.L, 1, 1);
    return get_il_enum(save_callback.L, -1);
}

/* Lua bindings to IL */

/* GetError() -> string */
static int il_geterror(lua_State* L)
{
	int errid = ilGetError();
	if (errid)
	{
		const char* error = get_str_il_enum(errid);
		if (!error)
		{
			lua_pushnil(L);
			lua_pushliteral(L, "UNKNOWN_ERROR");
			return 2;
		}
		lua_pushstring(L, error);
	}
	else
	{
		lua_pushliteral(L, "NO_ERROR");
	}
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

/* ActiveFace(Number) -> boolean */
static int il_active_face(lua_State* L)
{
	return il_result_bool(L, ilActiveFace(luaL_checkinteger(L, 1)));
}

/* ActiveImage(Number) -> boolean */
static int il_activeimage(lua_State* L)
{
	return il_result_bool(L, ilActiveImage(luaL_checkinteger(L, 1)));
}

/* ActiveLayer(Number) -> boolean */
static int il_activelayer(lua_State* L)
{
	return il_result_bool(L, ilActiveLayer(luaL_checkinteger(L, 1)));
}

/* ActiveMipmap(Number) -> boolean */
static int il_activemipmap(lua_State* L)
{
	return il_result_bool(L, ilActiveMipmap(luaL_checkinteger(L, 1)));
}

/* ApplyPal(FileName) -> boolean */
static int il_applypal(lua_State* L)
{
	return il_result_bool(L, ilApplyPal(luaL_checkstring(L, 1)));
}

/* ApplyProfile(InProfile, OutProfile) -> boolean */
static int il_applyprofile(lua_State* L)
{
	return il_result_bool(L, ilApplyProfile((ILstring)luaL_checkstring(L, 1), (ILstring)luaL_checkstring(L, 2)));
}

/* BindImage(Image) -> boolean */
static int il_bindimage(lua_State* L)
{
    ilBindImage(luaL_checkinteger(L, 1));
	lua_pushboolean(L, 1);
    return 1;
}

/* Blit(Source, DestX, DestY, DestZ, SrcX, SrcY, SrcZ, Width, Height, Depth) -> boolean */
static int il_blit(lua_State* L)
{
    return il_result_bool(L, ilBlit(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                              luaL_checkinteger(L, 3), luaL_checkinteger(L, 4),
                              luaL_checkinteger(L, 5), luaL_checkinteger(L, 6),
                              luaL_checkinteger(L, 7), luaL_checkinteger(L, 8),
                              luaL_checkinteger(L, 9), luaL_checkinteger(L, 10)));
}

/* ClampNTSC() -> boolean */
static int il_clampntsc(lua_State* L)
{
    return il_result_bool(L, ilClampNTSC());
}

/* ilClearColor(Red, Green, Blue, Alpha) -> boolean */
static int il_clearcolor(lua_State* L)
{
    ilClearColor((ILclampf)luaL_checknumber(L, 1),
		(ILclampf)luaL_checknumber(L, 2),
		(ILclampf)luaL_checknumber(L, 3),
		(ILclampf)luaL_checknumber(L, 4));
	lua_pushboolean(L, 1);
    return 1;
}

/* ilClearImage() -> boolean */
static int il_clearimage(lua_State* L)
{
    return il_result_bool(L, ilClearImage());
}

/* CloneCurImage() -> number */
static int il_clonecurimage(lua_State* L)
{
    return il_result_int(L, ilCloneCurImage());
}

/* CompressDXT(Data pointer, Width, Height, Depth, DXTCFormat) -> string */
static int il_compressdxt(lua_State* L)
{
    ILuint size;
    ILubyte* data = ilCompressDXT((ILubyte *)lua_topointer(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
                                  luaL_checkinteger(L, 4), get_il_enum(L, 5), &size);
    if (data)
    {
        lua_pushlightuserdata(L, (char*)data);
		lua_pushinteger(L, size);
        return 2;
    }
	lua_pushnil(L);
	il_geterror(L);
    return 2;
}

/* CompressFunc(enum) -> boolean */
static int il_compressfunc(lua_State* L)
{
    return il_result_bool(L, ilCompressFunc(get_il_enum(L, 1)));
}

/* ConvertImage(enum, enum) -> boolean */
static int il_convertimage(lua_State* L)
{
    return il_result_bool(L, ilConvertImage(get_il_enum(L, 1), get_il_enum(L, 2)));
}

/* ConvertPal(enum) -> boolean */
static int il_convertpal(lua_State* L)
{
    return il_result_bool(L, ilConvertPal(get_il_enum(L, 1)));
}

/* CopyImage(Number) -> boolean */
static int il_copyimage(lua_State* L)
{
    return il_result_bool(L, ilCopyImage(luaL_checkinteger(L, 1)));
}

/* CopyPixels(XOff, YOff, ZOff, Width, Height, Depth, Format, Type) -> Data */
static int il_copypixels(lua_State* L)
{
	ILuint x = luaL_checkinteger(L, 1),
	       y = luaL_checkinteger(L, 2),
	       z = luaL_checkinteger(L, 3),
	       w = luaL_checkinteger(L, 4),
	       h = luaL_checkinteger(L, 5),
	       d = luaL_checkinteger(L, 6);
	ILenum format = get_il_enum(L, 7);
	ILenum type = get_il_enum(L, 8);
	ILuint bufsize = w * h * d * ilGetBppFormat(format) * ilGetBpcType(type);
	void* buffer = malloc(bufsize);
	if (!buffer)
	{
		lua_pushnil(L);
		lua_pushstring(L, get_str_il_enum(IL_OUT_OF_MEMORY));
		return 2;
	}
	if (bufsize != ilCopyPixels(x, y, z, w, h, d, format, type, buffer))
	{
		lua_pushnil(L);
		il_geterror(L);
		return 2;
	}
	lua_pushlightuserdata(L, buffer);
	lua_pushinteger(L, bufsize);
	return 2;
}

/* CreateSubImage(enum, Number) -> number */
static int il_createsubimage(lua_State* L)
{
    return il_result_int(L, ilCreateSubImage(get_il_enum(L, 1), luaL_checkinteger(L, 2)));
}

/* DefaultImage() -> boolean */
static int il_defaultimage(lua_State* L)
{
   return il_result_bool(L, ilDefaultImage());
}

/* DeleteImage(Number) -> none */
static int il_deleteimage(lua_State* L)
{
    ilDeleteImage(luaL_checkinteger(L, 1));
	lua_pushboolean(L, 1);
    return 1;
}

/* DeleteImages(Number, Number, ...) -> none */
static int il_deleteimages(lua_State* L)
{
    int i, n = lua_gettop(L);
    ILuint* Images = (ILuint*)malloc(n * sizeof(ILuint));
    if (!Images)
    {
        lua_pushnil(L);
		lua_pushstring(L, get_str_il_enum(IL_OUT_OF_MEMORY));
        return 2;
    }
    for (i=1; i<=n; i++)
    {
        Images[i-1] = luaL_checkinteger(L, i);
    }
    ilDeleteImages(n, Images);
    free(Images);
	lua_pushboolean(L, 1);
    return 1;
}

/* DetermineType(FileName) -> string */
static int il_determinetype(lua_State* L)
{
    const char* filetype = get_str_il_enum(ilDetermineType(luaL_checkstring(L, 1)));
    if (filetype)
        lua_pushstring(L, filetype);
    else
        lua_pushliteral(L, "TYPE_UNKNOWN");
    return 1;
}

/* DetermineType(stream) -> string */
static int il_determinetypef(lua_State* L)
{
    const char* filetype = get_str_il_enum(ilDetermineTypeF((ILHANDLE)lua_topointer(L, 1)));
    if (filetype)
        lua_pushstring(L, filetype);
    else
        lua_pushliteral(L, "TYPE_UNKNOWN");
    return 1;
}

/* DetermineTypeL(data pointer, size) -> string */
static int il_determinetypel(lua_State* L)
{
    const char* filetype = get_str_il_enum(ilDetermineTypeL(lua_topointer(L, 1), luaL_checkinteger(L, 2)));
    if (filetype)
        lua_pushstring(L, filetype);
    else
        lua_pushliteral(L, "TYPE_UNKNOWN");
    return 1;
}

/* Disable(enum) -> boolean */
static int il_disable(lua_State* L)
{
    return il_result_bool(L, ilDisable(get_il_enum(L, 1)));
}

/* DxtcDataToImage() -> boolean */
static int il_dxtcdatatoimage(lua_State* L)
{
    return il_result_bool(L, ilDxtcDataToImage());
}

/* DxtcDataToSurface() -> boolean */
static int il_dxtcdatatosurface(lua_State* L)
{
    return il_result_bool(L, ilDxtcDataToSurface());
}

/* Enable(enum) -> boolean */
static int il_enable(lua_State* L)
{
    return il_result_bool(L, ilEnable(get_il_enum(L, 1)));
}

/* FlipSurfaceDxtcData() -> none  */
static int il_flipsurfacedxtcdata(lua_State* L)
{
    ilFlipSurfaceDxtcData();
	lua_pushboolean(L, 1);
    return 1;
}

/* FormatFunc(enum) -> boolean */
static int il_formatfunc(lua_State* L)
{
    return il_result_bool(L, ilFormatFunc(get_il_enum(L, 1)));
}

/* GenImages(number) -> Number, Number, ... */
static int il_genimages(lua_State* L)
{
    int i, n = luaL_checkinteger(L, 1);
    ILuint* Images = (ILuint*)malloc(n * sizeof(ILuint));
    if (!Images)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "not enough memory");
        return 2;
    }
    ilGenImages(n, Images);
    for (i=0; i<n; i++)
    {
        lua_pushinteger(L, Images[i]);
    }
    return n;
}

/* GenImage() -> Number */
static int il_genimage(lua_State* L)
{
    return il_result_int(L, ilGenImage());
}

/* GetAlpha(enum) -> pointer */
static int il_getalpha(lua_State* L)
{
    void* alpha = ilGetAlpha(get_il_enum(L, 1));
    if (alpha)
    {
        lua_pushlightuserdata(L, alpha);
		lua_pushinteger(L, ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT));
        return 2;
    }
	return il_result_bool(L, 0);
}

/* GetBoolean(enum) -> boolean */
static int il_getboolean(lua_State* L)
{
    lua_pushboolean(L, ilGetBoolean(get_il_enum(L, 1)));
    return 1;
}

/* GetData() -> pointer */
static int il_getdata(lua_State* L)
{
    void* data = ilGetData();
    if (data)
    {
        lua_pushlightuserdata(L, data);
		lua_pushinteger(L, ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
        return 2;
    }
	return il_result_bool(L, 0);
}

/* GetDXTCData(enum) -> pointer */
static int il_getdxtcdata(lua_State* L)
{
    ILenum format = get_il_enum(L, 1);
    size_t size = ilGetDXTCData(NULL, 0, format);
    if (size > 0)
    {
        void* data = malloc(size);
        if (!data)
        {
            lua_pushnil(L);
			lua_pushstring(L, get_str_il_enum(IL_OUT_OF_MEMORY));
            return 2;
        }
        if (ilGetDXTCData(data, size, format) == 0)
        {
			return il_result_bool(L, 0);
		}
        lua_pushlightuserdata(L, data);
        lua_pushinteger(L, size);
        return 2;
    }
	return il_result_bool(L, 0);
}

/* GetInteger(enum) -> number */
static int il_getinteger(lua_State* L)
{
    lua_pushinteger(L, ilGetInteger(get_il_enum(L, 1)));
    return 1;
}

/* GetLumpPos() -> number */
static int il_getlumppos(lua_State* L)
{
    return il_result_int(L, ilGetLumpPos());
}

/* GetPalette() -> string */
static int il_getpalette(lua_State* L)
{
    void* palette = ilGetPalette();
    if (palette)
    {
        lua_pushlightuserdata(L, palette);
        lua_pushinteger(L, ilGetInteger(IL_PALETTE_SIZE));
        return 2;
    }
	return il_result_bool(L, 0);
}

/* GetString(enum) -> string */
static int il_getstring(lua_State* L)
{
    lua_pushstring(L, ilGetString(get_il_enum(L, 1)));
    return 1;
}

/* Hint(enum, enum) -> none */
static int il_hint(lua_State* L)
{
    ilHint(get_il_enum(L, 1), get_il_enum(L, 2));
    return 0;
}

/* InvertSurfaceDxtcDataAlpha() -> none */
static int il_invertsurfacedxtcdataalpha(lua_State* L)
{
    return il_result_bool(L, ilInvertSurfaceDxtcDataAlpha());
}

/* ImageToDxtcData() -> none */
static int il_imagetodxtcdata(lua_State* L)
{
    return il_result_bool(L, ilImageToDxtcData(get_il_enum(L, 1)));
}

/* IsDisabled(enum) -> boolean */
static int il_isdisabled(lua_State* L)
{
    lua_pushboolean(L, ilIsDisabled(get_il_enum(L, 1)));
    return 1;
}

/* IsEnabled(enum) -> boolean */
static int il_isenabled(lua_State* L)
{
    lua_pushboolean(L, ilIsEnabled(get_il_enum(L, 1)));
    return 1;
}

/* IsImage(enum) -> boolean */
static int il_isimage(lua_State* L)
{
    lua_pushboolean(L, ilIsImage(get_il_enum(L, 1)));
    return 1;
}

/* IsValid(enum, FileName) -> boolean */
static int il_isvalid(lua_State* L)
{
    lua_pushboolean(L, ilIsValid(get_il_enum(L, 1), luaL_checkstring(L, 2)));
    return 1;
}

/* IsValidF(enum, stream) -> boolean */
static int il_isvalidf(lua_State* L)
{
    lua_pushboolean(L, ilIsValidF(get_il_enum(L, 1), (ILHANDLE)lua_topointer(L, 2)));
    return 1;
}

/* IsValidL(enum, data pointer, size) -> boolean */
static int il_isvalidl(lua_State* L)
{
    lua_pushboolean(L, ilIsValidL(get_il_enum(L, 1), (void*)lua_topointer(L, 2), luaL_checkinteger(L, 3)));
    return 1;
}

/* KeyColour(Red, Green, Blue, Alpha) -> none */
static int il_keycolour(lua_State* L)
{
    ilKeyColour((ILclampf)luaL_checknumber(L, 1),
		(ILclampf)luaL_checknumber(L, 2),
		(ILclampf)luaL_checknumber(L, 3),
		(ILclampf)luaL_checknumber(L, 4));
    return 0;
}

/* Load(enum, FileName) -> boolean */
static int il_load(lua_State* L)
{
    return il_result_bool(L, ilLoad(get_il_enum(L, 1), luaL_checkstring(L, 2)));
}

/* LoadF(enum, stream) -> boolean */
static int il_loadf(lua_State* L)
{
    return il_result_bool(L, ilLoadF(get_il_enum(L, 1), (ILHANDLE)lua_topointer(L, 2)));
}

/* LoadImage(FileName) -> boolean */
static int il_loadimage(lua_State* L)
{
    return il_result_bool(L, ilLoadImage(luaL_checkstring(L, 1)));
}

/* LoadL(enum, data pointer, size) -> boolean */
static int il_loadl(lua_State* L)
{
    return il_result_bool(L, ilLoadL(get_il_enum(L, 1), (void*)lua_topointer(L, 2), luaL_checkinteger(L, 3)));
}

/* LoadPal(FileName) -> boolean */
static int il_loadpal(lua_State* L)
{
    return il_result_bool(L, ilLoadPal(luaL_checkstring(L, 1)));
}

/* ModAlpha(AlphaValue) -> none */
static int il_modalpha(lua_State* L)
{
    ilModAlpha(luaL_checknumber(L, 1));
    return 0;
}

/* ModAlpha(enum) -> boolean */
static int il_originfunc(lua_State* L)
{
    return il_result_bool(L, ilOriginFunc(get_il_enum(L, 1)));
}

/* OverlayImage(Source, XCoord, YCoord, ZCoord) -> boolean */
static int il_overlayimage(lua_State* L)
{
    return il_result_bool(L, ilOverlayImage(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                                            luaL_checkinteger(L, 3), luaL_checkinteger(L, 4)));
}

/* PopAttrib() -> none */
static int il_popattrib(lua_State* L)
{
    ilPopAttrib();
    return 0;
}

/* PushAttrib(mask) -> none */
static int il_pushattrib(lua_State* L)
{
    ilPushAttrib(get_il_mask(L, 1));
    return 0;
}

/* RegisterFormat(enum) -> none */
static int il_registerformat(lua_State* L)
{
    ilRegisterFormat(get_il_enum(L, 1));
    return 0;
}

/* RegisterLoad(Ext, loadfunc) -> boolean */
static int il_registerload(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    luaL_unref(L, LUA_REGISTRYINDEX, load_callback.LoadProcRef);
    lua_pushvalue(L, 2);
    load_callback.LoadProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    load_callback.L = L;
    return il_result_bool(L, ilRegisterLoad(luaL_checkstring(L, 1), load_proc_cb));
}

/* RegisterMipNum(Number) -> boolean */
static int il_registermipnum(lua_State* L)
{
    return il_result_bool(L, ilRegisterMipNum(luaL_checkinteger(L, 1)));
}

/* RegisterNumFaces(Number) -> boolean */
static int il_registernumfaces(lua_State* L)
{
    return il_result_bool(L, ilRegisterNumFaces(luaL_checkinteger(L, 1)));
}

/* RegisterNumImages(Number) -> boolean */
static int il_registernumimages(lua_State* L)
{
    return il_result_bool(L, ilRegisterNumImages(luaL_checkinteger(L, 1)));
}

/* RegisterOrigin(enum) -> none */
static int il_registerorigin(lua_State* L)
{
    ilRegisterOrigin(get_il_enum(L, 1));
    return 0;
}

/* RegisterPal(data pointer, size) -> none */
static int il_registerpal(lua_State* L)
{
    ilRegisterPal((void*)lua_topointer(L, 1), luaL_checkinteger(L, 2), get_il_enum(L, 3));
    return 0;
}

/* RegisterSave(Ext, savefunc) -> boolean */
static int il_registersave(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    luaL_unref(L, LUA_REGISTRYINDEX, save_callback.SaveProcRef);
    lua_pushvalue(L, 2);
    save_callback.SaveProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    save_callback.L = L;
    return il_result_bool(L, ilRegisterSave(lua_tostring(L, 1), save_proc_cb));
}

/* ilRegisterType(enum) -> none */
static int il_registertype(lua_State* L)
{
    ilRegisterType(get_il_enum(L, 1));
    return 0;
}

/* RemoveLoad(Ext) -> boolean */
static int il_removeload(lua_State* L)
{
    luaL_unref(L, LUA_REGISTRYINDEX, load_callback.LoadProcRef);
    load_callback.LoadProcRef = 0;
    load_callback.L = 0;
    return il_result_bool(L, ilRemoveLoad(luaL_checkstring(L, 1)));
}

/* RemoveSave(Ext) -> boolean */
static int il_removesave(lua_State* L)
{
    luaL_unref(L, LUA_REGISTRYINDEX, save_callback.SaveProcRef);
    save_callback.SaveProcRef = 0;
    save_callback.L = 0;
    return il_result_bool(L, ilRemoveSave(luaL_checkstring(L, 1)));
}

/* ResetRead() -> none */
static int il_resetread(lua_State* L)
{
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fCloseRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fEofProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fGetcProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fOpenRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fReadProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fSeekRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fTellRProcRef);
    read_callbacks.L = 0;
    ilResetRead();
    return 0;
}

/* ResetWrite() -> none */
static int il_resetwrite(lua_State* L)
{
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fCloseWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fOpenWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fPutcProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fSeekWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fTellWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fWriteProcRef);
    write_callbacks.L = 0;
    ilResetWrite();
    return 0;
}

/* Save(enum, FileName) -> boolean */
static int il_save(lua_State* L)
{
    return il_result_bool(L, ilSave(get_il_enum(L, 1), luaL_checkstring(L, 2)));
}

/* SaveF(enum, stream) -> boolean */
static int il_savef(lua_State* L)
{
    return il_result_int(L, ilSaveF(get_il_enum(L, 1), (ILHANDLE)lua_topointer(L, 2)));
}

/* SaveImage(FileName) -> boolean */
static int il_saveimage(lua_State* L)
{
    return il_result_bool(L, ilSaveImage(luaL_checkstring(L, 1)));
}

/* SavePal(FileName) -> boolean */
static int il_savepal(lua_State* L)
{
    return il_result_bool(L, ilSavePal(luaL_checkstring(L, 1)));
}

/* SetAlpha(AlphaValue) -> boolean */
static int il_setalpha(lua_State* L)
{
    return il_result_bool(L, ilSetAlpha(luaL_checknumber(L, 1)));
}

/* SetData(data pointer) -> boolean */
static int il_setdata(lua_State* L)
{
    return il_result_bool(L, ilSetData((void*)lua_topointer(L, 1)));
}

/* SetDuration(Duration) -> boolean */
static int il_setduration(lua_State* L)
{
    return il_result_bool(L, ilSetDuration(luaL_checkinteger(L, 1)));
}

/* SetInteger(enum, Number) -> none */
static int il_setinteger(lua_State* L)
{
    ilSetInteger(get_il_enum(L, 1), luaL_checkinteger(L, 2));
    return 0;
}

/* SetPixels(XOff, YOff, ZOff, Width, Height, Depth, Format, Type, Data pointer) -> none */
static int il_setpixels(lua_State* L)
{
    ilSetPixels(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
                luaL_checkinteger(L, 4), luaL_checkinteger(L, 5), luaL_checkinteger(L, 6),
                get_il_enum(L, 7), get_il_enum(L, 8), (void*)lua_topointer(L, 9));
    return 0;
}

/* SetRead(openfunc, closefunc, eoffunc, getcfunc, readfunc, seekfunc, tellfunc) -> none */
static int il_setread(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    luaL_checktype(L, 4, LUA_TFUNCTION);
    luaL_checktype(L, 5, LUA_TFUNCTION);
    luaL_checktype(L, 6, LUA_TFUNCTION);
    luaL_checktype(L, 7, LUA_TFUNCTION);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fOpenRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fCloseRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fEofProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fGetcProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fReadProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fSeekRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fTellRProcRef);
    lua_pushvalue(L, 1); read_callbacks.fOpenRProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 2); read_callbacks.fCloseRProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 3); read_callbacks.fEofProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 4); read_callbacks.fGetcProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 5); read_callbacks.fReadProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 6); read_callbacks.fSeekRProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 7); read_callbacks.fTellRProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ilSetRead(luafOpenRProc, luafCloseRProc, luafEofProc, luafGetcProc, luafReadProc, luafSeekRProc, luafTellRProc);
    return 0;
}

/* SetString(enum, string) -> none */
static int il_setstring(lua_State* L)
{
    ilSetString(get_il_enum(L, 1), luaL_checkstring(L, 2));
    return 0;
}

/* SetWrite(openfunc, closefunc, putcfunc, seekfunc, tellfunc, writefunc) -> none */
static int il_setwrite(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    luaL_checktype(L, 4, LUA_TFUNCTION);
    luaL_checktype(L, 5, LUA_TFUNCTION);
    luaL_checktype(L, 6, LUA_TFUNCTION);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fOpenWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fCloseWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fPutcProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fSeekWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fTellWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fWriteProcRef);
    lua_pushvalue(L, 1); write_callbacks.fOpenWProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 2); write_callbacks.fCloseWProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 3); write_callbacks.fPutcProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 4); write_callbacks.fSeekWProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 5); write_callbacks.fTellWProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 6); write_callbacks.fWriteProcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ilSetWrite(luafOpenWProc, luafCloseWProc, luafPutcProc, luafSeekWProc, luafTellWProc, luafWriteProc);
    return 0;
}


/* ShutDown() -> none */
/*
static int il_shutdown(lua_State* L)
{
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fCloseRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fEofProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fGetcProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fOpenRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fReadProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fSeekRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, read_callbacks.fTellRProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fCloseWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fOpenWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fPutcProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fSeekWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fTellWProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, write_callbacks.fWriteProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, save_callback.SaveProcRef);
    luaL_unref(L, LUA_REGISTRYINDEX, load_callback.LoadProcRef);
    ilShutDown();
    return 0;
}
*/

/* SurfaceToDxtcData(enum) -> boolean */
static int il_surfacetodxtcdata(lua_State* L)
{
    return il_result_bool(L, ilSurfaceToDxtcData(get_il_enum(L, 1)));
}

/* TexImage(Width, Height, Depth, NumChannels, Format, Type, Data pointer) -> boolean */
static int il_teximage(lua_State* L)
{
    return il_result_bool(L, ilTexImage(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
                                        luaL_checkinteger(L, 4), get_il_enum(L, 5), get_il_enum(L, 6),
                                        (void*)lua_topointer(L, 7)));
}

/* TexImageDxtc(w, h, d, DxtFormat, Data pointer) -> boolean */
static int il_teximagedxtc(lua_State* L)
{
    return il_result_bool(L, ilTexImageDxtc(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
                                            get_il_enum(L, 4), (const ILubyte*)lua_topointer(L, 5)));
}

/* TypeFromExt(FileName) -> enum */
static int il_typefromext(lua_State* L)
{
    const char* str = get_str_il_enum(ilTypeFromExt(luaL_checkstring(L, 1)));
    if (str)
        lua_pushstring(L, str);
    else
        lua_pushliteral(L, "TYPE_UNKNOWN");
    return 1;
}

/* TypeFunc(enum) -> boolean */
static int il_typefunc(lua_State* L)
{
    return il_result_bool(L, ilTypeFunc(get_il_enum(L, 1)));
}

/* LoadData(FileName, Width, Height, Depth, Bpp) -> boolean */
static int il_loaddata(lua_State* L)
{
    return il_result_bool(L, ilLoadData(luaL_checkstring(L, 1), luaL_checkinteger(L, 2),
                                        luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5)));
}

/* LoadDataF(stream, Width, Height, Depth, Bpp) -> boolean */
static int il_loaddataf(lua_State* L)
{
    return il_result_bool(L, ilLoadDataF((ILHANDLE)lua_topointer(L, 1), luaL_checkinteger(L, 2),
                                         luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5)));
}

/* LoadDataL(Data pointer, Size, Width, Height, Depth, Bpp) -> boolean */
static int il_loaddatal(lua_State* L)
{
    return il_result_bool(L, ilLoadDataL((void*)lua_topointer(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
                                         luaL_checkinteger(L, 4), luaL_checkinteger(L, 5), luaL_checkinteger(L, 6)));
}

/* SaveData(FileName) -> boolean */
static int il_savedata(lua_State* L)
{
    return il_result_bool(L, ilSaveData(luaL_checkstring(L, 1)));
}

/* FreePtr(Data pointer) -> none */
static int il_freeptr(lua_State* L)
{
	void* p = (void *)lua_topointer(L, 1);
	if (p)
	{
		free(p);
	}
	return 0;
}

static const luaL_reg illib[] =
{
    {"ActiveFace", il_active_face},
    {"ActiveImage", il_activeimage},
    {"ActiveLayer", il_activelayer},
    {"ActiveMipmap", il_activemipmap},
    {"ApplyPal", il_applypal},
    {"ApplyProfile", il_applyprofile},
    {"BindImage", il_bindimage},
    {"Blit",     il_blit},
    {"ClampNTSC", il_clampntsc},
    {"ClearColor", il_clearcolor},
    {"ClearImage", il_clearimage},
    {"CloneCurImage", il_clonecurimage},
    {"CompressDXT", il_compressdxt},
    {"CompressFunc", il_compressfunc},
    {"ConvertImage", il_convertimage},
    {"ConvertPal", il_convertpal},
    {"CopyImage", il_copyimage},
    {"CopyPixels", il_copypixels},
    {"CreateSubImage", il_createsubimage},
    {"DefaultImage", il_defaultimage},
    {"DeleteImage", il_deleteimage},
    {"DeleteImages", il_deleteimages},
    {"DetermineType", il_determinetype},
    {"DetermineTypeF", il_determinetypef},
    {"DetermineTypeL", il_determinetypel},
    {"Disable", il_disable},
    {"DxtcDataToImage", il_dxtcdatatoimage},
    {"DxtcDataToSurface", il_dxtcdatatosurface},
    {"Enable", il_enable},
    {"FlipSurfaceDxtcData", il_flipsurfacedxtcdata},
    {"FormatFunc", il_formatfunc},
    {"GenImages", il_genimages},
    {"GenImage", il_genimage},
    {"GetAlpha", il_getalpha},
    {"GetBoolean", il_getboolean},
    /*{"GetBooleanv", il_getbooleanv}, */
    {"GetData", il_getdata},
    {"GetDXTCData", il_getdxtcdata},
    {"GetError", il_geterror},
    {"GetInteger", il_getinteger},
    /*{"GetIntegerv", il_getintegerv},*/
    {"GetLumpPos", il_getlumppos},
    {"GetPalette", il_getpalette},
    {"GetString", il_getstring},
    {"Hint",     il_hint},
    {"InvertSurfaceDxtcDataAlpha", il_invertsurfacedxtcdataalpha},
    {"ImageToDxtcData", il_imagetodxtcdata},
    {"IsDisabled", il_isdisabled},
    {"IsEnabled", il_isenabled},
    {"IsImage", il_isimage},
    {"IsValid", il_isvalid},
    {"IsValidF", il_isvalidf},
    {"IsValidL", il_isvalidl},
    {"KeyColour", il_keycolour},
    {"Load",     il_load},
    {"LoadF",     il_loadf},
    {"LoadImage", il_loadimage},
    {"LoadL",     il_loadl},
    {"LoadPal", il_loadpal},
    {"ModAlpha", il_modalpha},
    {"OriginFunc", il_originfunc},
    {"OverlayImage", il_overlayimage},
    {"PopAttrib", il_popattrib},
    {"PushAttrib", il_pushattrib},
    {"RegisterFormat", il_registerformat},
    {"RegisterLoad", il_registerload},
    {"RegisterMipNum", il_registermipnum},
    {"RegisterNumFaces", il_registernumfaces},
    {"RegisterNumImages", il_registernumimages},
    {"RegisterOrigin", il_registerorigin},
    {"RegisterPal", il_registerpal},
    {"RegisterSave", il_registersave},
    {"RegisterType", il_registertype},
    {"RemoveLoad", il_removeload},
    {"RemoveSave", il_removesave},
    /*{"ResetMemory", il_resetmemory}, depricated */
    {"ResetRead", il_resetread},
    {"ResetWrite", il_resetwrite},
    {"Save",     il_save},
    {"SaveF",     il_savef},
    {"SaveImage", il_saveimage},
    /*{"SaveL",     il_savel},*/
    {"SavePal", il_savepal},
    {"SetAlpha", il_setalpha},
    {"SetData", il_setdata},
    {"SetDuration", il_setduration},
    {"SetInteger", il_setinteger},
    /*{"SetMemory", il_setmemory},*/
    {"SetPixels", il_setpixels},
    {"SetRead", il_setread},
    {"SetString", il_setstring},
    {"SetWrite", il_setwrite},
    /*{"ShutDown", il_shutdown},*/
    {"SurfaceToDxtcData", il_surfacetodxtcdata},
    {"TexImage", il_teximage},
    {"TexImageDxtc", il_teximagedxtc},
    {"TypeFromExt", il_typefromext},
    {"TypeFunc", il_typefunc},
    {"LoadData", il_loaddata},
    {"LoadDataF", il_loaddataf},
    {"LoadDataL", il_loaddatal},
    {"SaveData", il_savedata},
    {"FreePtr", il_freeptr},
    {NULL, NULL}
};

LUAIL_API int luaopen_luadevil(lua_State* L)
{
    luaL_openlib(L, "il", illib, 0);
#ifdef WITH_ILU
    luaopen_luailu(L);
#endif

#ifdef WITH_ILUT
    luaopen_luailut(L);
#endif

    ilInit();
    return 1;
}

#ifdef __cplusplus
}
#endif

