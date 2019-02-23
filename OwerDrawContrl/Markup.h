// Markup.h: interface for the CMarkup class.
//
// Markup Release 8.1
// Copyright (C) 1999-2005 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#if !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
#define AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4996) // suppress VS 2005 deprecated function warnings
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#define _DS(i) (i?&((LPCTSTR)m_strDoc)[m_aPos[i].nStart]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif

EXPORT_CLASS CMarkup
{
public:
	CMarkup(void) { SetDoc( NULL ); InitDocFlags(); };
	CMarkup( LPCTSTR szDoc ) { SetDoc( szDoc ); InitDocFlags(); };
	CMarkup( int nFlags ) { SetDoc( NULL ); m_nFlags = nFlags; };
	CMarkup( const CMarkup& markup ) { *this = markup; };
	void operator=( const CMarkup& markup );
	~CMarkup(void) {};

	// Navigate
	bool Load( LPCTSTR szFileName );
	bool SetDoc( LPCTSTR szDoc );
	bool IsWellFormed(void);
	bool FindElem( LPCTSTR szName=NULL );
	bool FindChildElem( LPCTSTR szName=NULL );
	bool IntoElem(void);
	bool OutOfElem(void);
	void ResetChildPos(void) { x_SetPos(m_iPosParent,m_iPos,0); };
	void ResetMainPos(void) { x_SetPos(m_iPosParent,0,0); };
	void ResetPos(void) { x_SetPos(0,0,0); };
	CString GetTagName(void) const;
	CString GetChildTagName(void) const { return x_GetTagName(m_iPosChild); };
	CString GetData(void) const { return x_GetData(m_iPos); };
	CString GetChildData(void) const { return x_GetData(m_iPosChild); };
	CString GetElemContent(void) const { return x_GetElemContent(m_iPos); };
	CString GetAttrib( LPCTSTR szAttrib ) const { return x_GetAttrib(m_iPos,szAttrib); };
	CString GetChildAttrib( LPCTSTR szAttrib ) const { return x_GetAttrib(m_iPosChild,szAttrib); };
	CString GetAttribName( int n ) const;
	int FindNode( int nType=0 );
	int GetNodeType(void) { return m_nNodeType; };
	bool SavePos( LPCTSTR szPosName=_T("") );
	bool RestorePos( LPCTSTR szPosName=_T("") );
	const CString& GetError(void) const { return m_strError; };
	int GetDocFlags(void) const { return m_nFlags; };
	void SetDocFlags( int nFlags ) { m_nFlags = nFlags; };
	enum MarkupDocFlags
	{
		MDF_IGNORECASE = 8,
	};
	enum MarkupNodeFlags
	{
		MNF_WITHCDATA      = 0x01,
		MNF_WITHNOLINES    = 0x02,
		MNF_WITHXHTMLSPACE = 0x04,
		MNF_WITHREFS       = 0x08,
		MNF_WITHNOEND      = 0x10,
		MNF_ESCAPEQUOTES  = 0x100,
		MNF_NONENDED   = 0x100000,
		MNF_ILLDATA    = 0x200000,
	};
	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01
		MNT_TEXT					= 2,  // 0x02
		MNT_WHITESPACE				= 4,  // 0x04
		MNT_CDATA_SECTION			= 8,  // 0x08
		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
		MNT_COMMENT					= 32, // 0x20
		MNT_DOCUMENT_TYPE			= 64, // 0x40
		MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
		MNT_LONE_END_TAG			= 128,// 0x80
		MNT_NODE_ERROR              = 32768 // 0x8000
	};

	// Create
	bool Save( LPCTSTR szFileName );
	const CString& GetDoc() const { return m_strDoc; };
	bool AddElem( LPCTSTR szName, LPCTSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags); };
	bool InsertElem( LPCTSTR szName, LPCTSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags|MNF_INSERT); };
	bool AddChildElem( LPCTSTR szName, LPCTSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags|MNF_CHILD); };
	bool InsertChildElem( LPCTSTR szName, LPCTSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags|MNF_INSERT|MNF_CHILD); };
	bool AddElem( LPCTSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags); };
	bool InsertElem( LPCTSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags|MNF_INSERT); };
	bool AddChildElem( LPCTSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags|MNF_CHILD); };
	bool InsertChildElem( LPCTSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags|MNF_INSERT|MNF_CHILD); };
	bool AddAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool AddChildAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
	bool AddAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
	bool AddChildAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
	bool AddSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,0); };
	bool InsertSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,MNF_INSERT); };
	CString GetSubDoc() const { return x_GetSubDoc(m_iPos); };
	bool AddChildSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,MNF_CHILD); };
	bool InsertChildSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,MNF_CHILD|MNF_INSERT); };
	CString GetChildSubDoc() const { return x_GetSubDoc(m_iPosChild); };
	bool AddNode( int nType, LPCTSTR szText ) { return x_AddNode(nType,szText,0); };
	bool InsertNode( int nType, LPCTSTR szText ) { return x_AddNode(nType,szText,MNF_INSERT); };

	// Modify
	bool RemoveElem(void);
	bool RemoveChildElem(void);
	bool RemoveNode(void);
	bool SetAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool SetChildAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
	bool SetAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
	bool SetChildAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
	bool SetData( LPCTSTR szData, int nFlags=0 ) { return x_SetData(m_iPos,szData,nFlags); };
	bool SetChildData( LPCTSTR szData, int nFlags=0 ) { return x_SetData(m_iPosChild,szData,nFlags); };
	bool SetData( int nValue ) { return x_SetData(m_iPos,nValue); };
	bool SetChildData( int nValue ) { return x_SetData(m_iPosChild,nValue); };
	bool SetElemContent( LPCTSTR szContent ) { return x_SetElemContent(szContent); };

	// Utility
	static bool ReadTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError=NULL, int* pnFlags=NULL );
	static bool WriteTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError=NULL, int* pnFlags=NULL );
	static CString EscapeText( LPCTSTR szText, int nFlags = 0 );
	static CString UnescapeText( LPCTSTR szText, int nTextLength = -1 );

protected:

#ifdef _DEBUG
	LPCTSTR m_pMainDS;
	LPCTSTR m_pChildDS;
#endif

	CString m_strDoc;
	CString m_strError;

	int m_iPosParent;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;
	int m_iPosDeleted;
	int m_nNodeType;
	int m_nNodeOffset;
	int m_nNodeLength;
	int m_nFlags;

	struct ElemPos
	{
		ElemPos(void) {};
		ElemPos( const ElemPos& pos ) { *this = pos; };
		enum { EP_STBITS=22, EP_STMASK=0x2fffff, EP_LEVMASK=0xffff };
		int StartTagLen(void) const { return (nTagLengths & EP_STMASK); };
		void SetStartTagLen( int n ) { nTagLengths = (nTagLengths & ~EP_STMASK) + n; };
		void AdjustStartTagLen( int n ) { nTagLengths += n; };
		int EndTagLen(void) const { return (nTagLengths >> EP_STBITS); };
		void SetEndTagLen( int n ) { nTagLengths = (nTagLengths & EP_STMASK) + (n << EP_STBITS); };
		bool IsEmptyElement(void) { return (StartTagLen()==nLength)?true:false; };
		int StartContent(void) const { return nStart + StartTagLen(); };
		int ContentLen(void) const { return nLength - StartTagLen() - EndTagLen(); };
		int StartAfter(void) const { return nStart + nLength; };
		int Level(void) const { return nFlags & EP_LEVMASK; };
		void SetLevel( int nLev ) { nFlags = (nFlags & ~EP_LEVMASK) | nLev; };
		void ClearVirtualParent(void) { memset(this,0,sizeof(ElemPos)); };

		// Memory size: 8 32-bit integers == 32 bytes
		int nStart;
		int nLength;
		int nTagLengths; // 22 bits 4MB limit for start tag, 10 bits 1K limit for end tag
		int nFlags; // 16 bits flags, 16 bits level 65536 depth limit
		int iElemParent;
		int iElemChild; // first child
		int iElemNext;
		int iElemPrev; // if this is first child, iElemPrev points to last
	};

	enum MarkupNodeFlagsInternal
	{
		MNF_REPLACE    = 0x001000,
		MNF_INSERT     = 0x002000,
		MNF_CHILD      = 0x004000,
		MNF_QUOTED     = 0x008000,
		MNF_EMPTY      = 0x010000,
		MNF_DELETED    = 0x020000,
		MNF_FIRST      = 0x080000,
		MNF_PUBLIC     = 0x300000,
		MNF_ILLFORMED  = 0x800000,
	};

	struct NodePos
	{
		NodePos(void) {};
		NodePos( int n ) { nFlags=n; nNodeType=0; nStart=0; nLength=0; };
		int nNodeType;
		int nStart;
		int nLength;
		int nFlags;
		CString strMeta;
	};

	struct TokenPos
	{
		TokenPos( LPCTSTR sz, int n ) { Clear(); szDoc=sz; nTokenFlags=n; };
		void Clear(void) { nL=0; nR=-1; nNext=0; };
		int Length(void) const { return nR - nL + 1; };
		bool Match( LPCTSTR szName )
		{
			int nLen = nR - nL + 1;
			if ( nTokenFlags & MDF_IGNORECASE )
				return ( (_tcsncicmp( &szDoc[nL], szName, nLen ) == 0)
					&& ( szName[nLen] == _T('\0') || _tcschr(_T(" =/[]"),szName[nLen]) ) );
			else
				return ( (_tcsnccmp( &szDoc[nL], szName, nLen ) == 0)
					&& ( szName[nLen] == _T('\0') || _tcschr(_T(" =/[]"),szName[nLen]) ) );
		};
		int nL;
		int nR;
		int nNext;
		LPCTSTR szDoc;
		int nTokenFlags;
		int nPreSpaceStart;
		int nPreSpaceLength;
	};

	struct SavedPos
	{
		SavedPos(void) { nSavedPosFlags=0; iPos=0; };
		CString strName;
		int iPos;
		int nSavedPosFlags;
	};

	struct SavedPosMap
	{
		SavedPosMap(void) { pTable = NULL; };
		~SavedPosMap(void) { RemoveAll(); };
		void RemoveAll(void) { if (pTable) Release(); pTable=NULL; };
		enum { SPM_SIZE = 7, SPM_MAIN = 1, SPM_CHILD = 2, SPM_USED = 4, SPM_LAST = 8 };
		void Release(void) { for (int n=0;n<SPM_SIZE;++n) if (pTable[n]) delete[] pTable[n]; delete[] pTable; };
		void AllocMapTable(void) { pTable = new SavedPos*[SPM_SIZE]; for (int n=0; n<SPM_SIZE; ++n) pTable[n]=NULL; };
		int Hash( LPCTSTR szName ) { int n=0; while (*szName) n += *szName++; return n % SPM_SIZE; };
		SavedPos** pTable;
	};
	SavedPosMap m_mapSavedPos;

	struct PosArray
	{
		PosArray(void) { Clear(); };
		~PosArray(void) { Release(); };
		enum { PA_SEGBITS = 16, PA_SEGMASK = 0xffff };
		void RemoveAll(void) { Release(); Clear(); };
		void Release(void) { for (int n=0;n<SegsUsed();++n) delete[] (char*)pSegs[n]; if (pSegs) delete[] (char*)pSegs; };
		void Clear(void) { nSegs=0; nSize=0; pSegs=NULL; };
		int GetSize(void) const { return nSize; };
		int SegsUsed(void) const { return ((nSize-1)>>PA_SEGBITS) + 1; };
		ElemPos& operator[](int n) const { return pSegs[n>>PA_SEGBITS][n&PA_SEGMASK]; };
		ElemPos** pSegs;
		int nSize;
		int nSegs;
	};
	PosArray m_aPos;

	struct NodeStack
	{
		NodeStack(void) { nTop=-1; nSize=0; pN=NULL; };
		~NodeStack(void) { if (pN) delete [] pN; };
		NodePos& Top(void) { return pN[nTop]; };
		NodePos& At( int n ) { return pN[n]; };
		void Add(void) { ++nTop; if (nTop==nSize) Alloc(nSize*2+6); };
		void Remove(void) { --nTop; };
		int TopIndex(void) { return nTop; };
	protected:
		void Alloc( int nNewSize ) { NodePos* pNNew = new NodePos[nNewSize]; Copy(pNNew); nSize=nNewSize; };
		void Copy( NodePos* pNNew ) { for(int n=0;n<nSize;++n) pNNew[n]=pN[n]; if (pN) delete [] pN; pN=pNNew; };
		NodePos* pN;
		int nSize;
		int nTop;
	};

	void x_SetPos( int iPosParent, int iPos, int iPosChild )
	{
		m_iPosParent = iPosParent;
		m_iPos = iPos;
		m_iPosChild = iPosChild;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};
	int x_GetFreePos(void)
	{
		if ( m_iPosFree == m_aPos.GetSize() )
			x_AllocPosArray();
		return m_iPosFree++;
	};
	bool x_AllocPosArray( int nNewSize = 0 );

	void InitDocFlags(void)
	{
		// To always ignore case, define MARKUP_IGNORECASE
	#ifdef MARKUP_IGNORECASE
		m_nFlags = MDF_IGNORECASE;
	#else
		m_nFlags = 0;
	#endif
	};

	bool x_ParseDoc(void);
	int x_ParseElem( int iPos, TokenPos& token );
	static bool x_FindAny( LPCTSTR szDoc, int& nChar );
	static bool x_FindName( TokenPos& token );
	static CString x_GetToken( const TokenPos& token );
	int x_FindElem( int iPosParent, int iPos, LPCTSTR szPath ) const;
	CString x_GetPath( int iPos ) const;
	CString x_GetTagName( int iPos ) const;
	CString x_GetData( int iPos ) const;
	CString x_GetAttrib( int iPos, LPCTSTR szAttrib ) const;
	static CString x_EncodeCDATASection( LPCTSTR szData );
	bool x_AddElem( LPCTSTR szName, LPCTSTR szValue, int nFlags );
	bool x_AddElem( LPCTSTR szName, int nValue, int nFlags );
	CString x_GetSubDoc( int iPos ) const;
	bool x_AddSubDoc( LPCTSTR szSubDoc, int nFlags );
	static bool x_FindAttrib( TokenPos& token, LPCTSTR szAttrib, int n=0 );
	bool x_SetAttrib( int iPos, LPCTSTR szAttrib, LPCTSTR szValue );
	bool x_SetAttrib( int iPos, LPCTSTR szAttrib, int nValue );
	bool x_AddNode( int nNodeType, LPCTSTR szText, int nFlags );
	void x_RemoveNode( int iPosParent, int& iPos, int& nNodeType, int& nNodeOffset, int& nNodeLength );
	void x_AdjustForNode( int iPosParent, int iPos, int nShift );
	static bool x_CreateNode( CString& strNode, int nNodeType, LPCTSTR szText );
	int x_InsertNew( int iPosParent, int& iPosRel, NodePos& node );
	void x_LinkElem( int iPosParent, int iPosBefore, int iPos );
	int x_UnlinkElem( int iPos );
	int x_ReleaseSubDoc( int iPos );
	int x_ReleasePos( int iPos );
	void x_CheckSavedPos(void);
	static int x_ParseNode( TokenPos& token, NodePos& node );
	bool x_SetData( int iPos, LPCTSTR szData, int nFlags );
	bool x_SetData( int iPos, int nValue );
	int x_RemoveElem( int iPos );
	CString x_GetElemContent( int iPos ) const;
	bool x_SetElemContent( LPCTSTR szContent );
	void x_DocChange( int nLeft, int nReplace, const CString& strInsert );
	void x_Adjust( int iPos, int nShift, bool bAfterPos = false );
};

#endif // !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
