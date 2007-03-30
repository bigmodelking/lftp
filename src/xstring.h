/*
 * lftp and utils
 *
 * Copyright (c) 1996-2007 by Alexander V. Lukyanov (lav@yars.free.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef XSTRING_H
#define XSTRING_H

/* Declare string and memory handling routines.  Take care that an ANSI
   string.h and pre-ANSI memory.h might conflict, and that memory.h and
   strings.h conflict on some systems.  */

#if STDC_HEADERS || HAVE_STRING_H
# include <string.h>
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
#else
# include <strings.h>
# ifndef strchr
#  define strchr index
# endif
# ifndef strrchr
#  define strrchr rindex
# endif
# ifndef memcpy
#  define memcpy(Dst, Src, Num) bcopy (Src, Dst, Num)
# endif
# ifndef memcmp
#  define memcmp(Src1, Src2, Num) bcmp (Src1, Src2, Num)
# endif
#endif

#include <stdarg.h>

#if !HAVE_DECL_STRCASECMP
CDECL int strcasecmp(const char *s1,const char *s2);
CDECL int strncasecmp(const char *s1,const char *s2,size_t n);
#endif

#if !HAVE_DECL_VSNPRINTF
CDECL int vsnprintf(char *,size_t,const char *,va_list);
#endif
#if !HAVE_DECL_SNPRINTF
CDECL int snprintf(char *,size_t,const char *,...);
#endif

static inline int xstrcmp(const char *s1,const char *s2)
{
   if(s1==s2)
      return 0;
   if(s1==0 || s2==0)
      return 1;
   return strcmp(s1,s2);
}
static inline int xstrncmp(const char *s1,const char *s2,size_t len)
{
   if(s1==s2 || len==0)
      return 0;
   if(s1==0 || s2==0)
      return 1;
   return strncmp(s1,s2,len);
}
static inline int xstrcasecmp(const char *s1,const char *s2)
{
   if(s1==s2)
      return 0;
   if(s1==0 || s2==0)
      return 1;
   return strcasecmp(s1,s2);
}
static inline size_t xstrlen(const char *s)
{
   if(s==0)
      return 0;
   return strlen(s);
}

#include <stdarg.h>
#include "xmalloc.h"

/* this is a small and fast dynamic string class */
/* mostly used as xstrdup/xfree replacement */

class xstring0 // base class
{
protected:
   char *buf;
   xstring0() {}
public:
   ~xstring0() { xfree(buf); }
   operator const char *() const { return buf; }
   const char *get() const { return buf; }
   char *get_non_const() { return buf; }
};

// compact variant
class xstring_c : public xstring0
{
   // make xstring_c = xstrdup() fail:
   xstring_c& operator=(char *);
   const char *operator=(const char *s);
   const char *operator=(const xstring_c& s);

public:
   xstring_c() { buf=0; }
   xstring_c(const char *s) { buf=xstrdup(s); }
   const char *set(const char *s) { return xstrset(buf,s); }
   const char *set_allocated(char *s) { xfree(buf); return buf=s; }
   void truncate(size_t n) { buf[n]=0; }
};

// full implementation
class xstring : public xstring0
{
   size_t size;
   size_t len;

   void init() { buf=0; size=len=0; }
   void init(const char *s,int l);
   void init(const char *s);

   // make xstring = xstrdup() fail:
   xstring& operator=(char *);
   const char *operator=(const char *s) { return set(s); }
   const char *operator=(const xstring& s) { return set(s.get()); }

public:
   xstring() { init(); }
   xstring(const xstring &s) { init(s,s.length()); }
   xstring(const char *s) { init(s); }

   // allocates s bytes, with preferred granularity g
   void get_space(size_t s,size_t g=32);

   size_t length() const { return len; }

   const char *set(xstring &s) { return nset(s,s.length()); }
   const char *set(const xstring &s) { return nset(s,s.length()); }
   const char *set(const char *s);
   const char *nset(const char *s,int len);
   const char *set_allocated(char *s);
   const char *set_substr(int start,size_t sublen,const char *);

   const char *append(const char *s);
   static size_t vstrlen(va_list);
   const char *vappend(va_list);
   const char *vappend(...) __attribute__((sentinel));
   const char *vset(...) __attribute__((sentinel));

   void truncate(size_t n);
   void truncate_at(char c);
   /* set_length can be used to extend the string, e.g. after modification
      with get_space+get_non_const. */
   void set_length(size_t n) { buf[len=n]=0; }
};

#endif//XSTRING_H