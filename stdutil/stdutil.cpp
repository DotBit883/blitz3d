#include "stdutil.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <filesystem>

int atoi(const std::string &s) { return atoi(s.c_str()); }

double atof(const std::string &s) { return atof(s.c_str()); }

std::string itoa(int n) { return std::to_string(n); }

static int _finite(double n) { // definition: exponent anything but 2047.

  int e;                 // 11 bit exponent
  const int eMax = 2047; // 0x7ff, all bits = 1

  int *pn = (int *)&n;

  e = *++pn; // Intel order!
  e = (e >> 20) & eMax;

  return e != eMax;
}

static int _isnan(double n) { // definition: exponent 2047, nonzero fraction.

  int e;                 // 11 bit exponent
  const int eMax = 2047; // 0x7ff, all bits = 1

  int *pn = (int *)&n;

  e = *++pn; // Intel order!
  e = (e >> 20) & eMax;

  if (e != 2047)
    return 0; // almost always return here

  int fHi, fLo; // 52 bit fraction

  fHi = (*pn) & 0xfffff; // first 20 bits
  fLo = *--pn;           // last 32 bits

  return (fHi | fLo) != 0; // returns 0,1 not just 0,nonzero
}

std::string ftoa(float n) {
	return std::to_string(n);
}

std::string tolower(const std::string &s) {
  std::string t = s;
  for (int k = 0; k < t.size(); ++k)
    t[k] = tolower(t[k]);
  return t;
}

std::string toupper(const std::string &s) {
  std::string t = s;
  for (int k = 0; k < t.size(); ++k)
    t[k] = toupper(t[k]);
  return t;
}

std::filesystem::path fullfilename(const std::filesystem::path &t) {
	return std::filesystem::absolute(t);
}

// Returns the directory portion (parent path)
std::filesystem::path filenamepath(const std::filesystem::path &t) {
	return std::filesystem::absolute(t).parent_path();
}

// Returns the filename only (without the directory)
std::filesystem::path filenamefile(const std::filesystem::path &t) {
	return std::filesystem::absolute(t).filename();
}

const int MIN_SIZE = 256;

qstreambuf::qstreambuf() {
  buf = new char[MIN_SIZE];
  setg(buf, buf, buf);
  setp(buf, buf + MIN_SIZE);
}

qstreambuf::~qstreambuf() { delete buf; }

int qstreambuf::size() { return pptr() - gptr(); }

char *qstreambuf::data() { return gptr(); }

qstreambuf::int_type qstreambuf::underflow() {
  if (gptr() == egptr()) {
    if (gptr() == pptr())
      return traits_type::eof();
    setg(gptr(), gptr(), pptr());
  }

  return traits_type::to_int_type(*gptr());
}

qstreambuf::int_type qstreambuf::overflow(qstreambuf::int_type c) {
  if (c == traits_type::eof())
    return c;

  if (pptr() == epptr()) {
    int sz = size();
    int n_sz = sz * 2;
    if (n_sz < MIN_SIZE)
      n_sz = MIN_SIZE;
    char *n_buf = new char[n_sz];
    memcpy(n_buf, gptr(), sz);
    delete buf;
    buf = n_buf;
    setg(buf, buf, buf + sz);
    setp(buf + sz, buf + n_sz);
  }

  *pptr() = traits_type::to_char_type(c);
  pbump(1);
  return traits_type::not_eof(c);
}
