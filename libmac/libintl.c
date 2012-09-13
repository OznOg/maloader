#include <locale.h>
#include <libintl.h>

char *__darwin_libintl_setlocale(int categorie, const char *locale)
{
  return setlocale(categorie, locale);
}

char *__darwin_libintl_bindtextdomain(const char *domainname, const char *dirname)
{
  return bindtextdomain(domainname, dirname);
}

char *__darwin_libintl_textdomain(const char *domainname)
{
  return textdomain(domainname);
}

