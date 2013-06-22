/* Copyright 2013 Brian Swetland <swetland@frotz.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _TEXTUREFONT_H_
#define _TEXTUREFONT_H_

struct CharInfo {
	// location in texture
	u16 x;
	u16 y;
	u16 w;
	u16 h;
	// adjustment relative to 0,0 baseline
	s16 dx;
	s16 dy;
	// adjustment to the next character;
	u16 advance;
	u16 unused;
};

struct FontInfo {
	u32 magic;
	u32 first;
	u32 count;
	u32 unused;
	CharInfo info[0];
};

#define TEXTUREFONT_MAGIC 0x746E6F46

#endif
