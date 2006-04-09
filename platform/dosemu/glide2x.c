#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "glide.h"
#include "glide2x.h"

#define VERSION 11

#define __export
#define __stdcall __attribute__((__stdcall__))

#define __fptr(type) \
struct fptr_##type##_t \
{ \
    void          *entry; \
    type##_t      *thunk; \
    unsigned short segment; \
    unsigned int   offset; \
}

#define __faddr(addr) asm volatile ("mov %1, %%eax; add %%eax, %0": "=g"(addr): "m"(fptr.offset): "%eax")

#define __fcall(...) \
({ \
    unsigned int ret; \
    /* Typesafe thunk */ \
    asm volatile ( \
    /* Return address to DOS memory space */ \
        "push %%cs\n\t" \
        "push $1f\n\t" \
        "xor  %%ebx, %%ebx" \
        : : : "%ebx", "memory"); \
    (*(fptr.thunk)) (__VA_ARGS__); \
    asm volatile ( \
    /* Remove stale return address and stack entry point */ \
        "xchg %%ebx, %%esp\n\t" \
        "pop  %%eax\n\t" \
        "push %1\n\t" \
    /* Enter host OS memory space */ \
        "push %2\n\t" \
        "mov  thunk1, %%eax\n\t" \
        "add  %3, %%eax\n\t" \
        "push %%eax\n\t" \
        "lret\n\t" \
    /* Returned to DOS memory space */ \
        "1:\tmov  %%eax, %0" \
        :"=g"(ret) \
        :"m"(fptr.entry), "m"(fptr.segment), "m"(fptr.offset) \
        :"%eax", "memory"); \
    ret; \
})

#define DECLARE_THUNK(func,ret,...) \
typedef ret (func##_t) (__VA_ARGS__); \
ret func (__VA_ARGS__) \
{ \
    static __fptr(func) fptr = {0}; \
    printf ("%s\n", __func__); \
    if (!fptr.entry) \
    { \
        fptr.entry = entry (CMD_##func, &fptr.segment); \
        fptr.thunk = (func##_t *) &thunk; \
        /*asm ("push %es; lea %%cs:(0), %0, lea %%, %eaxfptr.offset = */ \
        assert (fptr.entry); \
    } {

#define DECLARE_STUB(func,ret,...) \
ret func (__VA_ARGS__) \
{ \
    printf ("%s\n", __func__); {

#define ENDDECLARE }}

void thunk (void);

static struct vxd_driver
{
    const    char  name[8];
    unsigned short segment;
    unsigned int   offset;
} vxd =
{
    .name[0] = 'O',
    .name[1] = 'P',
    .name[2] = 'E',
    .name[3] = 'N',
    .name[4] = 'G',
    .name[5] = 'L',
    .name[6] = 'D',
    .name[7] = '1',
    .segment = 0,
    .offset  = 0
};

static void *entry (unsigned int cmd, unsigned short *segment)
{
    void *func;

    if (!(vxd.segment || vxd.offset))
    {
        asm ("push %%es\n\t"
             "mov %2, %%edi\n\t"
             "mov $0x1684, %%eax\n\t"
             "mov $0, %%ebx\n\t"
             "int $0x2f\n\t"
             "mov %%es, %0\n\t"
             "mov %%edi, %1\n\t"
             "pop %%es"
             : "=m"(vxd.segment), "=m"(vxd.offset) /* Outputs */
             : "r"(vxd.name)                       /* Inputs */
             : "%eax", "%ebx", "%edi", "memory"    /* Clobbers */
            );
        assert (vxd.segment || vxd.offset);
    }

    asm ("mov  %1, %%eax\n\t"
         "push %%cs\n\t"
         "push $1f\n\t"
         "push %2\n\t"
         "push %3\n\t"
         "lret\n"
         "1:\tmov  %%eax, %0"
         : "=r"(func)                        /* Outputs */
         : "r"(cmd),
           "m"(vxd.segment), "m"(vxd.offset) /* Inputs */
         : "%eax", "memory"                  /* Clobbers */
        );

    *segment = vxd.segment;
}

/* This function is never run bus creates two static "naked" functions */
static void __attribute__((__used__)) thunk2 (void)
{
    asm volatile (
    /* Dummy to get parameters stacked and stack frame size */
        ".text\n"
        "_thunk:\tmov  %esp, %ebx\n\t"
        "ret\n\t"
    /* Entered host OS memory space */
        ".text\n\t"
        "thunk1:\tpop  %eax\n\t"
        "call %eax\n\t"
    /* Return to DOS memory space */
        "xchg %esp, %ebx\n\t"
        "lret\n");
}

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_4_8, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_4_WIDES, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_8_1, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_8_2, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_8_4, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_8_WIDES, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_16_1, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_16_2, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_16_WIDES, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_32_1, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOAD_DEFAULT_32_WIDES, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(GRSPLASH, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(GU3DFGETINFO, FxBool, char *filename, Gu3dfInfo *file_info)
    /*
     * Pretend file doesn't exist to get TRI to
     * generate textures on the fly.
     * FIXME: wont work on most games, probably
     */
    return FXFALSE;
/*
    __faddr (filename);
    __faddr (file_info);
    return __fcall (filename, file_info);
*/
ENDDECLARE

DECLARE_STUB(GU3DFLOAD, FxBool, char *filename, Gu3dfInfo *file_info)
    return FXFALSE;
/*
    __faddr (filename);
    __faddr (file_info);
    return __fcall (filename, file_info);
*/
ENDDECLARE

DECLARE_THUNK(GUALPHASOURCE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GUCOLORCOMBINEFUNCTION, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_STUB(GUENDIANSWAPWORDS, void, int a)
ENDDECLARE

DECLARE_STUB(GUENDIANSWAPBYTES, void, int a)
ENDDECLARE

DECLARE_STUB(GUFOGTABLEINDEXTOW, void, int a)
ENDDECLARE

DECLARE_THUNK(GUFOGGENERATEEXP, void, GrFog_t *table, int density)
    __faddr (table);
    __fcall (table, density);
ENDDECLARE

DECLARE_STUB(GUFOGGENERATEEXP2, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GUFOGGENERATELINEAR, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(GUTEXCREATECOLORMIPMAP, void, void)
ENDDECLARE

DECLARE_STUB(GUENCODERLE16, void, int a, int b, int c, int d)
ENDDECLARE

DECLARE_THUNK(GUDRAWTRIANGLEWITHCLIP, void, GrVertex *v1, GrVertex *v2, GrVertex *v3)
    __faddr (v1);
    __faddr (v2);
    __faddr (v3);
    __fcall (v1, v2, v3);
ENDDECLARE

DECLARE_STUB(GUAADRAWTRIANGLEWITHCLIP, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(GUDRAWPOLYGONVERTEXLISTWITHCLIP, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GUMPINIT, void, void)
ENDDECLARE

DECLARE_STUB(GUMPTEXCOMBINEFUNCTION, void, int a)
ENDDECLARE

DECLARE_STUB(GUMPTEXSOURCE, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GUFBREADREGION, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(GUFBWRITEREGION, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(GRGLIDEGETVERSION, void, char version[80])
    memcpy(version, "2.48.00.0455", 13);
ENDDECLARE

DECLARE_STUB(GRGLIDEGETSTATE, void, int a)
ENDDECLARE

DECLARE_THUNK(GRHINTS, void, int a, int b)
    __fcall (a, b);
ENDDECLARE

DECLARE_STUB(_GRGLIDEINIT, void, int version)
#warning TODO
ENDDECLARE

DECLARE_STUB(GRGLIDESHAMELESSPLUG, void, int a)
ENDDECLARE

DECLARE_STUB(GRRESETTRISTATS, void, void)
ENDDECLARE

DECLARE_STUB(GRTRISTATS, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GRSSTQUERYBOARDS, void, int a)
ENDDECLARE

DECLARE_THUNK(GRSSTQUERYHARDWARE, FxBool, GrHwConfiguration *hwConfig)
    __faddr (hwConfig);
    return __fcall (hwConfig);
ENDDECLARE

DECLARE_THUNK(GRSSTSELECT, void, int which_sst)
    __fcall (which_sst);
ENDDECLARE

DECLARE_STUB(GRSSTSCREENWIDTH, void, void)
ENDDECLARE

DECLARE_STUB(GRSSTSCREENHEIGHT, void, void)
ENDDECLARE

DECLARE_STUB(GRSSTVIDMODE, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GRTEXCALCMEMREQUIRED, FxU32, GrLOD_t lodmin, GrLOD_t lodmax, GrAspectRatio_t aspect, GrTextureFormat_t fmt)
#if 0
    int size, i;

    size = 0;

    for(i = lodmax; i <= lodmin; i++)
        size += texture_size(i, aspect, fmt);

    DPRINTF("size = %x\n", size);

    return size;
#endif
    return 0;
ENDDECLARE

/* FIXME? */
DECLARE_STUB(GRTEXDETAILCONTROL, void, int a, int b, int c, int d)
#if 0
    send_char(CMD_GrTexDetailControl);
#endif
ENDDECLARE

/* FIXME? */
DECLARE_STUB(GRTEXMINADDRESS, FxU32, int a)
    return 8;
ENDDECLARE

/* FIXME? */
DECLARE_STUB(GRTEXMAXADDRESS, FxU32, int a)
    return 8*1024*1024 - 128*1024;
ENDDECLARE

/* FIXME */
DECLARE_STUB(GRTEXTEXTUREMEMREQUIRED, int, FxU32 evenOdd, GrTexInfo *info)
    return 0;
ENDDECLARE

DECLARE_THUNK(GRTEXDOWNLOADMIPMAP, void, GrChipID_t tmu, FxU32 startAddress,
                                   FxU32 evenOdd, GrTexInfo *info)
    __faddr (info);
    __fcall (tmu, startAddress, evenOdd, info);
ENDDECLARE

DECLARE_STUB(GRTEXDOWNLOADTABLEPARTIAL, void, int a, int b, int c, int d, int e)
ENDDECLARE

/* FIXME? */
DECLARE_STUB(GRTEXDOWNLOADMIPMAPLEVEL, void,
                                       GrChipID_t        tmu,
                                       FxU32             startAddress,
                                       GrLOD_t           thisLod,
                                       GrLOD_t           largeLod,
                                       GrAspectRatio_t   aspectRatio,
                                       GrTextureFormat_t format,
                                       FxU32             evenOdd,
                                       void             *data)
    GrTexInfo info;

    if(thisLod == largeLod)
    {
        info.aspectRatio = aspectRatio;
        info.data        = data;
        info.format      = format;
        info.largeLod    = largeLod;
        info.smallLod    = largeLod;

        GRTEXDOWNLOADMIPMAP(tmu, startAddress, evenOdd, &info);
    }
ENDDECLARE

DECLARE_STUB(GRERRORSETCALLBACK, void, int a)
ENDDECLARE

DECLARE_STUB(GUMOVIESTART, void, void)
ENDDECLARE

DECLARE_STUB(GUMOVIESTOP, void, void)
ENDDECLARE

DECLARE_STUB(GUMOVIESETNAME, void, int a)
ENDDECLARE

DECLARE_THUNK(GUTEXALLOCATEMEMORY, int,
                                   GrChipID_t tmu, FxU8 odd_even_mask,
                                   int width, int height, GrTextureFormat_t fmt,
                                   GrMipMapMode_t mm_mode, GrLOD_t smallest_lod,
                                   GrLOD_t largest_lod, GrAspectRatio_t aspect,
                                   GrTextureClampMode_t s_clamp_mode,
                                   GrTextureClampMode_t t_clamp_mode,
                                   GrTextureFilterMode_t minfilter_mode,
                                   GrTextureFilterMode_t magfilter_mode,
                                   int lod_bias, FxBool trilinear)
    return __fcall (tmu, odd_even_mask, width, height, fmt,
                    mm_mode, smallest_lod, largest_lod, aspect,
                    s_clamp_mode, t_clamp_mode, minfilter_mode,
                    magfilter_mode, lod_bias, trilinear);
ENDDECLARE

DECLARE_STUB(GUTEXCHANGEATTRIBUTES, void, int a, int b, int c, int d,
                                    int e, int f, int g, int h,
                                    int i, int j, int k, int l)
ENDDECLARE

DECLARE_THUNK(GRTEXCOMBINEFUNCTION, void, int a, int b)
    __fcall (a, b);
ENDDECLARE

/* FIXME */
DECLARE_STUB(GUTEXCOMBINEFUNCTION, void, int a, int b)
/*    __fcall (a, b); */
ENDDECLARE

DECLARE_THUNK(GUTEXDOWNLOADMIPMAP, void, GrMipMapId_t mmid, void *src, GuNccTable *table)
    __faddr (src);
    __faddr (table);
    __fcall (mmid, src, table);
ENDDECLARE

DECLARE_STUB(GUTEXDOWNLOADMIPMAPLEVEL, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(GUTEXGETCURRENTMIPMAP, void, int a)
ENDDECLARE

DECLARE_STUB(GUTEXGETMIPMAPINFO, void, int a)
ENDDECLARE

/* FIXME */
DECLARE_STUB(GUTEXMEMQUERYAVAIL, FxU32, int a)
    return 16 * 1024 * 1024;
ENDDECLARE

DECLARE_THUNK(GUTEXMEMRESET, void, void)
    __fcall ();
ENDDECLARE

DECLARE_STUB(_GUMPTEXCOMBINEFUNCTION, void, int a)
ENDDECLARE

DECLARE_STUB(GUMPDRAWTRIANGLE, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(GRAADRAWPOINT, void, int a)
ENDDECLARE

DECLARE_THUNK(GRAADRAWLINE, void, GrVertex *v1, GrVertex *v2)
    __faddr (v1);
    __faddr (v2);
    __fcall (v1, v2);
ENDDECLARE

DECLARE_STUB(GRAADRAWTRIANGLE, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(GRAADRAWPOLYGON, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(GRAADRAWPOLYGONVERTEXLIST, void, int nverts, GrVertex vlist[])
ENDDECLARE

DECLARE_STUB(GRDRAWPOINT, void, int a)
ENDDECLARE

DECLARE_THUNK(GRDRAWLINE, void, GrVertex *v1, GrVertex *v2)
    __faddr (v1);
    __faddr (v2);
    __fcall (v1, v2);
ENDDECLARE

DECLARE_THUNK(GRDRAWTRIANGLE, void, GrVertex *v1, GrVertex *v2, GrVertex *v3)
    __faddr (v1);
    __faddr (v2);
    __faddr (v3);
    __fcall (v1, v2, v3);
ENDDECLARE

DECLARE_STUB(GRDRAWPLANARPOLYGON, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(GRDRAWPLANARPOLYGONVERTEXLIST, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GRDRAWPOLYGON, void, int a, int b, int c)
ENDDECLARE

DECLARE_THUNK(GRDRAWPOLYGONVERTEXLIST, void, int nverts, GrVertex *vlist)
    __faddr (vlist);
    __fcall (nverts, vlist);
ENDDECLARE

DECLARE_STUB(_GRCOLORCOMBINEDELTA0MODE, void, int a)
ENDDECLARE

DECLARE_THUNK(GRALPHABLENDFUNCTION, void, int a, int b, int c, int d)
    __fcall (a, b, c, d);
ENDDECLARE

DECLARE_THUNK(GRALPHACOMBINE, void, int a, int b, int c, int d, int e)
    __fcall (a, b, c, d, e);
ENDDECLARE

DECLARE_STUB(GRALPHACONTROLSITRGBLIGHTING, void, int a)
ENDDECLARE

DECLARE_THUNK(GRALPHATESTFUNCTION, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_STUB(GRALPHATESTREFERENCEVALUE, void, int a)
ENDDECLARE

DECLARE_THUNK(GRBUFFERCLEAR, void, int a, int b, int c)
    __fcall (a, b, c);
ENDDECLARE

/* FIXME? */
DECLARE_THUNK(GRBUFFERSWAP, void, int a)
    __fcall (a);
ENDDECLARE

/* FIXME? */
DECLARE_STUB(GRBUFFERNUMPENDING, int, void)
    return 0;
ENDDECLARE

DECLARE_THUNK(GRCHROMAKEYMODE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRCHROMAKEYVALUE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRCLIPWINDOW, void, int a, int b, int c, int d)
    __fcall (a, b, c, d);
ENDDECLARE

DECLARE_THUNK(GRCOLORCOMBINE, void, int a, int b, int c, int d, int e)
    __fcall (a, b, c, d, e);
ENDDECLARE

DECLARE_THUNK(GRCOLORMASK, void, int a, int b)
    __fcall (a, b);
ENDDECLARE

DECLARE_THUNK(GRCONSTANTCOLORVALUE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRCONSTANTCOLORVALUE4, void, int a, int b, int c, int d)
    __fcall (a, b, c, d);
ENDDECLARE

DECLARE_THUNK(GRCULLMODE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRDEPTHBIASLEVEL, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRDEPTHBUFFERFUNCTION, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRDEPTHBUFFERMODE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRDEPTHMASK, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRDISABLEALLEFFECTS, void, void)
    __fcall ();
ENDDECLARE

DECLARE_STUB(GRDITHERMODE, void, int a)
ENDDECLARE

DECLARE_THUNK(GRFOGMODE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRFOGCOLORVALUE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_THUNK(GRFOGTABLE, void, GrFog_t *ft)
    __faddr (ft);
    __fcall (ft);
ENDDECLARE

/* FIXME? */
DECLARE_THUNK(GRGLIDESHUTDOWN, void, void)
    __fcall ();
ENDDECLARE

DECLARE_STUB(GRGLIDESETSTATE, void, int a)
ENDDECLARE

DECLARE_THUNK(GRRENDERBUFFER, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_STUB(GRCHECKFORROOM, void, int a)
ENDDECLARE

DECLARE_STUB(_GRUPDATEPARAMINDEX, void, void)
ENDDECLARE

DECLARE_STUB(_GRREBUILDDATALIST, void, void)
ENDDECLARE

DECLARE_STUB(GRLFBCONSTANTALPHA, void, int a)
ENDDECLARE

DECLARE_STUB(GRLFBCONSTANTDEPTH, void, int a)
ENDDECLARE

/* FIXME! */
DECLARE_THUNK(GRLFBLOCK, FxBool,
                         GrLock_t           type,
                         GrBuffer_t         buffer,
                         GrLfbWriteMode_t   writeMode,
                         GrOriginLocation_t origin,
                         FxBool             pixelPipeline,
                         GrLfbInfo_t       *info)
#if 0
    info->strideInBytes = 2 * 1024;
    info->writeMode = GR_LFBWRITEMODE_565;
    info->origin = GR_ORIGIN_UPPER_LEFT;

    if((type & 1) == 1)
    {
        info->lfbPtr = lfb_write;

        if(!lfb_write_pending)
        {
            setup_blue_screen();
            buffer_is_locked = 1;
            locked_buffer = buffer;
        }


        lfb_write_pending = 0;
    }

    else
    {
        char c;
        int x, y;

        info->lfbPtr = lfb_read;

        if(!disable_lfb_read)
        {
            if(buffer == GR_BUFFER_AUXBUFFER)
            {
                for(y = 0; y < 480; y++)
                    for(x = 0; x < 640; x++)
                        lfb_read[y][x] = 0xB000;
            }

            else
            {
                send_char(CMD_GrLFBLock);
                send_int(buffer);
                send_context_switch();

                for(y = 0; y < 480; y++)
                    pipe->get((char *)lfb_read[y], 640*2);
            }

        }

    }

#endif
    return FXTRUE;
ENDDECLARE

/* FIXME! */
DECLARE_STUB(GRLFBUNLOCK, FxBool, GrLock_t type, GrBuffer_t buffer)
#if 0
    if((type & 1) == 1)
    {
       /*
        * If we are delaying LFB writes and some triangles
        * have been drawn this frame, then just mark that
        * there are writes yet to be sent.  If no triangles
        * have yet been drawn then the purpose of the writes
        * is probably to create a background so send them now.
        */
        if(delay_lfb_write && frame_started)
        {
            lfb_write_pending = 1;
        }

        else
        {
            send_blue_screen(buffer);
            buffer_is_locked = 0;
        }

    }

#endif
    return FXTRUE;
ENDDECLARE

DECLARE_STUB(GRLFBWRITECOLORFORMAT, void, int a)
ENDDECLARE

DECLARE_STUB(GRLFBWRITECOLORSWIZZLE, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GRLFBWRITEREGION, void, int a, int b, int c, int d,
                               int e, int f, int g, int h)
ENDDECLARE

DECLARE_STUB(GRLFBREADREGION, void, int a, int b, int c, int d,
                              int e, int f, int g)
ENDDECLARE

DECLARE_THUNK(GRSSTWINOPEN, FxBool, int a, int b, int c,
                      int d, int e, int f, int g)
    return __fcall (a, b, c, d, e, f, g);
ENDDECLARE

DECLARE_THUNK(GRSSTWINCLOSE, void, void)
    __fcall ();
ENDDECLARE

DECLARE_STUB(GRSSTCONTROL, void, int a)
ENDDECLARE

DECLARE_STUB(GRSSTPERFSTATS, void, int a)
ENDDECLARE

DECLARE_STUB(GRSSTRESETPERFSTATS, void, void)
ENDDECLARE

/* FIXME */
DECLARE_STUB(GRSSTSTATUS, int, void)
    return 0x0FFFF43F;
ENDDECLARE

DECLARE_STUB(GRSSTVIDEOLINE, void, void)
ENDDECLARE

DECLARE_STUB(GRSSTVRETRACEON, void, void)
ENDDECLARE

DECLARE_STUB(GRSSTIDLE, void, void)
ENDDECLARE

DECLARE_STUB(GRSSTISBUSY, void, void)
ENDDECLARE

DECLARE_THUNK(GRGAMMACORRECTIONVALUE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_STUB(GRSSTORIGIN, void, int a)
ENDDECLARE

DECLARE_STUB(GRSSTCONFIGPIPELINE, void, int a, int b, int c)
ENDDECLARE

DECLARE_THUNK(GRTEXCLAMPMODE, void, int a, int b, int c)
    __fcall (a, b, c);
ENDDECLARE

DECLARE_THUNK(GRTEXCOMBINE, void, int a, int b, int c, int d,
                            int e, int f, int g)
    __fcall (a, b, c, d, e, f, g);
ENDDECLARE

DECLARE_STUB(_GRTEXDETAILCONTROL, void, int a, int b)
ENDDECLARE

DECLARE_THUNK(GRTEXFILTERMODE, void, int a, int b, int c)
    __fcall (a, b, c);
ENDDECLARE

DECLARE_THUNK(GRTEXLODBIASVALUE, void, int a, int b)
    __fcall (a, b);
ENDDECLARE

DECLARE_THUNK(GRTEXMIPMAPMODE, void, GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend)
    __fcall (tmu, mode, lodBlend);
ENDDECLARE

DECLARE_STUB(GRTEXNCCTABLE, void, int a, int b)
ENDDECLARE

DECLARE_THUNK(GRTEXSOURCE, void, GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info)
    __faddr (info);
    __fcall (tmu, startAddress, evenOdd, info);
ENDDECLARE

DECLARE_STUB(GRTEXMULTIBASE, void, int a, int b)
ENDDECLARE

DECLARE_STUB(GRTEXMULTIBASEADDRESS, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOADNCCTABLE, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(_GRTEXDOWNLOADPALETTE, void, int a, int b, int c, int d)
ENDDECLARE

DECLARE_THUNK(GRTEXDOWNLOADTABLE, void, GrChipID_t tmu, GrTexTable_t type, void *data)
    __faddr (data);
    __fcall (tmu, type, data);
ENDDECLARE

DECLARE_STUB(GRTEXDOWNLOADMIPMAPLEVELPARTIAL, void, int a, int b, int c, int d,
                                              int e, int f, int g, int h,
                                              int i, int j)
ENDDECLARE

DECLARE_THUNK(GUTEXSOURCE, void, int a)
    __fcall (a);
ENDDECLARE

DECLARE_STUB(_GRCOMMANDTRANSPORTMAKEROOM, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(PIOINBYTE, void, int a)
ENDDECLARE

DECLARE_STUB(PIOINWORD, void, int a)
ENDDECLARE

DECLARE_STUB(PIOINLONG, void, int a)
ENDDECLARE

DECLARE_STUB(PIOOUTBYTE, void, int a, int b)
ENDDECLARE

DECLARE_STUB(PIOOUTWORD, void, int a, int b)
ENDDECLARE

DECLARE_STUB(PIOOUTLONG, void, int a, int b)
ENDDECLARE

DECLARE_STUB(PCIPRINTDEVICELIST, void, void)
ENDDECLARE

DECLARE_STUB(_PCIFETCHREGISTER, void, int a, int b, int c, int d)
ENDDECLARE

DECLARE_STUB(_PCIUPDATEREGISTER, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCIGETERRORSTRING, void, void)
ENDDECLARE

DECLARE_STUB(PCIGETERRORCODE, void, void)
ENDDECLARE

DECLARE_STUB(PCIOPENEX, void, int a)
ENDDECLARE

DECLARE_STUB(PCIOPEN, void, void)
ENDDECLARE

DECLARE_STUB(PCICLOSE, void, void)
ENDDECLARE

DECLARE_STUB(PCIDEVICEEXISTS, void, int a)
ENDDECLARE

DECLARE_STUB(PCIGETCONFIGDATA, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCIGETCONFIGDATARAW, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCISETCONFIGDATA, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCISETCONFIGDATARAW, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCIFINDCARDMULTI, void, int a, int b, int c, int d)
ENDDECLARE

DECLARE_STUB(PCIFINDCARDMULTIFUNC, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCIFINDCARD, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(PCIMAPCARDMULTI, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(PCIMAPCARDMULTIFUNC, void, int a, int b, int c, int d, int e, int f)
ENDDECLARE

DECLARE_STUB(PCIMAPCARD, void, int a, int b, int c, int d, int e)
ENDDECLARE

DECLARE_STUB(PCIMAPPHYSICALTOLINEAR, void, int a, int b, int c)
ENDDECLARE

DECLARE_STUB(PCIMAPPHYSICALDEVICETOLINEAR, void, int a, int b, int c, int d)
ENDDECLARE

DECLARE_STUB(PCIUNMAPPHYSICAL, void, int a, int b)
ENDDECLARE

DECLARE_STUB(PCISETPASSTHROUGHBASE, void, int a, int b)
ENDDECLARE

DECLARE_STUB(PCIOUTPUTDEBUGSTRING, void, int a)
ENDDECLARE

DECLARE_STUB(PCILINEARRANGESETPERMISSION, void, int a, int b, int c)
ENDDECLARE