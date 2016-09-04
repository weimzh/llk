//
// Copyright (c) 2005, 2006 Wei Mingzhi <whistler@openoffice.org>
// All Rights Reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA
//

#include <qsound.h>
#include <stdlib.h>
#include <string.h>

int SOUND_OpenAudio(int freq, int channels, int samples)
{
   return 0;
}

int SOUND_CloseAudio()
{
   return 0;
}

void *SOUND_LoadWAV(const char *filename)
{
   return strdup(filename);
}

void SOUND_FreeWAV(void *audio)
{
   free(audio);
}

void SOUND_PlayWAV(void *audio)
{
   QSound::play((char *)audio);
}
