/*
** rawpagetexture.cpp
** Texture class for Raven's raw fullscreen pages
**
**---------------------------------------------------------------------------
** Copyright 2004-2006 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#include "wl_def.h"
#include "files.h"
#include "w_wad.h"
#include "v_palette.h"
#include "textures.h"


//==========================================================================
//
// A raw 320x200 graphic used by Heretic and Hexen fullscreen images
//
//==========================================================================

class FRawPageTexture : public FTexture
{
public:
	FRawPageTexture (int lumpnum);
	~FRawPageTexture ();

	const BYTE *GetColumn (unsigned int column, const Span **spans_out);
	const BYTE *GetPixels ();
	void Unload ();

protected:
	BYTE *Pixels;
	static const Span DummySpans[2];

	void MakeTexture ();
};

//==========================================================================
//
// RAW textures must be exactly 64000 bytes long and not be identifiable
// as Doom patch
//
//==========================================================================

static bool CheckIfRaw(FileReader & data)
{
	if (data.GetLength() != 64000) return false;

	// This is probably a raw page graphic, but do some checking to be sure
	patch_t *foo;
	int height;
	int width;

	foo = (patch_t *)M_Malloc (data.GetLength());
	data.Seek (0, SEEK_SET);
	data.Read (foo, data.GetLength());

	height = LittleShort(foo->height);
	width = LittleShort(foo->width);

	if (height > 0 && height < 510 && width > 0 && width < 15997)
	{
		// The dimensions seem like they might be valid for a patch, so
		// check the column directory for extra security. At least one
		// column must begin exactly at the end of the column directory,
		// and none of them must point past the end of the patch.
		bool gapAtStart = true;
		int x;

		for (x = 0; x < width; ++x)
		{
			DWORD ofs = LittleLong(foo->columnofs[x]);
			if (ofs == (DWORD)width * 4 + 8)
			{
				gapAtStart = false;
			}
			else if (ofs >= 64000-1)	// Need one byte for an empty column
			{
				free (foo);
				return true;
			}
			else
			{
				// Ensure this column does not extend beyond the end of the patch
				const BYTE *foo2 = (const BYTE *)foo;
				while (ofs < 64000)
				{
					if (foo2[ofs] == 255)
					{
						free (foo);
						return true;
					}
					ofs += foo2[ofs+1] + 4;
				}
				if (ofs >= 64000)
				{
					free (foo);
					return true;
				}
			}
		}
		if (gapAtStart || (x != width))
		{
			M_Free (foo);
			return true;
		}
		M_Free(foo);
		return false;
	}
	else
	{
		M_Free (foo);
		return true;
	}
}

//==========================================================================
//
//
//
//==========================================================================

FTexture *RawPageTexture_TryCreate(FileReader & file, int lumpnum)
{
	if (!CheckIfRaw(file)) return NULL;
	return new FRawPageTexture(lumpnum);
}


//==========================================================================
//
//
//
//==========================================================================

const FTexture::Span FRawPageTexture::DummySpans[2] =
{
	{ 0, 200 }, { 0, 0 }
};

//==========================================================================
//
//
//
//==========================================================================

FRawPageTexture::FRawPageTexture (int lumpnum)
: FTexture(NULL, lumpnum), Pixels(0)
{
	Width = 320;
	Height = 200;
	WidthBits = 8;
	HeightBits = 8;
	WidthMask = 255;
}

//==========================================================================
//
//
//
//==========================================================================

FRawPageTexture::~FRawPageTexture ()
{
	Unload ();
}

//==========================================================================
//
//
//
//==========================================================================

void FRawPageTexture::Unload ()
{
	if (Pixels != NULL)
	{
		delete[] Pixels;
		Pixels = NULL;
	}
}

//==========================================================================
//
//
//
//==========================================================================

const BYTE *FRawPageTexture::GetColumn (unsigned int column, const Span **spans_out)
{
	if (Pixels == NULL)
	{
		MakeTexture ();
	}
	if ((unsigned)column >= (unsigned)Width)
	{
		column %= 320;
	}
	if (spans_out != NULL)
	{
		*spans_out = DummySpans;
	}
	return Pixels + column*Height;
}

//==========================================================================
//
//
//
//==========================================================================

const BYTE *FRawPageTexture::GetPixels ()
{
	if (Pixels == NULL)
	{
		MakeTexture ();
	}
	return Pixels;
}

//==========================================================================
//
//
//
//==========================================================================

void FRawPageTexture::MakeTexture ()
{
	FMemLump lump = Wads.ReadLump (SourceLump);
	const BYTE *source = (const BYTE *)lump.GetMem();
	const BYTE *source_p = source;
	BYTE *dest_p;

	Pixels = new BYTE[Width*Height];
	dest_p = Pixels;

	// Convert the source image from row-major to column-major format
	for (int y = 200; y != 0; --y)
	{
		for (int x = 320; x != 0; --x)
		{
			*dest_p = GPalette.Remap[*source_p];
			dest_p += 200;
			source_p++;
		}
		dest_p -= 200*320-1;
	}
}

