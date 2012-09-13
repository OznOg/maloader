// Copyright 2012 Sebastien Gonzalve. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of  conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above
//      copyright notice, this list of conditions and the following
//      disclaimer in the documentation and/or other materials
//      provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY Sebastien Gonzalve ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Sebastien Gonzalve OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

/*
 * Unfortunatly, the linux and mac passwd structure does not match
 * thus it is made necessary to have a conversion from one version
 * to the other.
 */
struct __darwin_passwd {
  char    *pw_name;       /* user name */
  char    *pw_passwd;     /* encrypted password */
  uid_t   pw_uid;         /* user uid */
  gid_t   pw_gid;         /* user gid */
  time_t  pw_change;      /* password change time */
  char    *pw_class;      /* user access class */
  char    *pw_gecos;      /* Honeywell login info */
  char    *pw_dir;        /* home directory */
  char    *pw_shell;      /* default shell */
  time_t  pw_expire;      /* account expiration */
  int     pw_fields;      /* internal: fields filled in */
};

/*
 * Return a correctly formed passwd structure taking care to
 * make fields point to the correct place.
 * FIXME having a correct getpwuid_r() implementation
 *       would be really better.
 */
struct __darwin_passwd *__darwin_getpwuid(uid_t uid)
{
  static struct __darwin_passwd m_darwin_passwd;
  struct passwd *m_passwd = getpwuid(uid);
  assert(m_passwd != NULL);

  m_darwin_passwd.pw_name = m_passwd->pw_name;
  m_darwin_passwd.pw_passwd = m_passwd->pw_passwd;
  m_darwin_passwd.pw_uid = m_passwd->pw_uid;
  m_darwin_passwd.pw_gid = m_passwd->pw_gid;
  m_darwin_passwd.pw_change = 0;
  m_darwin_passwd.pw_class = NULL;
  m_darwin_passwd.pw_gecos = m_passwd->pw_gecos;
  m_darwin_passwd.pw_dir = m_passwd->pw_dir;
  m_darwin_passwd.pw_shell = m_passwd->pw_shell;
  m_darwin_passwd.pw_expire = 0;
  m_darwin_passwd.pw_fields = 0;

  return &m_darwin_passwd;
}

