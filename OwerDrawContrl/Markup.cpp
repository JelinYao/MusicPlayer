// Markup.cpp: implementation of the CMarkup class.
//
// Markup Release 8.1
// Copyright (C) 1999-2005 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#include "stdafx.h"
#include "Markup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _MBCS
#pragma message( "Note: MBCS build (not UTF-8)" )
// For UTF-8, remove _MBCS from project settings C/C++ preprocessor definitions
#endif

// Defines for Windows CE
#ifdef _WIN32_WCE
#define _tclen(p) 1
#define _tccpy(p1,p2) *(p1)=*(p2)
#endif

// Customization
#define x_EOL _T("\r\n") // can be \r\n or \n or empty
#define x_EOLLEN (sizeof(x_EOL)/sizeof(_TCHAR)-1) // string length of x_EOL
#define x_ATTRIBQUOTE _T("\"") // can be double or single quote


void CMarkup::operator=( const CMarkup& markup )
{
	m_iPosParent = markup.m_iPosParent;
	m_iPos = markup.m_iPos;
	m_iPosChild = markup.m_iPosChild;
	m_iPosFree = markup.m_iPosFree;
	m_iPosDeleted = markup.m_iPosDeleted;
	m_nNodeType = markup.m_nNodeType;
	m_nNodeOffset = markup.m_nNodeOffset;
	m_nNodeLength = markup.m_nNodeLength;
	m_strDoc = markup.m_strDoc;
	m_strError = markup.m_strError;
	m_nFlags = markup.m_nFlags;

	// Copy used part of the index array
	m_aPos.RemoveAll();
	m_aPos.nSize = m_iPosFree;
	if ( m_aPos.nSize < 8 )
		m_aPos.nSize = 8;
	m_aPos.nSegs = m_aPos.SegsUsed();
	if ( m_aPos.nSegs )
	{
		m_aPos.pSegs = (ElemPos**)(new char[m_aPos.nSegs*sizeof(char*)]);
		int nSegSize = 1 << m_aPos.PA_SEGBITS;
		for ( int nSeg=0; nSeg < m_aPos.nSegs; ++nSeg )
		{
			if ( nSeg + 1 == m_aPos.nSegs )
				nSegSize = m_aPos.GetSize() - (nSeg << m_aPos.PA_SEGBITS);
			m_aPos.pSegs[nSeg] = (ElemPos*)(new char[nSegSize*sizeof(ElemPos)]);
			memcpy( m_aPos.pSegs[nSeg], markup.m_aPos.pSegs[nSeg], nSegSize*sizeof(ElemPos) );
		}
	}

	// Copy SavedPos map
	m_mapSavedPos.RemoveAll();
	if ( markup.m_mapSavedPos.pTable )
	{
		m_mapSavedPos.AllocMapTable();
		for ( int nSlot=0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			SavedPos* pCopySavedPos = markup.m_mapSavedPos.pTable[nSlot];
			if ( pCopySavedPos )
			{
				int nCount = 0;
				while ( pCopySavedPos[nCount].nSavedPosFlags & SavedPosMap::SPM_USED )
				{
					++nCount;
					if ( pCopySavedPos[nCount-1].nSavedPosFlags & SavedPosMap::SPM_LAST )
						break;
				}
				SavedPos* pNewSavedPos = new SavedPos[nCount];
				for ( int nCopy=0; nCopy<nCount; ++nCopy )
					pNewSavedPos[nCopy] = pCopySavedPos[nCopy];
				pNewSavedPos[nCount-1].nSavedPosFlags |= SavedPosMap::SPM_LAST;
				m_mapSavedPos.pTable[nSlot] = pNewSavedPos;
			}
		}
	}

	MARKUP_SETDEBUGSTATE;
}

bool CMarkup::SetDoc( LPCTSTR szDoc )
{
	// Set document text
	if ( szDoc )
		m_strDoc = szDoc;
	else
		m_strDoc.Empty();

	m_strError.Empty();
	return x_ParseDoc();
};

bool CMarkup::IsWellFormed(void)
{
	if ( m_aPos.GetSize()
			&& ! (m_aPos[0].nFlags & MNF_ILLFORMED)
			&& m_aPos[0].iElemChild
			&& ! m_aPos[m_aPos[0].iElemChild].iElemNext )
		return true;
	return false;
}

bool CMarkup::Load( LPCTSTR szFileName )
{
	if ( ! ReadTextFile(szFileName, m_strDoc, &m_strError, &m_nFlags) )
		return false;
	return x_ParseDoc();
}

bool CMarkup::ReadTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError, int* pnFlags )
{
	// Static utility method to load text file into strDoc
	//
	// Open file to read binary
	FILE* fp = _tfopen( szFileName, _T("r") ); //fix here
	if ( ! fp )
	{
		if ( pstrError )
			*pstrError = strerror(errno);
		return false;
	}

    FILE* fpTmp = _tfopen(szFileName, _T("r"));//fix here
	if (!fpTmp)
	{
		if (pstrError)
			*pstrError = strerror(errno);
		return false;
	}

	// Set flags to 0 unless flags argument provided
	int nFlags = pnFlags?*pnFlags:0;
	_TCHAR szDescBOM[20] = {0};
	strDoc.Empty();

	// Get file length
	fseek( fp, 0, SEEK_END );
	int nFileByteLen = ftell( fp );
	fseek( fp, 0, SEEK_SET );

    BOOL bIsGB2312 = FALSE;
    
    fseek(fpTmp, 0, SEEK_SET);

    _TCHAR firstLine[100];
    fread(firstLine, sizeof(_TCHAR), 100, fpTmp);
    fclose(fpTmp);

    CString firstLinsStr(firstLine);
    
    firstLinsStr.MakeLower();
    if (firstLinsStr.Find(_T("\"gb2312\"")) != -1)
        bIsGB2312 = TRUE;
    
    if(!bIsGB2312) // convert file to wide char
    {
        int nWideLen = 0;
        if ( nFileByteLen )
        {
            char* pBuffer = new char[nFileByteLen];
			memset(pBuffer,0,nFileByteLen); //fix here
            fread( pBuffer,1, nFileByteLen,  fp );
            // For ANSI files, replace CP_UTF8 with CP_ACP in both places
            /*nWideLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer, nFileByteLen, NULL, 0);
            MultiByteToWideChar(CP_UTF8, 0, pBuffer, nFileByteLen, strDoc.GetBuffer(nWideLen), nWideLen);
            strDoc.ReleaseBuffer( nWideLen );//*/

// 			nWideLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer, nFileByteLen, NULL, 0);
// 			WCHAR * pszWideChar_Buffer = new WCHAR[nWideLen];
//             MultiByteToWideChar(CP_UTF8, 0, pBuffer, nFileByteLen, pszWideChar_Buffer, nWideLen);
//             strDoc = pszWideChar_Buffer;
// 
// 			delete pBuffer;
// 			delete pszWideChar_Buffer;//*/
            strDoc = pBuffer;
			delete [] pBuffer;
        }
        if ( pstrError )
            (*pstrError).Format(_T("%s%d bytes to %d wide chars"),szDescBOM,nFileByteLen,nWideLen);
    }
    else // read file directly
    {
        if ( nFileByteLen )
        {
            fread( strDoc.GetBuffer(nFileByteLen), nFileByteLen, 1, fp );
            strDoc.ReleaseBuffer( nFileByteLen );
        }
        if ( pstrError )
            (*pstrError).Format( _T("%s%d bytes"), szDescBOM, nFileByteLen );
    }

	fclose( fp );
	if ( pnFlags )
		*pnFlags = nFlags;
	return true;
}

bool CMarkup::Save( LPCTSTR szFileName )
{
	return WriteTextFile( szFileName, m_strDoc, &m_strError, &m_nFlags );
}

bool CMarkup::WriteTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError, int* pnFlags )
{
	// Static utility method to save strDoc to text file
	//
	// Open file to write binary
	bool bSuccess = true;
	FILE* fp = _tfopen( szFileName, _T("w") ); //fix here
	if ( ! fp )
	{
		if ( pstrError )
			*pstrError = strerror(errno);
		return false;
	}

	// Set flags to 0 unless flags argument provided
	int nFlags = pnFlags?*pnFlags:0;
	_TCHAR szDescBOM[20] = {0};

	// Get document length
	int nDocLength = strDoc.GetLength();


#if defined( _UNICODE )
	int nMBLen = 0;
	if ( nDocLength )
	{
		// For ANSI files, replace CP_UTF8 with CP_ACP in both places
		nMBLen = WideCharToMultiByte(CP_ACP,0,strDoc,nDocLength,NULL,0,NULL,NULL); //fix here
		char* pBuffer = new char[nMBLen+1];
		WideCharToMultiByte(CP_ACP,0,strDoc,nDocLength,pBuffer,nMBLen+1,NULL,NULL); //fix here
		bSuccess = ( fwrite( pBuffer, nMBLen, 1, fp ) == 1 );
		delete [] pBuffer;
	}
	if ( pstrError )
		(*pstrError).Format( _T("%d wide chars to %s%d bytes"), nDocLength, szDescBOM, nMBLen );
#else // MBCS or UTF-8
	if ( nDocLength )
	{
		CString strDocWrite = strDoc; // reference unless converted
		nDocLength = strDocWrite.GetLength();
		bSuccess = ( fwrite( (LPCTSTR)strDocWrite, nDocLength, 1, fp ) == 1 );
	}
	if ( pstrError )
		(*pstrError).Format( _T("%s%d bytes"), szDescBOM, nDocLength );
#endif
	
	if ( ! bSuccess && pstrError )
		*pstrError = strerror(errno);
	fclose(fp);
	if ( pnFlags )
		*pnFlags = nFlags;
	return bSuccess;
}

bool CMarkup::FindElem( LPCTSTR szName )
{
	// Change current position only if found
	//
	if ( m_aPos.GetSize() )
	{
		int iPos = x_FindElem( m_iPosParent, m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkup::FindChildElem( LPCTSTR szName )
{
	// Change current child position only if found
	//
	// Shorthand: call this with no current main position
	// means find child under root element
	if ( ! m_iPos )
		FindElem();

	int iPosChild = x_FindElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}

CString CMarkup::EscapeText( LPCTSTR szText, int nFlags )
{
	// Convert text as seen outside XML document to XML friendly
	// replacing special characters with ampersand escape codes
	// E.g. convert "6>7" to "6&gt;7"
	//
	// &lt;   less than
	// &amp;  ampersand
	// &gt;   greater than
	//
	// and for attributes:
	//
	// &apos; apostrophe or single quote
	// &quot; double quote
	//
	static LPCTSTR szaReplace[] = { _T("&lt;"),_T("&amp;"),_T("&gt;"),_T("&apos;"),_T("&quot;") };
	LPCTSTR pFind = (nFlags&MNF_ESCAPEQUOTES)?_T("<&>\'\""):_T("<&>");
	CString strText;
	LPCTSTR pSource = szText;
	int nDestSize = (int)_tcslen(pSource);
	nDestSize += nDestSize / 10 + 7;
	_TCHAR* pDest = strText.GetBuffer(nDestSize);
	int nLen = 0;
	_TCHAR cSource = *pSource;
	LPCTSTR pFound;
	while ( cSource )
	{
		if ( nLen > nDestSize - 6 )
		{
			strText.ReleaseBuffer(nLen);
			nDestSize *= 2;
			pDest = strText.GetBuffer(nDestSize);
		}
		if ( (pFound=_tcschr(pFind,cSource)) != NULL )
		{
			bool bIgnoreAmpersand = false;
			if ( (nFlags&MNF_WITHREFS) && *pFound == _T('&') )
			{
				// Do not replace ampersand if it is start of any entity reference
				// &[#_:A-Za-zU][_:-.A-Za-z0-9U]*; where U is > 0x7f
				LPCTSTR pCheckEntity = pSource;
				++pCheckEntity;
				_TCHAR c = *pCheckEntity;
				if ( (c>=_T('A')&&c<=_T('Z')) || (c>=_T('a')&&c<=_T('z'))
						|| c==_T('#') || c==_T('_') || c==_T(':') || c>0x7f )
				{
					while ( 1 )
					{
						pCheckEntity += _tclen( pCheckEntity );
						c = *pCheckEntity;
						if ( c == _T(';') )
						{
							int nEntityLen = (int)(pCheckEntity - pSource) + 1;
							_tcsncpy(&pDest[nLen],pSource,nEntityLen);
							nLen += nEntityLen;
							pSource = pCheckEntity;
							bIgnoreAmpersand = true;
						}
						else if ( (c>=_T('A')&&c<=_T('Z')) || (c>=_T('a')&&c<=_T('z')) || (c>=_T('0')&&c<=_T('9'))
								|| c==_T('_') || c==_T(':') || c==_T('-') || c==_T('.') || c>0x7f )
							continue;
						break;
					}
				}
			}
			if ( ! bIgnoreAmpersand )
			{
				pFound = szaReplace[pFound-pFind];
				_tcscpy(&pDest[nLen],pFound);
				nLen += (int)_tcslen(pFound);
			}
		}
		else
		{
			_tccpy( &pDest[nLen], pSource );
			nLen += (int)_tclen( pSource );
		}
		pSource += _tclen( pSource );
		cSource = *pSource;
	}
	strText.ReleaseBuffer(nLen);
	return strText;
}

CString CMarkup::UnescapeText( LPCTSTR szText, int nTextLength /*=-1*/ )
{
	// Convert XML friendly text to text as seen outside XML document
	// ampersand escape codes replaced with special characters e.g. convert "6&gt;7" to "6>7"
	// ampersand numeric codes replaced with character e.g. convert &#60; to <
	// Conveniently the result is always the same or shorter in byte length
	//
	static LPCTSTR szaCode[] = { _T("lt;"),_T("amp;"),_T("gt;"),_T("apos;"),_T("quot;") };
	static int anCodeLen[] = { 3,4,3,5,5 };
	static LPCTSTR szSymbol = _T("<&>\'\"");
	CString strText;
	LPCTSTR pSource = szText;
	if ( nTextLength == -1 )
		nTextLength = (int)_tcslen(szText);
	_TCHAR* pDest = strText.GetBuffer( nTextLength );
	int nLen = 0;
	int nCharLen;
	int nChar = 0;
	while ( nChar < nTextLength )
	{
		if ( pSource[nChar] == _T('&') )
		{
			bool bCodeConverted = false;

			// Is it a numeric character reference?
			if ( pSource[nChar+1] == _T('#') )
			{
				// Is it a hex number?
				int nBase = 10;
				int nNumericChar = nChar + 2;
				_TCHAR cChar = pSource[nNumericChar];
				if ( cChar == _T('x') )
				{
					++nNumericChar;
					cChar = pSource[nNumericChar];
					nBase = 16;
				}

				// Look for terminating semi-colon within 7 characters
				int nCodeLen = 0;
				while ( nCodeLen < 7 && cChar && cChar != _T(';') )
				{
					// only ASCII digits 0-9, A-F, a-f expected
					nCodeLen += (int)_tclen( &pSource[nNumericChar+nCodeLen] );
					cChar = pSource[nNumericChar + nCodeLen];
				}

				// Process unicode
				if ( cChar == _T(';') )
				{
					int nUnicode = _tcstol( &pSource[nNumericChar], NULL, nBase );
#if defined(_UNICODE)
					pDest[nLen++] = (_TCHAR)nUnicode;
#elif defined(_MBCS)
					int nMBLen = wctomb( &pDest[nLen], (wchar_t)nUnicode );
					if ( nMBLen > 0 )
						nLen += nMBLen;
					else
						nUnicode = 0;
#else
					if ( nUnicode < 0x80 )
						pDest[nLen++] = (_TCHAR)nUnicode;
					else if ( nUnicode < 0x800 )
					{
						// Convert to 2-byte UTF-8
						pDest[nLen++] = (_TCHAR)(((nUnicode&0x7c0)>>6) | 0xc0);
						pDest[nLen++] = (_TCHAR)((nUnicode&0x3f) | 0x80);
					}
					else
					{
						// Convert to 3-byte UTF-8
						pDest[nLen++] = (_TCHAR)(((nUnicode&0xf000)>>12) | 0xe0);
						pDest[nLen++] = (_TCHAR)(((nUnicode&0xfc0)>>6) | 0x80);
						pDest[nLen++] = (_TCHAR)((nUnicode&0x3f) | 0x80);
					}
#endif
					if ( nUnicode )
					{
						// Increment index past ampersand semi-colon
						nChar = nNumericChar + nCodeLen + 1;
						bCodeConverted = true;
					}
				}
			}
			else // does not start with #
			{
				// Look for matching &code;
				for ( int nMatch = 0; nMatch < 5; ++nMatch )
				{
					if ( nChar < nTextLength - anCodeLen[nMatch]
						&& _tcsncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
					{
						// Insert symbol and increment index past ampersand semi-colon
						pDest[nLen++] = szSymbol[nMatch];
						nChar += anCodeLen[nMatch] + 1;
						bCodeConverted = true;
						break;
					}
				}
			}

			// If the code is not converted, leave it as is
			if ( ! bCodeConverted )
			{
				pDest[nLen++] = _T('&');
				++nChar;
			}
		}
		else // not &
		{
			nCharLen = (int)_tclen(&pSource[nChar]);
			_tccpy( &pDest[nLen], &pSource[nChar] );
			nLen += nCharLen;
			nChar += nCharLen;
		}
	}
	strText.ReleaseBuffer(nLen);
	return strText;
}


int CMarkup::FindNode( int nType )
{
	// Change current node position only if a node is found
	// If nType is 0 find any node, otherwise find node of type nType
	// Return type of node or 0 if not found
	// If found node is an element, change m_iPos

	// Determine where in document to start scanning for node
	int nTypeFound = 0;
	int nNodeOffset = m_nNodeOffset;
	if ( m_nNodeType > 1 )
	{
		// By-pass current node
		nNodeOffset += m_nNodeLength;
	}
	else
	{
		// Set position to begin looking for node
		nNodeOffset = 0; // default to start of document
		if ( m_iPos )
		{
			// After element
			nNodeOffset = m_aPos[m_iPos].StartAfter();
		}
		else if ( m_iPosParent )
		{
			// Immediately after start tag of parent
			if ( m_aPos[m_iPosParent].IsEmptyElement() )
				return 0;
			else
				nNodeOffset = m_aPos[m_iPosParent].StartContent();
		}
	}

	// Get nodes until we find what we're looking for
	int iPosNew = m_iPos;
	TokenPos token( m_strDoc, m_nFlags );
	NodePos node;
	token.nNext = nNodeOffset;
	do
	{
		nNodeOffset = token.nNext;
		nTypeFound = x_ParseNode( token, node );
		if ( nTypeFound == 0 )
		{
			// Check if we have reached the end of the parent element
			// Otherwise it is a lone end tag
			if ( m_iPosParent && nNodeOffset == m_aPos[m_iPosParent].StartContent()
					+ m_aPos[m_iPosParent].ContentLen() )
				return 0;
			nTypeFound = MNT_LONE_END_TAG;
		}
		else if ( nTypeFound < 0 )
		{
			if ( nTypeFound == -2 )
				return 0;
			// -1 is node error
			nTypeFound = MNT_NODE_ERROR;
		}
		else if ( nTypeFound == MNT_ELEMENT )
		{
			if ( iPosNew )
				iPosNew = m_aPos[iPosNew].iElemNext;
			else
				iPosNew = m_aPos[m_iPosParent].iElemChild;
			if ( ! iPosNew )
				return 0;
			if ( ! nType || (nType & nTypeFound) )
			{
				// Found element node, move position to this element
				x_SetPos( m_iPosParent, iPosNew, 0 );
				return m_nNodeType;
			}
			token.nNext = m_aPos[iPosNew].StartAfter();
		}
	}
	while ( nType && ! (nType & nTypeFound) );

	m_iPos = iPosNew;
	m_iPosChild = 0;
	m_nNodeOffset = nNodeOffset;
	m_nNodeLength = token.nNext - nNodeOffset;
	m_nNodeType = nTypeFound;
	MARKUP_SETDEBUGSTATE;
	return m_nNodeType;
}

bool CMarkup::RemoveNode(void)
{
	if ( m_iPos || m_nNodeLength )
	{
		x_RemoveNode( m_iPosParent, m_iPos, m_nNodeType, m_nNodeOffset, m_nNodeLength );
		m_iPosChild = 0;
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	return false;
}

CString CMarkup::GetTagName(void) const
{
	// Return the tag name at the current main position
	CString strTagName;

	// This method is primarily for elements, however
	// it does return something for certain other nodes
	if ( m_nNodeLength )
	{
		switch ( m_nNodeType )
		{
		case MNT_PROCESSING_INSTRUCTION:
		case MNT_LONE_END_TAG:
			{
				// <?target or </tagname
				TokenPos token( m_strDoc, m_nFlags );
				token.nNext = m_nNodeOffset + 2;
				if ( x_FindName(token) )
					strTagName = x_GetToken( token );
			}
			break;
		case MNT_COMMENT:
			strTagName = _T("#comment");
			break;
		case MNT_CDATA_SECTION:
			strTagName = _T("#cdata-section");
			break;
		case MNT_DOCUMENT_TYPE:
			{
				// <!DOCTYPE name
				TokenPos token( m_strDoc, m_nFlags );
				token.nNext = m_nNodeOffset + 2;
				if ( x_FindName(token) && x_FindName(token) )
					strTagName = x_GetToken( token );
			}
			break;
		case MNT_TEXT:
		case MNT_WHITESPACE:
			strTagName = _T("#text");
			break;
		}
		return strTagName;
	}

	if ( m_iPos )
		strTagName = x_GetTagName( m_iPos );
	return strTagName;
}

bool CMarkup::IntoElem(void)
{
	// If there is no child position and IntoElem is called it will succeed in release 6.3
	// (A subsequent call to FindElem will find the first element)
	// The following short-hand behavior was never part of EDOM and was misleading
	// It would find a child element if there was no current child element position and go into it
	// It is removed in release 6.3, this change is NOT backwards compatible!
	// if ( ! m_iPosChild )
	//	FindChildElem();

	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		x_SetPos( m_iPos, m_iPosChild, 0 );
		return true;
	}
	return false;
}

bool CMarkup::OutOfElem(void)
{
	// Go to parent element
	if ( m_iPosParent )
	{
		x_SetPos( m_aPos[m_iPosParent].iElemParent, m_iPosParent, m_iPos );
		return true;
	}
	return false;
}

CString CMarkup::GetAttribName( int n ) const
{
	// Return nth attribute name of main position
	TokenPos token( m_strDoc, m_nFlags );
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[m_iPos].nStart + 1;
	else if ( m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return _T("");
	if ( x_FindAttrib(token,NULL,n) )
		return x_GetToken( token );
	return _T("");
}

bool CMarkup::SavePos( LPCTSTR szPosName )
{
	// Save current element position in saved position map
	if ( szPosName )
	{
		SavedPos savedpos;
		if ( szPosName )
			savedpos.strName = szPosName;
		if ( m_iPosChild )
		{
			savedpos.iPos = m_iPosChild;
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_CHILD;
		}
		else if ( m_iPos )
		{
			savedpos.iPos = m_iPos;
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_MAIN;
		}
		else
		{
			savedpos.iPos = m_iPosParent;
		}
		savedpos.nSavedPosFlags |= SavedPosMap::SPM_USED;

		if ( ! m_mapSavedPos.pTable )
			m_mapSavedPos.AllocMapTable();
		int nSlot = m_mapSavedPos.Hash( szPosName );
		SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
		int nOffset = 0;
		if ( ! pSavedPos )
		{
			pSavedPos = new SavedPos[2];
			pSavedPos[1].nSavedPosFlags = SavedPosMap::SPM_LAST;
			m_mapSavedPos.pTable[nSlot] = pSavedPos;
		}
		else
		{
			while ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				if ( pSavedPos[nOffset].strName == szPosName )
					break;
				if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
				{
					int nNewSize = (nOffset + 6) * 2;
					SavedPos* pNewSavedPos = new SavedPos[nNewSize];
					for ( int nCopy=0; nCopy<=nOffset; ++nCopy )
						pNewSavedPos[nCopy] = pSavedPos[nCopy];
					pNewSavedPos[nOffset].nSavedPosFlags ^= SavedPosMap::SPM_LAST;
					pNewSavedPos[nNewSize-1].nSavedPosFlags = SavedPosMap::SPM_LAST;
					delete [] pSavedPos;
					pSavedPos = pNewSavedPos;
					m_mapSavedPos.pTable[nSlot] = pSavedPos;
					++nOffset;
					break;
				}
				++nOffset;
			}
		}
		if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_LAST;
		pSavedPos[nOffset] = savedpos;

		/*
		// To review hash table balance, uncomment and watch strBalance
		CString strBalance, strSlot;
		for ( nSlot=0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			pSavedPos = m_mapSavedPos.pTable[nSlot];
			int nCount = 0;
			while ( pSavedPos && pSavedPos->nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				++nCount;
				if ( pSavedPos->nSavedPosFlags & SavedPosMap::SPM_LAST )
					break;
				++pSavedPos;
			}
			strSlot.Format( _T("%d "), nCount );
			strBalance += strSlot;
		}
		*/

		return true;
	}
	return false;
}

bool CMarkup::RestorePos( LPCTSTR szPosName )
{
	// Restore element position if found in saved position map
	if ( szPosName && m_mapSavedPos.pTable )
	{
		int nSlot = m_mapSavedPos.Hash( szPosName );
		SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
		if ( pSavedPos )
		{
			int nOffset = 0;
			while ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				if ( pSavedPos[nOffset].strName == szPosName )
				{
					int i = pSavedPos[nOffset].iPos;
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_CHILD )
						x_SetPos( m_aPos[m_aPos[i].iElemParent].iElemParent, m_aPos[i].iElemParent, i );
					else if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_MAIN )
						x_SetPos( m_aPos[i].iElemParent, i, 0 );
					else
						x_SetPos( i, 0, 0 );
					return true;
				}
				if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
					break;
				++nOffset;
			}
		}
	}
	return false;
}

bool CMarkup::RemoveElem(void)
{
	// Remove current main position element
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		int iPos = x_RemoveElem( m_iPos );
		x_SetPos( m_iPosParent, iPos, 0 );
		return true;
	}
	return false;
}

bool CMarkup::RemoveChildElem(void)
{
	// Remove current child position element
	if ( m_iPosChild )
	{
		int iPosChild = x_RemoveElem( m_iPosChild );
		x_SetPos( m_iPosParent, m_iPos, iPosChild );
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

bool CMarkup::x_AllocPosArray( int nNewSize /*=0*/ )
{
	// Resize m_aPos when the document is created or the array is filled
	// The PosArray class is implemented using segments to reduce contiguous memory requirements
	// It reduces reallocations (copying of memory) since this only occurs within one segment
	// The "Grow By" algorithm ensures there are no reallocations after 2 segments
	//
	if ( ! nNewSize )
		nNewSize = m_iPosFree + (m_iPosFree>>1); // Grow By: multiply size by 1.5
	if ( m_aPos.GetSize() < nNewSize )
	{
		// Grow By: new size can be at most one more complete segment
		int nSeg = (m_aPos.GetSize()?m_aPos.GetSize()-1:0) >> m_aPos.PA_SEGBITS;
		int nNewSeg = (nNewSize-1) >> m_aPos.PA_SEGBITS;
		if ( nNewSeg > nSeg + 1 )
		{
			nNewSeg = nSeg + 1;
			nNewSize = (nNewSeg+1) << m_aPos.PA_SEGBITS;
		}

		// Allocate array of segments
		if ( m_aPos.nSegs <= nNewSeg )
		{
			int nNewSegments = 4 + nNewSeg * 2;
			char* pNewSegments = new char[nNewSegments*sizeof(char*)];
			if ( m_aPos.SegsUsed() )
				memcpy( pNewSegments, m_aPos.pSegs, m_aPos.SegsUsed()*sizeof(char*) );
			if ( m_aPos.pSegs )
				delete[] (char*)m_aPos.pSegs;
			m_aPos.pSegs = (ElemPos**)pNewSegments;
			m_aPos.nSegs = nNewSegments;
		}

		// Calculate segment sizes
		int nSegSize = m_aPos.GetSize() - (nSeg << m_aPos.PA_SEGBITS);
		int nNewSegSize = nNewSize - (nNewSeg << m_aPos.PA_SEGBITS);

		// Complete first segment
		int nFullSegSize = 1 << m_aPos.PA_SEGBITS;
		if ( nSeg < nNewSeg && nSegSize < nFullSegSize )
		{
			char* pNewFirstSeg = new char[ nFullSegSize * sizeof(ElemPos) ];
			if ( nSegSize )
			{
				// Reallocate
				memcpy( pNewFirstSeg, m_aPos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
				delete[] (char*)m_aPos.pSegs[nSeg];
			}
			m_aPos.pSegs[nSeg] = (ElemPos*)pNewFirstSeg;
		}

		// New segment
		char* pNewSeg = new char[ nNewSegSize * sizeof(ElemPos) ];
		if ( nNewSeg == nSeg && nSegSize )
		{
			// Reallocate
			memcpy( pNewSeg, m_aPos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
			delete[] (char*)m_aPos.pSegs[nSeg];
		}
		m_aPos.pSegs[nNewSeg] = (ElemPos*)pNewSeg;
		m_aPos.nSize = nNewSize;
	}
	return true;
}

bool CMarkup::x_ParseDoc(void)
{
	// Preserve pre-parse result
	CString strResult = m_strError;

	// Reset indexes
	ResetPos();
	m_mapSavedPos.RemoveAll();

	// Starting size of position array: 1 element per 64 bytes of document
	// Tight fit when parsing small doc, only 0 to 2 reallocs when parsing large doc
	// Start at 8 when creating new document
	m_iPosFree = 1;
	x_AllocPosArray( m_strDoc.GetLength() / 64 + 8 );
	m_iPosDeleted = 0;

	// Parse document
	m_aPos[0].ClearVirtualParent();
	if ( m_strDoc.GetLength() )
	{
		TokenPos token( m_strDoc, m_nFlags );
		int iPos = x_ParseElem( 0, token );
		m_aPos[0].nLength = m_strDoc.GetLength();
		if ( iPos > 0 )
		{
			m_aPos[0].iElemChild = iPos;
			if ( m_aPos[iPos].iElemNext )
				m_strError = _T("Root element has sibling");
		}
		else
			m_strError = _T("No root element");
	}
	else
		m_strError = _T("Empty document");

	ResetPos();

	// Combine preserved result with parse error
	if ( ! strResult.IsEmpty() )
	{
		if ( m_strError.IsEmpty() )
			m_strError = strResult;
		else
			m_strError = strResult + _T(", ") + m_strError;
	}

	return IsWellFormed();
};

int CMarkup::x_ParseElem( int iPosParent, TokenPos& token )
{
	// This is either called by x_ParseDoc or x_AddSubDoc or x_SetElemContent
	// Returns index of the first element encountered or zero if no elements
	//
	int iElemRoot = 0;
	int iPos = iPosParent;
	int iVirtualParent = iPosParent;
	int nRootDepth = m_aPos[iPos].Level();
	token.nNext = 0;
	m_strError.Empty();

	// Loop through the nodes of the document
	NodeStack aNodes;
	aNodes.Add();
	int nDepth = 0;
	int nMatchDepth;
	int iPosChild;
	int iPosMatch;
	int nTypeFound = 0;
	ElemPos* pElem;
	int iElemFirst, iElemLast;
	while ( 1 )
	{
		nTypeFound = x_ParseNode( token, aNodes.Top() );
		nMatchDepth = 0;
		if ( nTypeFound == MNT_ELEMENT ) // start tag
		{
			iPos = x_GetFreePos();
			if ( ! iElemRoot )
				iElemRoot = iPos;
			pElem = &m_aPos[iPos];
			pElem->iElemParent = iPosParent;
			pElem->iElemNext = 0;
			if ( m_aPos[iPosParent].iElemChild )
			{
				iElemFirst = m_aPos[iPosParent].iElemChild;
				iElemLast = m_aPos[iElemFirst].iElemPrev;
				m_aPos[iElemLast].iElemNext = iPos;
				pElem->iElemPrev = iElemLast;
				m_aPos[iElemFirst].iElemPrev = iPos;
				pElem->nFlags = 0;
			}
			else
			{
				m_aPos[iPosParent].iElemChild = iPos;
				pElem->iElemPrev = iPos;
				pElem->nFlags = MNF_FIRST;
			}
			pElem->SetLevel( nRootDepth + nDepth );
			pElem->iElemChild = 0;
			pElem->nStart = aNodes.Top().nStart;
			pElem->SetStartTagLen( aNodes.Top().nLength );
			if ( aNodes.Top().nFlags & MNF_EMPTY )
			{
				iPos = iPosParent;
				pElem->SetEndTagLen( 0 );
				pElem->nLength = aNodes.Top().nLength;
			}
			else
			{
				iPosParent = iPos;
				++nDepth;
				aNodes.Add();
			}
		}
		else if ( nTypeFound == 0 ) // end tag
		{
			nMatchDepth = nDepth;
			iPosMatch = iPos;
			while ( nMatchDepth && ! token.Match(aNodes.At(nMatchDepth-1).strMeta) )
			{
				/*
				// Auto-switch case sensitivity
				if ( ! (token.nTokenFlags & MDF_IGNORECASE ) )
				{
					token.nTokenFlags |= MDF_IGNORECASE;
					if ( token.Match(aNodes.At(nMatchDepth-1).strMeta) )
						break;
					token.nTokenFlags |= MDF_IGNORECASE;
				}
				*/
				--nMatchDepth;
				iPosMatch = m_aPos[iPosMatch].iElemParent;
			}
			if ( nMatchDepth == 0 )
			{
				// Not matched at all, it is a lone end tag, a non-element node
				m_aPos[iVirtualParent].nFlags |= MNF_ILLFORMED;
				m_aPos[iPos].nFlags |= MNF_ILLDATA;
				if ( m_strError.IsEmpty() )
				{
					m_strError.Format( _T("No start tag for end tag '%s' at offset %d"),
						x_GetToken(token), aNodes.Top().nStart );
				}
			}
			else
			{
				pElem = &m_aPos[iPosMatch];
				pElem->nLength = aNodes.Top().nStart - pElem->nStart + aNodes.Top().nLength;
				pElem->SetEndTagLen( aNodes.Top().nLength );
			}
		}
		else if ( nTypeFound == -1 )
		{
			m_aPos[iVirtualParent].nFlags |= MNF_ILLFORMED;
			m_aPos[iPos].nFlags |= MNF_ILLDATA;
			if ( m_strError.IsEmpty() )
				m_strError = aNodes.Top().strMeta;
		}

		// Matched end tag, or end of document
		if ( nMatchDepth || nTypeFound == -2 )
		{
			if ( nDepth > nMatchDepth )
				m_aPos[iVirtualParent].nFlags |= MNF_ILLFORMED;

			// Process any non-ended elements
			while ( nDepth > nMatchDepth )
			{
				// Element with no end tag
				pElem = &m_aPos[iPos];
				iPosChild = pElem->iElemChild;
				iPosParent = pElem->iElemParent;
				pElem->SetEndTagLen( 0 );
				pElem->nFlags |= MNF_NONENDED;
				pElem->iElemChild = 0;
				pElem->nLength = pElem->StartTagLen();
				if ( pElem->nFlags & MNF_ILLDATA )
				{
					pElem->nFlags ^= MNF_ILLDATA;
					m_aPos[iPosParent].nFlags |= MNF_ILLDATA;
				}
				while ( iPosChild )
				{
					m_aPos[iPosChild].iElemParent = iPosParent;
					m_aPos[iPosChild].iElemPrev = iPos;
					m_aPos[iPos].iElemNext = iPosChild;
					iPos = iPosChild;
					iPosChild = m_aPos[iPosChild].iElemNext;
				}
				iPos = iPosParent;
				aNodes.Remove();
				--nDepth;

				// Error string
				// if end tag did not match, top node is end tag that did not match pElem
				// if end of document, any nodes below top have no end tag
				if ( m_strError.IsEmpty() )
				{
					if ( nTypeFound == 0 )
						m_strError.Format( _T("End tag '%s' at offset %d does not match start tag '%s' at offset %d"),
							x_GetToken(token), token.nL-1, aNodes.Top().strMeta, pElem->nStart );
					else
						m_strError.Format( _T("Element '%s' at offset %d not ended"),
							aNodes.Top().strMeta, aNodes.Top().nStart );
				}
			}
			if ( nTypeFound == -2 )
				break;
			iPosParent = m_aPos[iPos].iElemParent;
			iPos = iPosParent;
			aNodes.Remove();
			--nDepth;
		}
	}
	return iElemRoot;
}

bool CMarkup::x_FindAny( LPCTSTR szDoc, int& nChar )
{
	// Starting at nChar, find a non-whitespace char
	// return false if no non-whitespace before end of document, nChar points to end
	// otherwise return true and nChar points to non-whitespace char
	while ( szDoc[nChar] && _tcschr(_T(" \t\n\r"),szDoc[nChar]) )
		++nChar;
	return szDoc[nChar] != _T('\0');
}

bool CMarkup::x_FindName( CMarkup::TokenPos& token )
{
	// Starting at token.nNext, bypass whitespace and find the next name
	// returns true on success, members of token point to token
	// returns false on end of document, members point to end of document
	LPCTSTR szDoc = token.szDoc;
	int nChar = token.nNext;

	// By-pass leading whitespace
	if ( ! x_FindAny(szDoc,nChar) )
	{
		// No token was found before end of document
		token.nL = nChar;
		token.nR = nChar - 1;
		token.nNext = nChar;
		return false;
	}

	// Go until special char or whitespace
	token.nL = nChar;
	while ( szDoc[nChar] && ! _tcschr(_T(" \t\n\r<>=\\/?!"),szDoc[nChar]) )
		nChar += (int)_tclen(&szDoc[nChar]);

	// Adjust end position if it is one special char
	if ( nChar == token.nL )
		++nChar; // it is a special char
	token.nR = nChar - 1;

	// nNext points to one past last char of token
	token.nNext = nChar;
	return true;
}

CString CMarkup::x_GetToken( const CMarkup::TokenPos& token )
{
	// The token contains indexes into the document identifying a small substring
	// Build the substring from those indexes and return it
	if ( token.nL > token.nR )
		return _T("");
	CString strToken( &token.szDoc[token.nL], token.Length() );
	return strToken;
}

int CMarkup::x_FindElem( int iPosParent, int iPos, LPCTSTR szPath ) const
{
	// If szPath is NULL or empty, go to next sibling element
	// Otherwise go to next sibling element with matching path
	//
	if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[0] )
		return iPos;

	// Search
	TokenPos token( m_strDoc, m_nFlags );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStart + 1;
		x_FindName( token ); // Locate tag name
		if ( token.Match(szPath) )
			return iPos;
		iPos = m_aPos[iPos].iElemNext;
	}
	return 0;

}

int CMarkup::x_ParseNode( CMarkup::TokenPos& token, CMarkup::NodePos& node )
{
	// Call this with token.nNext set to the start of the node or tag
	// Upon return token.nNext points to the char after the node or tag
	// 
	// <!--...--> comment
	// <!DOCTYPE ...> dtd
	// <?target ...?> processing instruction
	// <![CDATA[...]]> cdata section
	// <NAME ...> element start tag
	// </NAME ...> element end tag
	//
	// returns the nodetype or
	// 0 for end tag
	// -1 for bad node
	// -2 for end of document
	//
	enum ParseBits
	{
		PD_OPENTAG = 1,
		PD_BANG = 2,
		PD_DASH = 4,
		PD_BRACKET = 8,
		PD_TEXTORWS = 16,
		PD_DOCTYPE = 32,
		PD_INQUOTE_S = 64,
		PD_INQUOTE_D = 128,
	};
	int nParseFlags = 0;

	LPCTSTR szFindEnd = NULL;
	int nNodeType = -1;
	int nEndLen = 0;
	int nName = 0;
	unsigned int cDminus1 = 0, cDminus2 = 0;
	#define FINDNODETYPE(e,t,n) { szFindEnd=e; nEndLen=(sizeof(e)-1)/sizeof(_TCHAR); nNodeType=t; if(n) nName=(int)(pDoc-token.szDoc)+n-1; }
	#define FINDNODEBAD(e) { szFindEnd=_T(">"); nEndLen=1; node.strMeta.Format(_T("Incorrect %s at offset %d"),e,nR); nNodeType=-1; }

	node.nStart = token.nNext;
	node.nFlags = 0;

	int nR = token.nNext;
	LPCTSTR pDoc = &token.szDoc[nR];
	register unsigned int cD = (unsigned int)*pDoc;
	if ( ! cD )
	{
		node.nLength = 0;
		node.nNodeType = 0;
		return -2; // end of document
	}

	while ( 1 )
	{
		cD = (unsigned int)*pDoc;
		if ( ! cD )
		{
			nR = (int)(pDoc - token.szDoc) - 1;
			if ( nNodeType != MNT_WHITESPACE && nNodeType != MNT_TEXT )
			{
				LPCTSTR szType = _T("tag");
				if ( (nParseFlags & PD_DOCTYPE) || nNodeType == MNT_DOCUMENT_TYPE )
					szType = _T("Doctype");
				else if ( nNodeType == MNT_ELEMENT )
					szType = _T("Element tag");
				else if ( nNodeType == 0 )
					szType = _T("Element end tag");
				else if ( nNodeType == MNT_CDATA_SECTION )
					szType = _T("CDATA Section");
				else if ( nNodeType == MNT_PROCESSING_INSTRUCTION )
					szType = _T("Processing instruction");
				else if ( nNodeType == MNT_COMMENT )
					szType = _T("Comment");
				nNodeType = -1;
				node.strMeta.Format( _T("%s at offset %d unterminated"), szType, node.nStart );
			}
			break;
		}

		if ( nName )
		{
			if ( _tcschr(_T(" \t\n\r/>"),(_TCHAR)cD) )
			{
				int nNameLen = (int)(pDoc - token.szDoc) - nName;
				if ( nNodeType == 0 )
				{
					token.nL = nName;
					token.nR = nName + nNameLen - 1;
				}
				else
				{
					memcpy( node.strMeta.GetBuffer(nNameLen), &token.szDoc[nName], nNameLen*sizeof(_TCHAR) );
					node.strMeta.ReleaseBuffer( nNameLen );
				}
				nName = 0;
				cDminus2 = 0;
				cDminus1 = 0;
			}
			else
			{
				++pDoc;
				continue;
			}
		}

		if ( szFindEnd )
		{
			if ( cD == _T('>') && ! (nParseFlags & (PD_INQUOTE_S|PD_INQUOTE_D)) )
			{
				nR = (int)(pDoc - token.szDoc);
				if ( nEndLen == 1 )
				{
					szFindEnd = NULL;
					if ( nNodeType == MNT_ELEMENT && cDminus1 == _T('/') )
					{
						if ( (! cDminus2) || _tcschr(_T(" \t\n\r\'\""),(_TCHAR)cDminus2) )
							node.nFlags |= MNF_EMPTY;
					}
				}
				else if ( nR > nEndLen )
				{
					// Test for end of PI or comment
					LPCTSTR pEnd = pDoc - nEndLen + 1;
					LPCTSTR pFindEnd = szFindEnd;
					int nLen = nEndLen;
					while ( --nLen && *pEnd++ == *pFindEnd++ );
					if ( nLen == 0 )
						szFindEnd = NULL;
				}
				if ( ! szFindEnd && ! (nParseFlags & PD_DOCTYPE) )
					break;
			}
			else if ( cD == _T('<') && (nNodeType == MNT_TEXT || nNodeType == -1) )
			{
				nR = (int)(pDoc - token.szDoc) - 1;
				break;
			}
			else if ( nNodeType & (MNT_ELEMENT|MNT_DOCUMENT_TYPE) )
			{
				if ( cD == _T('\"') && ! (nParseFlags&PD_INQUOTE_S) )
					nParseFlags ^= PD_INQUOTE_D;
				else if ( cD == _T('\'') && ! (nParseFlags&PD_INQUOTE_D) )
					nParseFlags ^= PD_INQUOTE_S;
				if ( nNodeType == MNT_ELEMENT )
				{
					cDminus2 = cDminus1;
					cDminus1 = cD;
				}
			}
		}
		else if ( nParseFlags )
		{
			if ( nParseFlags & PD_TEXTORWS )
			{
				if ( cD == _T('<') )
				{
					nR = (int)(pDoc - token.szDoc) - 1;
					nNodeType = MNT_WHITESPACE;
					break;
				}
				else if ( ! _tcschr(_T(" \t\n\r"),(_TCHAR)cD) )
				{
					nParseFlags ^= PD_TEXTORWS;
					FINDNODETYPE( _T("<"), MNT_TEXT, 0 )
				}
			}
			else if ( nParseFlags & PD_OPENTAG )
			{
				nParseFlags ^= PD_OPENTAG;
				if ( cD > 0x60 || ( cD > 0x40 && cD < 0x5b ) || cD == 0x5f || cD == 0x3a )
					FINDNODETYPE( _T(">"), MNT_ELEMENT, 1 )
				else if ( cD == _T('/') )
					FINDNODETYPE( _T(">"), 0, 2 )
				else if ( cD == _T('!') )
					nParseFlags |= PD_BANG;
				else if ( cD == _T('?') )
					FINDNODETYPE( _T("?>"), MNT_PROCESSING_INSTRUCTION, 2 )
				else
					FINDNODEBAD( _T("tag name character") )
			}
			else if ( nParseFlags & PD_BANG )
			{
				nParseFlags ^= PD_BANG;
				if ( cD == _T('-') )
					nParseFlags |= PD_DASH;
				else if ( cD == _T('[') && !(nParseFlags & PD_DOCTYPE) )
					nParseFlags |= PD_BRACKET;
				else if ( cD == _T('D') && !(nParseFlags & PD_DOCTYPE) )
					nParseFlags |= PD_DOCTYPE;
				else if ( _tcschr(_T("EAN"),(_TCHAR)cD) ) // <!ELEMENT ATTLIST ENTITY NOTATION
					FINDNODETYPE( _T(">"), MNT_DOCUMENT_TYPE, 0 )
				else
					FINDNODEBAD( _T("! tag") )
			}
			else if ( nParseFlags & PD_DASH )
			{
				nParseFlags ^= PD_DASH;
				if ( cD == _T('-') )
					FINDNODETYPE( _T("-->"), MNT_COMMENT, 0 )
				else
					FINDNODEBAD( _T("comment tag") )
			}
			else if ( nParseFlags & PD_BRACKET )
			{
				nParseFlags ^= PD_BRACKET;
				if ( cD == _T('C') )
					FINDNODETYPE( _T("]]>"), MNT_CDATA_SECTION, 0 )
				else
					FINDNODEBAD( _T("tag") )
			}
			else if ( nParseFlags & PD_DOCTYPE )
			{
				if ( cD == _T('<') )
					nParseFlags |= PD_OPENTAG;
				else if ( cD == _T('>') )
				{
					nR = (int)(pDoc - token.szDoc);
					nNodeType = MNT_DOCUMENT_TYPE;
					break;
				}
			}
		}
		else if ( cD == _T('<') )
		{
			nParseFlags |= PD_OPENTAG;
		}
		else
		{
			nNodeType = MNT_WHITESPACE;
			if ( _tcschr(_T(" \t\n\r"),(_TCHAR)cD) )
				nParseFlags |= PD_TEXTORWS;
			else
				FINDNODETYPE( _T("<"), MNT_TEXT, 0 )
		}
		pDoc += _tclen( pDoc );
	}
	token.nNext = nR + 1;
	node.nLength = token.nNext - node.nStart;
	node.nNodeType = nNodeType;
	return nNodeType;
}

CString CMarkup::x_GetPath( int iPos ) const
{
	CString strPath;
	while ( iPos )
	{
		CString strTagName = x_GetTagName( iPos );
		int iPosParent = m_aPos[iPos].iElemParent;
		int iPosSib = 0;
		int nCount = 0;
		while ( iPosSib != iPos )
		{
			iPosSib = x_FindElem( iPosParent, iPosSib, strTagName );
			++nCount;
		}
		if ( nCount > 1 )
		{
			_TCHAR szPred[25];
			_stprintf( szPred, _T("[%d]"), nCount );
			strPath = _T("/") + strTagName + szPred + strPath;
		}
		else
			strPath = _T("/") + strTagName + strPath;
		iPos = iPosParent;
	}
	return strPath;
}

CString CMarkup::x_GetTagName( int iPos ) const
{
	// Return the tag name at specified element
	TokenPos token( m_strDoc, m_nFlags );
	token.nNext = m_aPos[iPos].nStart + 1;
	if ( ! iPos || ! x_FindName( token ) )
		return _T("");

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkup::x_FindAttrib( CMarkup::TokenPos& token, LPCTSTR szAttrib, int n/*=0*/ )
{
	// Return true if found, otherwise false and token.nNext is new insertion point
	// If szAttrib is NULL find attrib n and leave token at attrib name
	// If szAttrib is given, find matching attrib and leave token at value
	// support non-well-formed attributes e.g. href=/advanced_search?hl=en, nowrap
	// token also holds start and length of preceeding whitespace to support remove
	//
	int nPreSpaceStart;
	int nPreSpaceLength;
	int nChar;
	_TCHAR cFirstChar;
	LPCTSTR szDoc = token.szDoc;
	int nAttrib = -1; // starts at tag name
	int nFoundAttribNameR = 0;
	bool bAfterEqual = false;
	while ( 1 )
	{
		// Starting at token.nNext, bypass whitespace and find the next token
		nChar = token.nNext;
		nPreSpaceStart = nChar;
		if ( ! x_FindAny(szDoc,nChar) )
			break;
		nPreSpaceLength = nChar - nPreSpaceStart;

		// Is it an opening quote?
		cFirstChar = szDoc[nChar];
		if ( cFirstChar == _T('\"') || cFirstChar == _T('\'') )
		{
			token.nTokenFlags |= MNF_QUOTED;

			// Move past opening quote
			++nChar;
			token.nL = nChar;

			// Look for closing quote
			while ( szDoc[nChar] && szDoc[nChar] != cFirstChar )
				nChar += (int)_tclen( &szDoc[nChar] );

			// Set right to before closing quote
			token.nR = nChar - 1;

			// Set nChar past closing quote unless at end of document
			if ( szDoc[nChar] )
				++nChar;
		}
		else
		{
			token.nTokenFlags &= ~MNF_QUOTED;

			// Go until special char or whitespace
			token.nL = nChar;
			if ( bAfterEqual )
			{
				while ( szDoc[nChar] && ! _tcschr(_T(" \t\n\r>"),szDoc[nChar]) )
					nChar += (int)_tclen( &szDoc[nChar] );
			}
			else
			{
				while ( szDoc[nChar] && ! _tcschr(_T("= \t\n\r>/?"),szDoc[nChar]) )
					nChar += (int)_tclen( &szDoc[nChar] );
			}

			// Adjust end position if it is one special char
			if ( nChar == token.nL )
				++nChar; // it is a special char
			token.nR = nChar - 1;
		}

		// nNext points to one past last char of token
		token.nNext = nChar;

		if ( ! bAfterEqual && ! (token.nTokenFlags&MNF_QUOTED) )
		{
			// Is it an equal sign?
			_TCHAR cChar = szDoc[token.nL];
			if ( cChar == _T('=') )
			{
				bAfterEqual = true;
				continue;
			}

			// Is it the right angle bracket?
			if ( cChar == _T('>') || cChar == _T('/') || cChar == _T('?') )
			{
				token.nNext = nPreSpaceStart;
				break; // attrib not found
			}

			if ( nFoundAttribNameR )
				break;

			// Attribute name
			if ( nAttrib != -1 )
			{
				if ( ! szAttrib )
				{
					if ( nAttrib == n )
						return true; // found by number
				}
				else if ( token.Match(szAttrib) )
				{
					// Matched attrib name, go forward to value
					nFoundAttribNameR = token.nR;
					token.nPreSpaceStart = nPreSpaceStart;
					token.nPreSpaceLength = nPreSpaceLength;
				}
			}
			++nAttrib;
		}
		else if ( nFoundAttribNameR )
			break;
		bAfterEqual = false;
	}

	if ( nFoundAttribNameR )
	{
		if ( ! bAfterEqual )
		{
			// when attribute has no value the value is the attribute name
			token.nL = token.nPreSpaceStart + token.nPreSpaceLength;
			token.nR = nFoundAttribNameR;
			token.nNext = nFoundAttribNameR + 1;
		}
		return true; // found by name
	}
	return false; // not found
}

CString CMarkup::x_GetAttrib( int iPos, LPCTSTR szAttrib ) const
{
	// Return the value of the attrib
	TokenPos token( m_strDoc, m_nFlags );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStart + 1;
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return _T("");

	if ( szAttrib && x_FindAttrib( token, szAttrib ) )
		return UnescapeText( &token.szDoc[token.nL], token.Length() );
	return _T("");
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, int nValue )
{
	// Convert integer to string
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_SetAttrib( iPos, szAttrib, szVal );
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, LPCTSTR szValue )
{
	// Set attribute in iPos element
	TokenPos token( m_strDoc, m_nFlags );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStart + 1;
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return false;

	// Create insertion text depending on whether attribute already exists
	// Decision: for empty value leaving attrib="" instead of removing attrib
	int nReplace = 0;
	int nInsertAt;
	CString strInsert;
	strInsert += x_ATTRIBQUOTE;
	strInsert += EscapeText( szValue, MNF_ESCAPEQUOTES );
	strInsert += x_ATTRIBQUOTE;
	if ( x_FindAttrib( token, szAttrib ) )
	{
		// Replace value
		nInsertAt = token.nL - ((token.nTokenFlags&MNF_QUOTED)?1:0);
		nReplace = token.Length() + ((token.nTokenFlags&MNF_QUOTED)?2:0);
	}
	else
	{
		// Insert string name value pair
		CString strFormat;
		strFormat = _T(" ");
		strFormat += szAttrib;
		strFormat += _T("=");
		strFormat += strInsert;
		strInsert = strFormat;
		nInsertAt = token.nNext;
	}

	x_DocChange( nInsertAt, nReplace, strInsert );
	int nAdjust = strInsert.GetLength() - nReplace;
	if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
	{
		x_AdjustForNode( m_iPosParent, m_iPos, nAdjust );
		m_nNodeLength += nAdjust;
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	m_aPos[iPos].AdjustStartTagLen( nAdjust );
	m_aPos[iPos].nLength += nAdjust;
	x_Adjust( iPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}


bool CMarkup::x_CreateNode( CString& strNode, int nNodeType, LPCTSTR szText )
{
	// Set strNode based on nNodeType and szData
	// Return false if szData would jeopardize well-formed document
	//
	switch ( nNodeType )
	{
	case MNT_PROCESSING_INSTRUCTION:
		strNode = "<?";
		strNode += szText;
		strNode += "?>";
		break;
	case MNT_COMMENT:
		strNode = "<!--";
		strNode += szText;
		strNode += "-->";
		break;
	case MNT_ELEMENT:
		strNode = "<";
		strNode += szText;
		strNode += "/>";
		break;
	case MNT_TEXT:
	case MNT_WHITESPACE:
		strNode = EscapeText( szText );
		break;
	case MNT_DOCUMENT_TYPE:
		strNode = szText;
		break;
	case MNT_LONE_END_TAG:
		return false;
	case MNT_CDATA_SECTION:
		if ( _tcsstr(szText,_T("]]>")) != NULL )
			return false;
		strNode = "<![CDATA[";
		strNode += szText;
		strNode += "]]>";
		break;
	}
	return true;
}

CString CMarkup::x_EncodeCDATASection( LPCTSTR szData )
{
	// Split CDATA Sections if there are any end delimiters
	CString strData = _T("<![CDATA[");
	LPCTSTR pszNextStart = szData;
	LPCTSTR pszEnd = _tcsstr( szData, _T("]]>") );
	while ( pszEnd )
	{
		strData += CString( pszNextStart, (int)(pszEnd - pszNextStart) );
		strData += _T("]]]]><![CDATA[>");
		pszNextStart = pszEnd + 3;
		pszEnd = _tcsstr( pszNextStart, _T("]]>") );
	}
	strData += pszNextStart;
	strData += _T("]]>");
	return strData;
}

bool CMarkup::x_SetData( int iPos, int nValue )
{
	// Convert integer to string
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_SetData( iPos, szVal, 0 );
}

bool CMarkup::x_SetData( int iPos, LPCTSTR szData, int nFlags )
{
	// Set data at specified position
	// if nFlags==1, set content of element to a CDATA Section
	CString strInsert;

	if ( iPos == m_iPos && m_nNodeLength )
	{
		// Not an element
		if ( ! x_CreateNode(strInsert, m_nNodeType, szData) )
			return false;
		x_DocChange( m_nNodeOffset, m_nNodeLength, strInsert );
		x_AdjustForNode( m_iPosParent, iPos, strInsert.GetLength() - m_nNodeLength );
		m_nNodeLength = strInsert.GetLength();
		MARKUP_SETDEBUGSTATE;
		return true;
	}

	// Set data in iPos element
	if ( ! iPos || m_aPos[iPos].iElemChild )
		return false;

	// Build strInsert from szData based on nFlags
	if ( nFlags & MNF_WITHCDATA )
		strInsert = x_EncodeCDATASection( szData );
	else
		strInsert = EscapeText( szData, nFlags );

	// Insert
	NodePos node( MNF_WITHNOLINES|MNF_REPLACE );
	node.strMeta = strInsert;
	int iPosBefore = 0;
	int nReplace = x_InsertNew( iPos, iPosBefore, node );
	int nAdjust = node.strMeta.GetLength() - nReplace;
	x_Adjust( iPos, nAdjust );
	m_aPos[iPos].nLength += nAdjust;
	if ( m_aPos[iPos].nFlags & MNF_ILLDATA )
		m_aPos[iPos].nFlags &= ~MNF_ILLDATA;
	MARKUP_SETDEBUGSTATE;
	return true;
}

CString CMarkup::x_GetData( int iPos ) const
{
	if ( iPos == m_iPos && m_nNodeLength )
	{
		if ( m_nNodeType == MNT_COMMENT )
			return m_strDoc.Mid( m_nNodeOffset+4, m_nNodeLength-7 );
		else if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
			return m_strDoc.Mid( m_nNodeOffset+2, m_nNodeLength-4 );
		else if ( m_nNodeType == MNT_CDATA_SECTION )
			return m_strDoc.Mid( m_nNodeOffset+9, m_nNodeLength-12 );
		else if ( m_nNodeType == MNT_TEXT )
			return UnescapeText( &((LPCTSTR)m_strDoc)[m_nNodeOffset], m_nNodeLength );
		else if ( m_nNodeType == MNT_LONE_END_TAG )
			return m_strDoc.Mid( m_nNodeOffset+2, m_nNodeLength-3 );
		else
			return m_strDoc.Mid( m_nNodeOffset, m_nNodeLength );
	}

	// Return a string representing data between start and end tag
	// Return empty string if there are any children elements
	CString strData;
	if ( ! m_aPos[iPos].iElemChild && ! m_aPos[iPos].IsEmptyElement() )
	{
		// Quick scan for any tags inside content
		int nContentLen = m_aPos[iPos].ContentLen();
		int nStartContent = m_aPos[iPos].StartContent();
		LPCTSTR pszContent = &((LPCTSTR)m_strDoc)[nStartContent];
		LPCTSTR pszTag = _tcschr( pszContent, _T('<') );
		if ( pszTag && ((int)(pszTag-pszContent) < nContentLen) )
		{
			// Concatenate all CDATA Sections and text nodes, ignore other nodes
			TokenPos token( m_strDoc, m_nFlags );
			token.nNext = nStartContent;
			NodePos node;
			while ( token.nNext < nStartContent + nContentLen )
			{
				x_ParseNode( token, node );
				if ( node.nNodeType == MNT_TEXT )
					strData += UnescapeText( &token.szDoc[node.nStart], node.nLength );
				else if ( node.nNodeType == MNT_CDATA_SECTION )
					strData += m_strDoc.Mid( node.nStart+9, node.nLength-12 );
			}
		}
		else // no tags
			strData = UnescapeText( &((LPCTSTR)m_strDoc)[nStartContent], nContentLen );
	}
	return strData;
}

CString CMarkup::x_GetElemContent( int iPos ) const
{
	if ( iPos && m_aPos[iPos].ContentLen() )
		return m_strDoc.Mid( m_aPos[iPos].StartContent(), m_aPos[iPos].ContentLen() );
	return _T("");
}

bool CMarkup::x_SetElemContent( LPCTSTR szContent )
{
	// Set data in iPos element only
	if ( ! m_iPos )
		return false;

	if ( m_nNodeLength )
		return false; // not an element

	// Unlink all children
	int iPos = m_iPos;
	int iPosChild = m_aPos[iPos].iElemChild;
	bool bHadChild = (iPosChild != 0);
	while ( iPosChild )
		iPosChild = x_ReleaseSubDoc( iPosChild );
	if ( bHadChild )
		x_CheckSavedPos();

	// Parse content
	bool bWellFormed = true;
	TokenPos token( szContent, m_nFlags );
	int iPosVirtual = x_GetFreePos();
	m_aPos[iPosVirtual].ClearVirtualParent();
	m_aPos[iPosVirtual].SetLevel( m_aPos[iPos].Level() + 1 );
	iPosChild = x_ParseElem( iPosVirtual, token );
	if ( m_aPos[iPosVirtual].nFlags & MNF_ILLFORMED )
		bWellFormed = false;
	m_aPos[iPos].nFlags = (m_aPos[iPos].nFlags & ~MNF_ILLDATA) | (m_aPos[iPosVirtual].nFlags & MNF_ILLDATA);

	// Prepare insert and adjust offsets
	NodePos node( MNF_WITHNOLINES|MNF_REPLACE );
	node.strMeta = szContent;
	int iPosBefore = 0;
	int nReplace = x_InsertNew( iPos, iPosBefore, node );
	
	// Adjust and link in the inserted elements
	x_Adjust( iPosChild, node.nStart );
	m_aPos[iPosChild].nStart += node.nStart;
	m_aPos[iPos].iElemChild = iPosChild;
	while ( iPosChild )
	{
		m_aPos[iPosChild].iElemParent = iPos;
		iPosChild = m_aPos[iPosChild].iElemNext;
	}
	x_ReleasePos( iPosVirtual );

	int nAdjust = node.strMeta.GetLength() - nReplace;
	x_Adjust( iPos, nAdjust, true );
	m_aPos[iPos].nLength += nAdjust;

	x_SetPos( m_iPosParent, m_iPos, 0 );
	return bWellFormed;
}

void CMarkup::x_DocChange( int nLeft, int nReplace, const CString& strInsert )
{
	// Insert strInsert int m_strDoc at nLeft replacing nReplace chars
	// Do this with only one buffer reallocation if it grows
	//
	int nDocLength = m_strDoc.GetLength();
	int nInsLength = strInsert.GetLength();
	int nNewLength = nInsLength + nDocLength - nReplace;

	// When creating a document, reduce reallocs by reserving string space
	// Allow for 1.5 times the current allocation
	int nBufferLen = nNewLength;
	int nAllocLen = ((CStringData*)((LPCTSTR)m_strDoc)-1)->nAllocLength;
	if ( nNewLength > nAllocLen )
	{
		nBufferLen += nBufferLen/2 + 128;
		if ( nBufferLen < nNewLength )
			nBufferLen = nNewLength;
	}
	_TCHAR* pDoc = m_strDoc.GetBuffer( nBufferLen );

	// Move part of old doc that goes after insert, then copy insert into it
	if ( nLeft+nReplace < nDocLength )
		memmove( &pDoc[nLeft+nInsLength], &pDoc[nLeft+nReplace], (nDocLength-nLeft-nReplace)*sizeof(_TCHAR) );
	memcpy( &pDoc[nLeft], strInsert, nInsLength*sizeof(_TCHAR) );
	m_strDoc.ReleaseBuffer( nNewLength );

}

void CMarkup::x_Adjust( int iPos, int nShift, bool bAfterPos /*=false*/ )
{
	// Loop through affected elements and adjust indexes
	// Algorithm:
	// 1. update children unless bAfterPos
	//    (if no children or bAfterPos is true, length of iPos not affected)
	// 2. update starts of next siblings and their children
	// 3. go up until there is a next sibling of a parent and update starts
	// 4. step 2
	int iPosTop = m_aPos[iPos].iElemParent;
	bool bPosFirst = bAfterPos; // mark as first to skip its children

	// Stop when we've reached the virtual parent (which has no tags)
	while ( m_aPos[iPos].StartTagLen() )
	{
		// Were we at containing parent of affected position?
		bool bPosTop = false;
		if ( iPos == iPosTop )
		{
			// Move iPosTop up one towards root
			iPosTop = m_aPos[iPos].iElemParent;
			bPosTop = true;
		}

		// Traverse to the next update position
		if ( ! bPosTop && ! bPosFirst && m_aPos[iPos].iElemChild )
		{
			// Depth first
			iPos = m_aPos[iPos].iElemChild;
		}
		else if ( m_aPos[iPos].iElemNext )
		{
			iPos = m_aPos[iPos].iElemNext;
		}
		else
		{
			// Look for next sibling of a parent of iPos
			// When going back up, parents have already been done except iPosTop
			while ( 1 )
			{
				iPos = m_aPos[iPos].iElemParent;
				if ( iPos == iPosTop )
					break;
				if ( m_aPos[iPos].iElemNext )
				{
					iPos = m_aPos[iPos].iElemNext;
					break;
				}
			}
		}
		bPosFirst = false;

		// Shift indexes at iPos
		if ( iPos != iPosTop )
			m_aPos[iPos].nStart += nShift;
		else
			m_aPos[iPos].nLength += nShift;
	}
}

int CMarkup::x_InsertNew( int iPosParent, int& iPosRel, CMarkup::NodePos& node )
{
	// Parent empty tag or tags with no content?
	bool bEmptyParentTag = iPosParent && m_aPos[iPosParent].IsEmptyElement();
	bool bNoContentParentTags = iPosParent && ! m_aPos[iPosParent].ContentLen();
	if ( node.nLength )
	{
		// Located at a non-element node
		if ( ! (node.nFlags & MNF_INSERT) )
			node.nStart += node.nLength;
	}
	else if ( iPosRel )
	{
		// Located at an element
		node.nStart = m_aPos[iPosRel].nStart;
		if ( ! (node.nFlags & MNF_INSERT) ) // follow iPosRel
			node.nStart += m_aPos[iPosRel].nLength;
	}
	else if ( bEmptyParentTag )
	{
		// Parent has no separate end tag, so split empty element
		if ( m_aPos[iPosParent].nFlags & MNF_NONENDED )
			node.nStart = m_aPos[iPosParent].StartContent();
		else
			node.nStart = m_aPos[iPosParent].StartContent() - 1;
	}
	else
	{
		if ( node.nFlags & (MNF_INSERT|MNF_REPLACE) )
			node.nStart = m_aPos[iPosParent].StartContent();
		else // before end tag
			node.nStart = m_aPos[iPosParent].StartAfter() - m_aPos[iPosParent].EndTagLen();
	}

	// Go up to start of next node, unless its splitting an empty element
	if ( ! (node.nFlags&(MNF_WITHNOLINES|MNF_REPLACE)) && ! bEmptyParentTag )
	{
		LPCTSTR szDoc = (LPCTSTR)m_strDoc;
		int nChar = node.nStart;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
			node.nStart = nChar;
	}

	// Is insert relative to element position? (i.e. not other kind of node)
	if ( ! node.nLength )
	{
		// Modify iPosRel to reflect position before
		if ( iPosRel )
		{
			if ( node.nFlags & MNF_INSERT )
			{
				if ( ! (m_aPos[iPosRel].nFlags & MNF_FIRST) )
					iPosRel = m_aPos[iPosRel].iElemPrev;
				else
					iPosRel = 0;
			}
		}
		else if ( ! (node.nFlags & MNF_INSERT) )
		{
			// If parent has a child, add after last child
			if ( m_aPos[iPosParent].iElemChild )
				iPosRel = m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev;
		}
	}

	// Get node length (used only by x_AddNode)
	node.nLength = node.strMeta.GetLength();

	// Prepare end of lines
	if ( (! (node.nFlags & MNF_WITHNOLINES)) && (bEmptyParentTag || bNoContentParentTags) )
		node.nStart += x_EOLLEN;
	if ( ! (node.nFlags & MNF_WITHNOLINES) )
		node.strMeta += x_EOL;

	// Calculate insert offset and replace length
	int nReplace = 0;
	int nInsertAt = node.nStart;
	if ( bEmptyParentTag )
	{
		CString strTagName = x_GetTagName( iPosParent );
		CString strFormat;
		if ( node.nFlags & MNF_WITHNOLINES )
			strFormat = _T(">");
		else
			strFormat = _T(">") x_EOL;
		strFormat += node.strMeta;
		strFormat += _T("</");
		strFormat += strTagName;
		node.strMeta = strFormat;
		if ( m_aPos[iPosParent].nFlags & MNF_NONENDED )
		{
			nInsertAt = m_aPos[iPosParent].StartAfter() - 1;
			nReplace = 0;
			m_aPos[iPosParent].nFlags ^= MNF_NONENDED;
		}
		else
		{
			nInsertAt = m_aPos[iPosParent].StartAfter() - 2;
			nReplace = 1;
			m_aPos[iPosParent].AdjustStartTagLen( -1 );
		}
		m_aPos[iPosParent].SetEndTagLen( 3 + strTagName.GetLength() );
	}
	else
	{
		if ( node.nFlags & MNF_REPLACE )
		{
			nInsertAt = m_aPos[iPosParent].StartContent();
			nReplace = m_aPos[iPosParent].ContentLen();
		}
		else if ( bNoContentParentTags )
		{
			node.strMeta = x_EOL + node.strMeta;
			nInsertAt = m_aPos[iPosParent].StartContent();
		}
	}
	x_DocChange( nInsertAt, nReplace, node.strMeta );
	return nReplace;
}

bool CMarkup::x_AddElem( LPCTSTR szName, int nValue, int nFlags )
{
	// Convert integer to string
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_AddElem( szName, szVal, nFlags );
}

bool CMarkup::x_AddElem( LPCTSTR szName, LPCTSTR szValue, int nFlags )
{
	if ( nFlags & MNF_CHILD )
	{
		// Adding a child element under main position
		if ( ! m_iPos )
			return false;
	}

	// Locate where to add element relative to current node
	NodePos node( nFlags );
	int iPosParent, iPosBefore;
	if ( nFlags & MNF_CHILD )
	{
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
		node.nStart = m_nNodeOffset;
		node.nLength = m_nNodeLength;
	}

	// Cannot have data in non-ended element
	if ( (nFlags&MNF_WITHNOEND) && szValue && szValue[0] )
		return false;

	// Allocate ElemPos structure for this element
	int iPos = x_GetFreePos();

	// Create string for insert
	// If no szValue is specified, an empty element is created
	// i.e. either <NAME>value</NAME> or <NAME/>
	//
	ElemPos* pElem = &m_aPos[iPos];
	int nLenName = (int)_tcslen(szName);
	if ( ! szValue || ! szValue[0] )
	{
		// <NAME/> empty element
		node.strMeta = _T("<");
		node.strMeta += szName;
		if ( nFlags & MNF_WITHNOEND )
		{
			node.strMeta += _T(">");
			pElem->SetStartTagLen( nLenName + 2 );
			pElem->nLength = nLenName + 2;
		}
		else
		{
			if ( nFlags & MNF_WITHXHTMLSPACE )
			{
				node.strMeta += _T(" />");
				pElem->SetStartTagLen( nLenName + 4 );
				pElem->nLength = nLenName + 4;
			}
			else
			{
				node.strMeta += _T("/>");
				pElem->SetStartTagLen( nLenName + 3 );
				pElem->nLength = nLenName + 3;
			}
		}
		pElem->SetEndTagLen( 0 );
	}
	else
	{
		// <NAME>value</NAME>
		CString strValue;
		if ( nFlags & MNF_WITHCDATA )
			strValue = x_EncodeCDATASection( szValue );
		else
			strValue = EscapeText( szValue, nFlags );
		int nLenValue = strValue.GetLength();
		node.strMeta = _T("<");
		node.strMeta += szName;
		node.strMeta += _T(">");
		node.strMeta += strValue;
		node.strMeta += _T("</");
		node.strMeta += szName;
		node.strMeta += _T(">");
		pElem->SetEndTagLen( nLenName + 3 );
		pElem->nLength = nLenName * 2 + nLenValue + 5;
		pElem->SetStartTagLen( nLenName + 2 );
	}

	// Insert
	int nReplace = x_InsertNew( iPosParent, iPosBefore, node );

	pElem->nStart = node.nStart;
	pElem->iElemChild = 0;
	if ( nFlags & MNF_WITHNOEND )
		pElem->nFlags = MNF_NONENDED;
	else
		pElem->nFlags = 0;
	x_LinkElem( iPosParent, iPosBefore, iPos );

	x_Adjust( iPos, node.strMeta.GetLength() - nReplace );

	if ( nFlags & MNF_CHILD )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else
		x_SetPos( iPosParent, iPos, 0 );
	return true;
}

CString CMarkup::x_GetSubDoc( int iPos ) const
{
	if ( iPos )
	{
		int nStart = m_aPos[iPos].nStart;
		int nNext = nStart + m_aPos[iPos].nLength;
		LPCTSTR szDoc = (LPCTSTR)m_strDoc;
		int nChar = nNext;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
			nNext = nChar;
		return m_strDoc.Mid( nStart, nNext - nStart );
	}
	return _T("");
}

bool CMarkup::x_AddSubDoc( LPCTSTR szSubDoc, int nFlags )
{
	// Add subdocument, parse, and modify positions of affected elements
	//
	NodePos node( nFlags );
	int iPosParent, iPosBefore;
	if ( nFlags & MNF_CHILD )
	{
		// Add a subdocument under main position, before or after child
		if ( ! m_iPos )
			return false;
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		// Add a subdocument under parent position, before or after main
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
		node.nStart = m_nNodeOffset;
		node.nLength = m_nNodeLength;
	}

	// Parse subdocument
	bool bWellFormed = true;
	TokenPos token( szSubDoc, m_nFlags );
	int iPosVirtual = x_GetFreePos();
	m_aPos[iPosVirtual].ClearVirtualParent();
	m_aPos[iPosVirtual].SetLevel( m_aPos[iPosParent].Level() + 1 );
	int iPos = x_ParseElem( iPosVirtual, token );
	if ( (!iPos) || m_aPos[iPosVirtual].nFlags & MNF_ILLFORMED )
		bWellFormed = false;
	if ( m_aPos[iPosVirtual].nFlags & MNF_ILLDATA )
		m_aPos[iPosParent].nFlags |= MNF_ILLDATA;

	// Extract subdocument without leading/trailing nodes
	int nExtractStart = 0;
	int iPosLast = m_aPos[iPos].iElemPrev;
	if ( bWellFormed )
	{
		nExtractStart = m_aPos[iPos].nStart;
		int nExtractLength = m_aPos[iPos].nLength;
		if ( iPos != iPosLast )
		{
			nExtractLength = m_aPos[iPosLast].nStart - nExtractStart + m_aPos[iPosLast].nLength;
			bWellFormed = false; // treat as subdoc here, but return not well-formed
		}
		memcpy( node.strMeta.GetBuffer(nExtractLength+x_EOLLEN), &szSubDoc[nExtractStart], nExtractLength*sizeof(_TCHAR) );
		node.strMeta.ReleaseBuffer( nExtractLength );
	}
	else
	{
		node.strMeta = szSubDoc;
		node.nFlags |= MNF_WITHNOLINES;
	}

	// Insert
	int nReplace = x_InsertNew( iPosParent, iPosBefore, node );

	// Adjust and link in the inserted elements
	// iPosVirtual will stop it from affecting rest of document
	int nAdjust = node.nStart - nExtractStart;
	if ( iPos && nAdjust )
	{
		x_Adjust( iPos, nAdjust );
		m_aPos[iPos].nStart += nAdjust;
	}
	int iPosChild = iPos;
	while ( iPosChild )
	{
		int iPosNext = m_aPos[iPosChild].iElemNext;
		x_LinkElem( iPosParent, iPosBefore, iPosChild );
		iPosBefore = iPosChild;
		iPosChild = iPosNext;
	}
	x_ReleasePos( iPosVirtual );

	// Now adjust remainder of document
	x_Adjust( iPosLast, node.strMeta.GetLength() - nReplace, true );

	// Set position to top element of subdocument
	if ( nFlags & MNF_CHILD )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else // Main
		x_SetPos( m_iPosParent, iPos, 0 );
	return bWellFormed;
}

int CMarkup::x_RemoveElem( int iPos )
{
	// Remove element and all contained elements
	// Return new position
	//
	if ( ! iPos )
		return 0;

	// Determine whether any whitespace up to next tag
	int nAfterEnd = m_aPos[iPos].StartAfter();
	LPCTSTR szDoc = (LPCTSTR)m_strDoc;
	int nChar = nAfterEnd;
	if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
		nAfterEnd = nChar;

	// Remove from document, adjust affected indexes, and unlink
	int nLen = nAfterEnd - m_aPos[iPos].nStart;
	x_DocChange( m_aPos[iPos].nStart, nLen, CString() );
	x_Adjust( iPos, - nLen, true );
	int iPosPrev = x_UnlinkElem( iPos );
	x_CheckSavedPos();
	return iPosPrev;
}

void CMarkup::x_LinkElem( int iPosParent, int iPosBefore, int iPos )
{
	// Link in element, and initialize nFlags, and iElem indexes
	ElemPos* pElem = &m_aPos[iPos];
	pElem->iElemParent = iPosParent;
	if ( iPosBefore )
	{
		// Link in after iPosBefore
		pElem->nFlags &= ~MNF_FIRST;
		pElem->iElemNext = m_aPos[iPosBefore].iElemNext;
		if ( pElem->iElemNext )
			m_aPos[pElem->iElemNext].iElemPrev = iPos;
		else
			m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev = iPos;
		m_aPos[iPosBefore].iElemNext = iPos;
		pElem->iElemPrev = iPosBefore;
	}
	else
	{
		// Link in as first child
		pElem->nFlags |= MNF_FIRST;
		if ( m_aPos[iPosParent].iElemChild )
		{
			pElem->iElemNext = m_aPos[iPosParent].iElemChild;
			pElem->iElemPrev = m_aPos[pElem->iElemNext].iElemPrev;
			m_aPos[pElem->iElemNext].iElemPrev = iPos;
			m_aPos[pElem->iElemNext].nFlags ^= MNF_FIRST;
		}
		else
		{
			pElem->iElemNext = 0;
			pElem->iElemPrev = iPos;
		}
		m_aPos[iPosParent].iElemChild = iPos;
	}
	if ( iPosParent )
		pElem->SetLevel( m_aPos[iPosParent].Level() + 1 );
}

int CMarkup::x_UnlinkElem( int iPos )
{
	// Fix links to remove element and mark as deleted
	// return previous position or zero if none
	ElemPos* pElem = &m_aPos[iPos];

	// Find previous sibling and bypass removed element
	int iPosPrev = 0;
	if ( pElem->nFlags & MNF_FIRST )
	{
		if ( pElem->iElemNext ) // set next as first child
		{
			m_aPos[pElem->iElemParent].iElemChild = pElem->iElemNext;
			m_aPos[pElem->iElemNext].iElemPrev = pElem->iElemPrev;
			m_aPos[pElem->iElemNext].nFlags |= MNF_FIRST;
		}
		else // no children remaining
			m_aPos[pElem->iElemParent].iElemChild = 0;
	}
	else
	{
		iPosPrev = pElem->iElemPrev;
		m_aPos[iPosPrev].iElemNext = pElem->iElemNext;
		if ( pElem->iElemNext )
			m_aPos[pElem->iElemNext].iElemPrev = iPosPrev;
		else
			m_aPos[m_aPos[pElem->iElemParent].iElemChild].iElemPrev = iPosPrev;
	}
	x_ReleaseSubDoc( iPos );
	return iPosPrev;
}

int CMarkup::x_ReleasePos( int iPos )
{
	int iPosNext = m_aPos[iPos].iElemNext;
	m_aPos[iPos].iElemNext = m_iPosDeleted;
	m_aPos[iPos].nFlags = MNF_DELETED;
	m_iPosDeleted = iPos;
	return iPosNext;
}

int CMarkup::x_ReleaseSubDoc( int iPos )
{
	// Mark position structures as deleted by depth first traversal
	// Tricky because iElemNext used in traversal is overwritten for linked list of deleted
	// Return value is what iElemNext was before being overwritten
	//
	int iPosNext = 0, iPosTop = iPos;
	while ( 1 )
	{
		if ( m_aPos[iPos].iElemChild )
			iPos = m_aPos[iPos].iElemChild;
		else
		{
			while ( 1 )
			{
				iPosNext = x_ReleasePos( iPos );
				if ( iPos == iPosTop )
					return iPosNext;
				if ( iPosNext )
					break;
				iPos = m_aPos[iPos].iElemParent;
			}
			iPos = iPosNext;
		}
	}
	return iPosNext;
}

void CMarkup::x_CheckSavedPos(void)
{
	// Remove any saved positions now pointing to deleted elements
	// Must be done as part of element removal before position reassigned
	if ( m_mapSavedPos.pTable )
	{
		for ( int nSlot = 0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
			if ( pSavedPos )
			{
				int nOffset = 0;
				int nSavedPosCount = 0;
				while ( 1 )
				{
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
					{
						int iPos = pSavedPos[nOffset].iPos;
						if ( ! (m_aPos[iPos].nFlags & MNF_DELETED) )
						{
							if ( nSavedPosCount < nOffset )
							{
								pSavedPos[nSavedPosCount] = pSavedPos[nOffset];
								pSavedPos[nSavedPosCount].nSavedPosFlags &= ~SavedPosMap::SPM_LAST;
							}
							++nSavedPosCount;
						}
					}
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
					{
						while ( nSavedPosCount <= nOffset )
							pSavedPos[nSavedPosCount++].nSavedPosFlags &= ~SavedPosMap::SPM_USED;
						break;
					}
					++nOffset;
				}
			}
		}
	}
}

void CMarkup::x_AdjustForNode( int iPosParent, int iPos, int nShift )
{
	// Adjust affected indexes
	bool bAfterPos = true;
	if ( ! iPos )
	{
		// Change happened before or at first element under iPosParent
		// If there are any children of iPosParent, adjust from there
		// otherwise start at parent and adjust from there
		iPos = m_aPos[iPosParent].iElemChild;
		if ( iPos )
		{
			m_aPos[iPos].nStart += nShift;
			bAfterPos = false;
		}
		else
		{
			iPos = iPosParent;
			m_aPos[iPos].nLength += nShift;
		}
	}
	x_Adjust( iPos, nShift, bAfterPos );
}

bool CMarkup::x_AddNode( int nNodeType, LPCTSTR szText, int nFlags )
{
	// Only comments, DTDs, and processing instructions are followed by CRLF
	// Other nodes are usually concerned with mixed content, so no CRLF
	if ( ! (nNodeType & (MNT_PROCESSING_INSTRUCTION|MNT_COMMENT|MNT_DOCUMENT_TYPE)) )
		nFlags |= MNF_WITHNOLINES;

	// Add node of nNodeType after current node position
	NodePos node( nFlags );
	if ( ! x_CreateNode(node.strMeta, nNodeType, szText) )
		return false;

	// Locate where to add node relative to current node
	int iPosBefore = m_iPos;
	int iPosParent = m_iPosParent;
	node.nStart = m_nNodeOffset;
	node.nLength = m_nNodeLength;
	node.nNodeType = nNodeType;

	int nReplace = x_InsertNew( iPosParent, iPosBefore, node );

	// If its a new element, create an ElemPos
	int iPos = iPosBefore;
	if ( nNodeType == MNT_ELEMENT )
	{
		// Set indexes
		iPos = x_GetFreePos();
		ElemPos* pElem = &m_aPos[iPos];
		pElem->nStart = node.nStart;
		pElem->SetStartTagLen( node.nLength );
		pElem->SetEndTagLen( 0 );
		pElem->nLength = node.nLength;
		node.nStart = 0;
		node.nLength = 0;
		pElem->iElemChild = 0;
		pElem->nFlags = 0;
		x_LinkElem( iPosParent, iPosBefore, iPos );
	}

	// Need to adjust element positions after iPos
	x_AdjustForNode( iPosParent, iPos, node.strMeta.GetLength() - nReplace );

	// Set current position
	m_iPos = iPos;
	m_iPosChild = 0;
	m_nNodeOffset = node.nStart;
	m_nNodeLength = node.nLength;
	m_nNodeType = nNodeType;
	MARKUP_SETDEBUGSTATE;
	return true;
}

void CMarkup::x_RemoveNode( int iPosParent, int& iPos, int& nNodeType, int& nNodeOffset, int& nNodeLength )
{
	// Remove node and return new position
	//
	int iPosPrev = iPos;

	// Removing an element?
	if ( nNodeType == MNT_ELEMENT )
	{
		nNodeOffset = m_aPos[iPos].nStart;
		nNodeLength = m_aPos[iPos].nLength;
		iPosPrev = x_UnlinkElem( iPos );
		x_CheckSavedPos();
	}

	// Find previous node type, offset and length
	int nPrevOffset = 0;
	if ( iPosPrev )
		nPrevOffset = m_aPos[iPosPrev].StartAfter();
	else if ( iPosParent )
		nPrevOffset = m_aPos[iPosParent].StartContent();
	TokenPos token( m_strDoc, m_nFlags );
	NodePos node;
	token.nNext = nPrevOffset;
	int nPrevType = 0;
	while ( token.nNext < nNodeOffset )
	{
		nPrevOffset = token.nNext;
		nPrevType = x_ParseNode( token, node );
	}
	int nPrevLength = nNodeOffset - nPrevOffset;
	if ( ! nPrevLength )
	{
		// Previous node is iPosPrev element
		nPrevOffset = 0;
		if ( iPosPrev )
			nPrevType = MNT_ELEMENT;
	}

	// Remove node from document
 	x_DocChange( nNodeOffset, nNodeLength, CString() );
	x_AdjustForNode( iPosParent, iPosPrev, - nNodeLength );

	// Was removed node a lone end tag?
	if ( nNodeType == MNT_LONE_END_TAG )
	{
		// See if we can unset parent MNF_ILLDATA flag
		token.nNext = m_aPos[iPosParent].StartContent();
		int nEndOfContent = token.nNext + m_aPos[iPosParent].ContentLen();
		int iPosChild = m_aPos[iPosParent].iElemChild;
		while ( token.nNext < nEndOfContent )
		{
			if ( x_ParseNode(token,node) <= 0 )
				break;
			if ( node.nNodeType == MNT_ELEMENT )
			{
				token.nNext = m_aPos[iPosChild].StartAfter();
				iPosChild = m_aPos[iPosChild].iElemNext;
			}
		}
		if ( token.nNext == nEndOfContent )
			m_aPos[iPosParent].nFlags &= ~MNF_ILLDATA;
	}

	nNodeType = nPrevType;
	nNodeOffset = nPrevOffset;
	nNodeLength = nPrevLength;
	iPos = iPosPrev;
}

