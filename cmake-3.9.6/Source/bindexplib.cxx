/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
/*-------------------------------------------------------------------------
  Portions of this source have been derived from the 'bindexplib' tool
  provided by the CERN ROOT Data Analysis Framework project (root.cern.ch).
  Permission has been granted by Pere Mato <pere.mato@cern.ch> to distribute
  this derived work under the CMake license.
-------------------------------------------------------------------------*/

/*
*----------------------------------------------------------------------
* Program:  dumpexts.exe
* Author:   Gordon Chaffee
*
* History:  The real functionality of this file was written by
*           Matt Pietrek in 1993 in his pedump utility.  I've
*           modified it to dump the externals in a bunch of object
*           files to create a .def file.
*
* Notes:    Visual C++ puts an underscore before each exported symbol.
*           This file removes them.  I don't know if this is a problem
*           this other compilers.  If _MSC_VER is defined,
*           the underscore is removed.  If not, it isn't.  To get a
*           full dump of an object file, use the -f option.  This can
*           help determine the something that may be different with a
*           compiler other than Visual C++.
*   ======================================
* Corrections (Axel 2006-04-04):
*   Conversion to C++. Mostly.
*
 * Extension (Axel 2006-03-15)
 *    As soon as an object file contains an /EXPORT directive (which
 *    is generated by the compiler when a symbol is declared as
 *    declspec(dllexport)) no to-be-exported symbols are printed,
 *    as the linker will see these directives, and if those directives
 *    are present we only export selectively (i.e. we trust the
 *    programmer).
 *
 *   ======================================
*   ======================================
* Corrections (Valery Fine 23/02/98):
*
*           The "(vector) deleting destructor" MUST not be exported
*           To recognize it the following test are introduced:
*  "@@UAEPAXI@Z"  scalar deleting dtor
*  "@@QAEPAXI@Z"  vector deleting dtor
*  "AEPAXI@Z"     vector deleting dtor with thunk adjustor
*   ======================================
* Corrections (Valery Fine 12/02/97):
*
*    It created a wrong EXPORTS for the global pointers and constants.
*    The Section Header has been involved to discover the missing information
*    Now the pointers are correctly supplied  supplied with "DATA" descriptor
*        the constants  with no extra descriptor.
*
* Corrections (Valery Fine 16/09/96):
*
*     It didn't work for C++ code with global variables and class definitons
*     The DumpExternalObject function has been introduced to generate .DEF file
*
* Author:   Valery Fine 16/09/96  (E-mail: fine@vxcern.cern.ch)
*----------------------------------------------------------------------
*/
#include "bindexplib.h"

#include "cmsys/Encoding.hxx"
#include "cmsys/FStream.hxx"
#include <iostream>
#include <windows.h>

#ifndef IMAGE_FILE_MACHINE_ARMNT
#define IMAGE_FILE_MACHINE_ARMNT 0x01c4
#endif

typedef struct cmANON_OBJECT_HEADER_BIGOBJ
{
  /* same as ANON_OBJECT_HEADER_V2 */
  WORD Sig1;    // Must be IMAGE_FILE_MACHINE_UNKNOWN
  WORD Sig2;    // Must be 0xffff
  WORD Version; // >= 2 (implies the Flags field is present)
  WORD Machine; // Actual machine - IMAGE_FILE_MACHINE_xxx
  DWORD TimeDateStamp;
  CLSID ClassID;        // {D1BAA1C7-BAEE-4ba9-AF20-FAF66AA4DCB8}
  DWORD SizeOfData;     // Size of data that follows the header
  DWORD Flags;          // 0x1 -> contains metadata
  DWORD MetaDataSize;   // Size of CLR metadata
  DWORD MetaDataOffset; // Offset of CLR metadata

  /* bigobj specifics */
  DWORD NumberOfSections; // extended from WORD
  DWORD PointerToSymbolTable;
  DWORD NumberOfSymbols;
} cmANON_OBJECT_HEADER_BIGOBJ;

typedef struct _cmIMAGE_SYMBOL_EX
{
  union
  {
    BYTE ShortName[8];
    struct
    {
      DWORD Short; // if 0, use LongName
      DWORD Long;  // offset into string table
    } Name;
    DWORD LongName[2]; // PBYTE  [2]
  } N;
  DWORD Value;
  LONG SectionNumber;
  WORD Type;
  BYTE StorageClass;
  BYTE NumberOfAuxSymbols;
} cmIMAGE_SYMBOL_EX;
typedef cmIMAGE_SYMBOL_EX UNALIGNED* cmPIMAGE_SYMBOL_EX;

PIMAGE_SECTION_HEADER GetSectionHeaderOffset(
  PIMAGE_FILE_HEADER pImageFileHeader)
{
  return (PIMAGE_SECTION_HEADER)((DWORD_PTR)pImageFileHeader +
                                 IMAGE_SIZEOF_FILE_HEADER +
                                 pImageFileHeader->SizeOfOptionalHeader);
}

PIMAGE_SECTION_HEADER GetSectionHeaderOffset(
  cmANON_OBJECT_HEADER_BIGOBJ* pImageFileHeader)
{
  return (PIMAGE_SECTION_HEADER)((DWORD_PTR)pImageFileHeader +
                                 sizeof(cmANON_OBJECT_HEADER_BIGOBJ));
}

/*
+ * Utility func, strstr with size
+ */
const char* StrNStr(const char* start, const char* find, size_t& size)
{
  size_t len;
  const char* hint;

  if (!start || !find || !size) {
    size = 0;
    return 0;
  }
  len = strlen(find);

  while ((hint = (const char*)memchr(start, find[0], size - len + 1))) {
    size -= (hint - start);
    if (!strncmp(hint, find, len))
      return hint;
    start = hint + 1;
  }

  size = 0;
  return 0;
}

template <
  // cmANON_OBJECT_HEADER_BIGOBJ or IMAGE_FILE_HEADER
  class ObjectHeaderType,
  // cmPIMAGE_SYMBOL_EX or PIMAGE_SYMBOL
  class SymbolTableType>
class DumpSymbols
{
public:
  /*
   *----------------------------------------------------------------------
   * Constructor --
   *
   *     Initialize variables from pointer to object header.
   *
   *----------------------------------------------------------------------
   */

  DumpSymbols(ObjectHeaderType* ih, std::set<std::string>& symbols,
              std::set<std::string>& dataSymbols, bool isI386)
    : Symbols(symbols)
    , DataSymbols(dataSymbols)
  {
    this->ObjectImageHeader = ih;
    this->SymbolTable =
      (SymbolTableType*)((DWORD_PTR) this->ObjectImageHeader +
                         this->ObjectImageHeader->PointerToSymbolTable);
    this->SectionHeaders = GetSectionHeaderOffset(this->ObjectImageHeader);
    this->SymbolCount = this->ObjectImageHeader->NumberOfSymbols;
    this->IsI386 = isI386;
  }

  /*
   *----------------------------------------------------------------------
   * DumpObjFile --
   *
   *      Dump an object file's exported symbols.
   *----------------------------------------------------------------------
   */
  void DumpObjFile() { this->DumpExternalsObjects(); }

  /*
   *----------------------------------------------------------------------
   * DumpExternalsObjects --
   *
   *      Dumps a COFF symbol table from an OBJ.
   *----------------------------------------------------------------------
   */
  void DumpExternalsObjects()
  {
    unsigned i;
    PSTR stringTable;
    std::string symbol;
    DWORD SectChar;
    /*
     * The string table apparently starts right after the symbol table
     */
    stringTable = (PSTR) & this->SymbolTable[this->SymbolCount];
    SymbolTableType* pSymbolTable = this->SymbolTable;
    for (i = 0; i < this->SymbolCount; i++) {
      if (pSymbolTable->SectionNumber > 0 &&
          (pSymbolTable->Type == 0x20 || pSymbolTable->Type == 0x0)) {
        if (pSymbolTable->StorageClass == IMAGE_SYM_CLASS_EXTERNAL) {
          /*
          *    The name of the Function entry points
          */
          if (pSymbolTable->N.Name.Short != 0) {
            symbol = "";
            symbol.insert(0, (const char*)pSymbolTable->N.ShortName, 8);
          } else {
            symbol = stringTable + pSymbolTable->N.Name.Long;
          }

          // clear out any leading spaces
          while (isspace(symbol[0]))
            symbol.erase(0, 1);
          // if it starts with _ and has an @ then it is a __cdecl
          // so remove the @ stuff for the export
          if (symbol[0] == '_') {
            std::string::size_type posAt = symbol.find('@');
            if (posAt != std::string::npos) {
              symbol.erase(posAt);
            }
          }
          // For i386 builds we need to remove _
          if (this->IsI386 && symbol[0] == '_') {
            symbol.erase(0, 1);
          }

          // Check whether it is "Scalar deleting destructor" and "Vector
          // deleting destructor"
          // if scalarPrefix and vectorPrefix are not found then print
          // the symbol
          const char* scalarPrefix = "??_G";
          const char* vectorPrefix = "??_E";
          // The original code had a check for
          //     symbol.find("real@") == std::string::npos)
          // but this disallows member functions with the name "real".
          if (symbol.compare(0, 4, scalarPrefix) &&
              symbol.compare(0, 4, vectorPrefix)) {
            SectChar = this->SectionHeaders[pSymbolTable->SectionNumber - 1]
                         .Characteristics;
            // skip symbols containing a dot
            if (symbol.find('.') == std::string::npos) {
              if (!pSymbolTable->Type && (SectChar & IMAGE_SCN_MEM_WRITE)) {
                // Read only (i.e. constants) must be excluded
                this->DataSymbols.insert(symbol);
              } else {
                if (pSymbolTable->Type || !(SectChar & IMAGE_SCN_MEM_READ) ||
                    (SectChar & IMAGE_SCN_MEM_EXECUTE)) {
                  this->Symbols.insert(symbol);
                }
              }
            }
          }
        }
      }

      /*
      * Take into account any aux symbols
      */
      i += pSymbolTable->NumberOfAuxSymbols;
      pSymbolTable += pSymbolTable->NumberOfAuxSymbols;
      pSymbolTable++;
    }
  }

private:
  std::set<std::string>& Symbols;
  std::set<std::string>& DataSymbols;
  DWORD_PTR SymbolCount;
  PIMAGE_SECTION_HEADER SectionHeaders;
  ObjectHeaderType* ObjectImageHeader;
  SymbolTableType* SymbolTable;
  bool IsI386;
};

bool DumpFile(const char* filename, std::set<std::string>& symbols,
              std::set<std::string>& dataSymbols)
{
  HANDLE hFile;
  HANDLE hFileMapping;
  LPVOID lpFileBase;
  PIMAGE_DOS_HEADER dosHeader;

  hFile = CreateFileW(cmsys::Encoding::ToWide(filename).c_str(), GENERIC_READ,
                      FILE_SHARE_READ, NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, 0);

  if (hFile == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Couldn't open file '%s' with CreateFile()\n", filename);
    return false;
  }

  hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (hFileMapping == 0) {
    CloseHandle(hFile);
    fprintf(stderr, "Couldn't open file mapping with CreateFileMapping()\n");
    return false;
  }

  lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
  if (lpFileBase == 0) {
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    fprintf(stderr, "Couldn't map view of file with MapViewOfFile()\n");
    return false;
  }

  dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
  if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
    fprintf(stderr, "File is an executable.  I don't dump those.\n");
    return false;
  }
  /* Does it look like a COFF OBJ file??? */
  else if (((dosHeader->e_magic == IMAGE_FILE_MACHINE_I386) ||
            (dosHeader->e_magic == IMAGE_FILE_MACHINE_AMD64) ||
            (dosHeader->e_magic == IMAGE_FILE_MACHINE_ARMNT)) &&
           (dosHeader->e_sp == 0)) {
    /*
    * The two tests above aren't what they look like.  They're
    * really checking for IMAGE_FILE_HEADER.Machine == i386 (0x14C)
    * and IMAGE_FILE_HEADER.SizeOfOptionalHeader == 0;
    */
    DumpSymbols<IMAGE_FILE_HEADER, IMAGE_SYMBOL> symbolDumper(
      (PIMAGE_FILE_HEADER)lpFileBase, symbols, dataSymbols,
      (dosHeader->e_magic == IMAGE_FILE_MACHINE_I386));
    symbolDumper.DumpObjFile();
  } else {
    // check for /bigobj format
    cmANON_OBJECT_HEADER_BIGOBJ* h = (cmANON_OBJECT_HEADER_BIGOBJ*)lpFileBase;
    if (h->Sig1 == 0x0 && h->Sig2 == 0xffff) {
      DumpSymbols<cmANON_OBJECT_HEADER_BIGOBJ, cmIMAGE_SYMBOL_EX> symbolDumper(
        (cmANON_OBJECT_HEADER_BIGOBJ*)lpFileBase, symbols, dataSymbols,
        (h->Machine == IMAGE_FILE_MACHINE_I386));
      symbolDumper.DumpObjFile();
    } else {
      printf("unrecognized file format in '%s'\n", filename);
      return false;
    }
  }
  UnmapViewOfFile(lpFileBase);
  CloseHandle(hFileMapping);
  CloseHandle(hFile);
  return true;
}

bool bindexplib::AddObjectFile(const char* filename)
{
  return DumpFile(filename, this->Symbols, this->DataSymbols);
}

bool bindexplib::AddDefinitionFile(const char* filename)
{
  cmsys::ifstream infile(filename);
  if (!infile) {
    fprintf(stderr, "Couldn't open definition file '%s'\n", filename);
    return false;
  }
  std::string str;
  while (std::getline(infile, str)) {
    // skip the LIBRAY and EXPORTS lines (if any)
    if ((str.compare(0, 7, "LIBRARY") == 0) ||
        (str.compare(0, 7, "EXPORTS") == 0)) {
      continue;
    }
    // remove leading tabs & spaces
    str.erase(0, str.find_first_not_of(" \t"));
    std::size_t found = str.find(" \t DATA");
    if (found != std::string::npos) {
      str.erase(found, std::string::npos);
      this->DataSymbols.insert(str);
    } else {
      this->Symbols.insert(str);
    }
  }
  infile.close();
  return true;
}

void bindexplib::WriteFile(FILE* file)
{
  fprintf(file, "EXPORTS \n");
  for (std::set<std::string>::const_iterator i = this->DataSymbols.begin();
       i != this->DataSymbols.end(); ++i) {
    fprintf(file, "\t%s \t DATA\n", i->c_str());
  }
  for (std::set<std::string>::const_iterator i = this->Symbols.begin();
       i != this->Symbols.end(); ++i) {
    fprintf(file, "\t%s\n", i->c_str());
  }
}
