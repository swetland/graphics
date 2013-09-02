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

#ifndef _SHARED_H_
#define _SHARED_H_

/* Constants that are shared with shader programs. */
/* Ensure that the textual version below stays in sync. */

#define A_POSITION	0
#define A_NORMAL	1
#define A_TEXCOORD	2

#if INCLUDE_SHADER_GLOBALS
static const char *shader_globals = 
"#define A_POSITION 0\n"
"#define A_NORMAL 1\n"
"#define A_TEXCOORD 2\n"
;
#endif

#endif
