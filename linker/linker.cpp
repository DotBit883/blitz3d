#include "linker.h"
#include "image_util.h"
#include "stdutil.h"
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>
#include <istream>
#include <map>
#include <sys/mman.h>

class BBModule : public Module {
public:
  BBModule();
  BBModule(std::istream &in);
  ~BBModule();

  void *link(Module *libs);
  bool createExe(const char *exe_file, const char *dll_file);

  int getPC();

  void emit(int byte);
  void emitw(int word);
  void emitd(int dword);
  void emitx(void *mem, int sz);
  bool addSymbol(const char *sym, int pc);
  bool addReloc(const char *dest_sym, int pc, bool pcrel);

  bool findSymbol(const char *sym, int *pc);

private:
  char *data;
  int data_sz, pc;
  bool linked;
  size_t linked_mem_size;

  std::map<std::string, int> symbols;
  std::map<int, std::string> rel_relocs, abs_relocs;

  bool findSym(const std::string &t, Module *libs, int *n) {
    if (findSymbol(t.c_str(), n))
      return true;
    if (libs->findSymbol(t.c_str(), n))
      return true;
    std::string err = "Symbol '" + t + "' not found";
    throw std::runtime_error("[Blitz Linker Error] " + err);
  }

  void ensure(int n) {
    if (pc + n <= data_sz)
      return;

    data_sz = data_sz / 2 + data_sz;
    if (data_sz < pc + n)
      data_sz = pc + n;

    char *old_data = data;

    data = d_new char[data_sz];
    memcpy(data, old_data, pc);
    delete[] old_data;
  }
};

BBModule::BBModule()
    : data(0), data_sz(0), pc(0), linked(false), linked_mem_size(0) {}

BBModule::~BBModule() {
  if (linked)
    munmap(data, linked_mem_size);
  else
    delete[] data;
  data = nullptr;
  linked_mem_size = 0;
}

void *BBModule::link(Module *libs) {
  if (linked)
    return data;

  int dest;
  std::map<int, std::string>::iterator it;

  char *p = reinterpret_cast<char *>(mmap(nullptr, pc,
                                          PROT_READ | PROT_WRITE | PROT_EXEC,
                                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

  linked_mem_size = pc;

  memcpy(p, data, pc);
  delete[] data;
  data = p;

  linked = true;

  for (it = rel_relocs.begin(); it != rel_relocs.end(); ++it) {
    if (!findSym(it->second, libs, &dest))
      return 0;
    int *p = reinterpret_cast<int *>(data + it->first);
    *p += (dest - reinterpret_cast<intptr_t>(p));
  }

  for (it = abs_relocs.begin(); it != abs_relocs.end(); ++it) {
    if (!findSym(it->second, libs, &dest))
      return 0;
    int *p = reinterpret_cast<int *>(data + it->first);
    *p += dest;
  }

  return data;
}

int BBModule::getPC() { return pc; }

void BBModule::emit(int byte) {
  ensure(1);
  data[pc++] = byte;
}

void BBModule::emitw(int word) {
  ensure(2);
  *(short *)(data + pc) = word;
  pc += 2;
}

void BBModule::emitd(int dword) {
  ensure(4);
  *(int *)(data + pc) = dword;
  pc += 4;
}

void BBModule::emitx(void *mem, int sz) {
  ensure(sz);
  memcpy(data + pc, mem, sz);
  pc += sz;
}

bool BBModule::addSymbol(const char *sym, int pc) {
  std::string t(sym);
  if (symbols.find(t) != symbols.end())
    return false;
  symbols[t] = pc;
  return true;
}

bool BBModule::addReloc(const char *dest_sym, int pc, bool pcrel) {
  std::map<int, std::string> &rel = pcrel ? rel_relocs : abs_relocs;
  if (rel.find(pc) != rel.end())
    return false;
  rel[pc] = std::string(dest_sym);
  return true;
}

bool BBModule::findSymbol(const char *sym, int *pc) {
  std::string t = std::string(sym);
  std::map<std::string, int>::iterator it = symbols.find(t);
  if (it == symbols.end())
    return false;
  *pc = it->second + reinterpret_cast<intptr_t>(data);
  return true;
}

int Linker::version() { return VERSION; }

bool Linker::canCreateExe() {
#ifdef DEMO
  return false;
#else
  return true;
#endif
}

Module *Linker::createModule() { return d_new BBModule(); }

void Linker::deleteModule(Module *mod) { delete mod; }

Linker *linkerGetLinker() {
  static Linker linker;
  return &linker;
}

bool BBModule::createExe(const char *exe_file, const char *dll_file) {
#ifdef DEMO
  return false;
#else
  // find proc address of bbWinMain
  void *hmod = dlopen(dll_file, RTLD_LAZY);
  if (!hmod)
    return false;
  void *proc = dlsym(hmod, "bbWinMain");
  int entry = reinterpret_cast<intptr_t>(proc);
  dlclose(hmod);
  if (!proc)
    return false;

  try {
    std::filesystem::copy(dll_file, exe_file,
                          std::filesystem::copy_options::overwrite_existing);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Error copying file: " << e.what() << std::endl;
    return false;
  }

  if (!openImage(exe_file))
    return false;

  makeExe(entry);

  // create module
  // code size: code...
  // num_syms:  name,val...
  // num_rels:  name,val...
  // num_abss:  name,val...
  //
  qstreambuf buf;
  std::iostream out(&buf);

  std::map<std::string, int>::iterator it;
  std::map<int, std::string>::iterator rit;

  // write the code
  int sz = pc;
  out.write((char *)&sz, 4);
  out.write(data, pc);

  // write symbols
  sz = symbols.size();
  out.write((char *)&sz, 4);
  for (it = symbols.begin(); it != symbols.end(); ++it) {
    std::string t = it->first + '\0';
    out.write(t.data(), t.size());
    sz = it->second;
    out.write((char *)&sz, 4);
  }

  // write relative relocs
  sz = rel_relocs.size();
  out.write((char *)&sz, 4);
  for (rit = rel_relocs.begin(); rit != rel_relocs.end(); ++rit) {
    std::string t = rit->second + '\0';
    out.write(t.data(), t.size());
    sz = rit->first;
    out.write((char *)&sz, 4);
  }

  // write absolute relocs
  sz = abs_relocs.size();
  out.write((char *)&sz, 4);
  for (rit = abs_relocs.begin(); rit != abs_relocs.end(); ++rit) {
    std::string t = rit->second + '\0';
    out.write(t.data(), t.size());
    sz = rit->first;
    out.write((char *)&sz, 4);
  }

  replaceRsrc(10, 1111, 1033, buf.data(), buf.size());

  closeImage();

  return true;
#endif
}
