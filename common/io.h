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

#ifndef _IO_H_
#define _IO_H_

#include "stringutils.h"
#include <stdio.h>

/* treat filenames as normal filenames, don't use asset search process */
void io_ignore_asset_paths(void);

/* locate an asset and return a FILE* */
FILE *io_fopen_asset(stringptr fn, const char *kind);

#endif

