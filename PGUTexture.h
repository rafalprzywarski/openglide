// PGUTexture.h: interface for the PGUTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PGUTEXTURE_H__890C0F0B_EE56_4AD3_9A08_4622F62B9A81__INCLUDED_)
#define AFX_PGUTEXTURE_H__890C0F0B_EE56_4AD3_9A08_4622F62B9A81__INCLUDED_

#include "Glide.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PGUTexture  
{
    enum {MAX_MM = 500};
    GrMipMapInfo mm_info[MAX_MM];
    FxU32 mm_start[MAX_MM];
    FxU32 m_free_mem;
    GrMipMapId_t m_free_id;
    GrMipMapId_t m_current_id;
public:
	FxU32 MemQueryAvail(GrChipID_t tmu);
	GrMipMapId_t GetCurrentMipMap( GrChipID_t tmu );
	FxBool ChangeAttributes(GrMipMapId_t mmid, int width, int height, GrTextureFormat_t fmt, GrMipMapMode_t mm_mode, GrLOD_t smallest_lod, GrLOD_t largest_lod, GrAspectRatio_t aspect, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode, GrTextureFilterMode_t minFilterMode, GrTextureFilterMode_t magFilterMode);
	GrMipMapInfo * GetMipMapInfo(GrMipMapId_t mmid);
	void Source(GrMipMapId_t id);
	void MemReset();
	void DownloadMipMapLevel( GrMipMapId_t mmid, GrLOD_t lod, const void **src );
	void DownloadMipMap( GrMipMapId_t mmid, const void *src, const GuNccTable *table );
	GrMipMapId_t AllocateMemory(GrChipID_t tmu, FxU8 odd_even_mask, int width, int height, GrTextureFormat_t fmt, GrMipMapMode_t mm_mode, GrLOD_t smallest_lod, GrLOD_t largest_lod, GrAspectRatio_t aspect, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode, float lod_bias, FxBool trilinear );
	PGUTexture();
	virtual ~PGUTexture();
};

#endif // !defined(AFX_PGUTEXTURE_H__890C0F0B_EE56_4AD3_9A08_4622F62B9A81__INCLUDED_)
