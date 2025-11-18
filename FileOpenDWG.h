#pragma once

#define AD_PROTOTYPES

class CAbstractView;
class CPenStyle;
class CPrim;

class CFileOpenDWG
{
public:
	CFileOpenDWG();
	~CFileOpenDWG();
	
	bool	Create(char*, WORD);
	bool	Initialize(short* iError);
	void	Load(char*);
#if ODA_FUNCTIONALITY
	void	ReadBlock(AD_DB_HANDLE, PAD_BLKH, PAD_ENT_HDR);
#endif
#if ODA_FUNCTIONALITY
	void	ReadBlocks(AD_DB_HANDLE);
#endif
#if ODA_FUNCTIONALITY
	void	ReadBlockTable(AD_DB_HANDLE);
#endif
	void	ReadEntities();
#if ODA_FUNCTIONALITY
	CPrim*	ReadEntity(AD_VMADDR);
#endif
#if ODA_FUNCTIONALITY
	CPrim*	ReadEntityPolyline(AD_VMADDR);
#endif
	void	ReadHeaderSection();
	void	ReadLayerTable();
	void	ReadLnTypsTable();
	void	ReadVPortTable();
	void	WriteBlocksSection();
	void	WriteEntities();
	void	WriteLayer(const CString& strName, PENCOLOR);
	void	WriteLayerTable();
	void	WritePenStyle(CPenStyle*);
	void	WritePenStyleTable();

#if ODA_FUNCTIONALITY
	// HACK: temp to allow layer object handle to be passed to primitive without changing arg lists
	static AD_OBJHANDLE	ms_objecthandle;
#endif

private:

#if ODA_FUNCTIONALITY
	PAD_DWGHDR		m_dhd;
#endif
#if ODA_FUNCTIONALITY
	AD_DB_HANDLE	m_hdb;
#endif
#if ODA_FUNCTIONALITY
	PAD_ENT_HDR		m_henhd;
#endif
#if ODA_FUNCTIONALITY
	PAD_ENT			m_hen;
#endif
#if ODA_FUNCTIONALITY
	PAD_TB			m_htb;
#endif
#if ODA_FUNCTIONALITY
	PAD_XD			m_hxd;
#endif
	bool			m_bInitialized;
	int				m_iVpCount;
	CViewport		m_vp[17];
	CAbstractView	m_av[17];
	CTMat			m_tm[17];
	CTMat			m_tmView[17];
};
