#pragma once

#include <Windows.h>

#include <afx.h> // for basic MFC definitions
#include <afxstr.h>

class CPegDoc;
class CPrim;

class CFilePeg: public CFile
{
public:
    static const WORD SECTION_HEADER = 0x0101;
    static const WORD SECTION_TABLES = 0x0102;
    static const WORD SECTION_BLOCKS = 0x0103;
    static const WORD SECTION_SEGMENTS = 0x0104;
    static const WORD SECTION_END = 0x01ff;

    static const WORD TABLE = 0x0200;
    static const WORD TABLE_VPORT = 0x0201;
    static const WORD TABLE_LTYPE = 0x0202;
    static const WORD TABLE_LAYER = 0x0203;
    static const WORD TABLE_END = 0x02ff;

public:
    CFilePeg() { };

    // Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
    CFilePeg(const CFilePeg&) = delete;
    CFilePeg& operator=(const CFilePeg&) = delete;

    virtual ~CFilePeg() { }

    void	Load(CPegDoc* document);
    void	ReadBlocksSection(CPegDoc* document);
    void	ReadEntitiesSection(CPegDoc* document);
    void	ReadHeaderSection(CPegDoc* document);
    void	ReadLayerTable(CPegDoc* document);
    void	ReadLnTypsTable(CPegDoc* document);
    void	ReadTablesSection(CPegDoc* document);
    void	ReadVPortTable(CPegDoc* document);
    void	Unload(CPegDoc* document);
    void	WriteBlocksSection(CPegDoc* document);
    void	WriteEntitiesSection(CPegDoc* document);
    void	WriteHeaderSection(CPegDoc* document);
    void	WriteLayerTable(CPegDoc* document);
    void	WritePenStyleTable(CPegDoc* document);
    void	WriteTablesSection(CPegDoc* document);
    void	WriteVPortTable(CPegDoc* document);
};

void FilePeg_ReadString(CFile& file, CString& str);
bool FilePeg_ReadPrim(CFile& file, CPrim*& primitive);
inline void FilePeg_ReadDouble(CFile& file, double& d) { file.Read(&d, sizeof(double)); }
inline void FilePeg_ReadShort(CFile& file, short& i) { file.Read(&i, sizeof(short)); }
inline void FilePeg_ReadUnsignedShort(CFile& file, unsigned short& i) { file.Read(&i, sizeof(unsigned short)); }
inline void FilePeg_ReadWord(CFile& file, WORD& w) { file.Read(&w, sizeof(WORD)); }
inline WORD FilePeg_ReadWord(CFile& file) { WORD w; file.Read(&w, sizeof(WORD)); return w; }
inline void FilePeg_WriteDouble(CFile& file, double d) { file.Write(&d, sizeof(double)); }
inline void FilePeg_WriteWord(CFile& file, WORD w) { file.Write(&w, sizeof(WORD)); }
void FilePeg_WriteString(CFile& file, const CString& str);
