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

#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <string.h>

class stringptr {
public:
	stringptr() : str(""), len(0) {}
	stringptr(const char *s) : str(s), len(strlen(s)) {}
	stringptr(const char *s, int _len) : str(s), len(_len) {}
	int length() { return len; }
	const char *cstr(void) { return str; }
	operator const char *() { return str; }
private:
	const char *str;
	int len;
};

template <int N>
class stackstring {
public:
	stackstring() {
		len = 0;
		oops = 0;
		str[0] = 0;
	}
	stackstring(stringptr sp) {
		int l2 = sp.length();
		if (l2 >= N) {
			len = N - 1;
			oops = 1;
		} else {
			len = l2;
			oops = 0;
		}
		memcpy(str, sp.cstr(), len + 1);
	}
	stackstring<N>& trim(int n) {
		if (len > n) {
			len = n;
			str[len] = 0;
		}
		return *this;
	}
	stackstring<N>& append(const char *astr, int alen) {
		if ((N - len - 1) >= alen) {
			memcpy(str + len, astr, alen + 1);
			len += alen;
		} else {
			memcpy(str + len, astr, N - len - 1);
			len = N - 1;
			oops = 1;
		}
		return *this;
	}
	stackstring<N>& append(stringptr sp) {
		return append(sp.cstr(), sp.length());
	}
	stackstring<N>& append(char c) {
		return append(&c, 1);
	}
	int length(void) { return len; }
	int error(void) { return oops; }
	const char *cstr(void) { return str; }
	operator const char *() { return str; }
	operator stringptr () { return stringptr(str, len); }
private:
	int len;
	int oops;
	char str[N];
};

typedef stackstring<1024> string1024;
typedef stackstring<128> string128;

#endif
