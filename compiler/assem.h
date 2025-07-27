#ifndef ASSEM_H
#define ASSEM_H
#pragma once

#include "linker.h"
#include <istream>

class Assem {
public:
  std::istream &in;
  Module *mod;
  Assem(std::istream &in, Module *mod) : in(in), mod(mod) {}
  virtual void assemble() = 0;
};

#endif