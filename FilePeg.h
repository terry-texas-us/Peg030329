#pragma once

class CPegDoc;

class CFilePeg : public CFile
{
public:
	static const WORD SECTION_HEADER	= 0x0101;
	static const WORD SECTION_TABLES	= 0x0102;
	static const WORD SECTION_BLOCKS	= 0x0103;
	static const WORD SECTION_SEGMENTS 	= 0x0104;
	static const WORD SECTION_END		= 0x01ff;

	static const WORD TABLE				= 0x0200;
	static const WORD TABLE_VPORT		= 0x0201;
	static const WORD TABLE_LTYPE		= 0x0202;
	static const WORD TABLE_LAYER		= 0x0203;
	static const WORD TABLE_END			= 0x02ff;

public:
	CFilePeg() {};
	
	// Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
	CFilePeg(const CFilePeg&) = delete;
	CFilePeg& operator=(const CFilePeg&) = delete;

	virtual ~CFilePeg() {}

	void	Load(CPegDoc* pDoc);
	void	ReadBlocksSection(CPegDoc* pDoc);
	void	ReadEntitiesSection(CPegDoc* pDoc);
	void	ReadHeaderSection(CPegDoc* pDoc);
	void	ReadLayerTable(CPegDoc* pDoc);
	void	ReadLnTypsTable(CPegDoc* pDoc);
	void	ReadTablesSection(CPegDoc* pDoc);
	void	ReadVPortTable(CPegDoc* pDoc);
	void	Unload(CPegDoc* pDoc);
	void	WriteBlocksSection(CPegDoc* pDoc);
	void	WriteEntitiesSection(CPegDoc* pDoc);
	void	WriteHeaderSection(CPegDoc* pDoc);
	void	WriteLayerTable(CPegDoc* pDoc);
	void	WritePenStyleTable(CPegDoc* pDoc);
	void	WriteTablesSection(CPegDoc* pDoc);
	void	WriteVPortTable(CPegDoc* pDoc);
};

void FilePeg_ReadString(CFile& fl, CString& str);
bool FilePeg_ReadPrim(CFile& fl, CPrim*&);
inline void FilePeg_ReadDouble(CFile& fl, double& d) {fl.Read(&d, sizeof(double));}
inline void FilePeg_ReadShort(CFile& fl, short& i) {fl.Read(&i, sizeof(short));}
inline void FilePeg_ReadWord(CFile& fl, WORD& w) {fl.Read(&w, sizeof(WORD));}
inline WORD FilePeg_ReadWord(CFile& fl) {WORD w; fl.Read(&w, sizeof(WORD)); return w;}
inline void FilePeg_WriteDouble(CFile& fl, double d) {fl.Write(&d, sizeof(double));}
inline void FilePeg_WriteWord(CFile& fl, WORD w) {fl.Write(&w, sizeof(WORD));}
void FilePeg_WriteString(CFile& fl, const CString& str);
