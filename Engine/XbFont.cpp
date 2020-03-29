#include "XBFont.h"


//-----------------------------------------------------------------------------
// Static objects
//-----------------------------------------------------------------------------
DWORD CXBFont::m_dwFontVertexShader = 0L;
DWORD CXBFont::m_dwFontPixelShader  = 0L;

    
    
    
//-----------------------------------------------------------------------------
// Name: CXBFont()
// Desc: Font class constructor.
//-----------------------------------------------------------------------------
CXBFont::CXBFont()
{
    m_pFontTexture       = NULL;

    m_fFontHeight        = 36.0f;
    m_fFontTopPadding    =  0.0f;
    m_fFontBottomPadding =  0.0f;
    m_fFontYAdvance      = 36.0f;

    m_dwNumGlyphs        = 0L;
    m_Glyphs             = NULL;

    m_fCursorX           = 0.0f;
    m_fCursorY           = 0.0f;

    m_fXScaleFactor      = 1.0f;
    m_fYScaleFactor      = 1.0f;
    m_fSlantFactor       = 0.0f;

    m_cMaxGlyph          = 0;
    m_TranslatorTable    = NULL;
    m_bTranslatorTableWasAllocated = FALSE;
    
    m_dwNestedBeginCount = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CXBFont()
// Desc: Font class destructor.
//-----------------------------------------------------------------------------
CXBFont::~CXBFont()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: CreateShaders()
// Desc: Creates a vertex shader for rendering fonts, and a pixel shader for
//       font rendering to ensure NTSC safe colors
//-----------------------------------------------------------------------------
HRESULT CXBFont::CreateShaders()
{
    // Create the vertex shader
    if( 0L == m_dwFontVertexShader )
    {
        // Specify the vertex declaration, used here to manually specify a constant.
        DWORD dwFontVertexDecl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( 0, D3DVSDT_FLOAT4 ),        // Vertex/tex coord combo
            D3DVSD_REG( 3, D3DVSDT_D3DCOLOR ),      // Color
            D3DVSD_END()
        };

        // Microcode for the vertex shader, where input is defined as:
        //    v0.xy = screen space position
        //    v0.zw = tex coords
        //    v3    = diffuse color
        static DWORD dwFontVertexShaderInstructions[] = 
        {
            0x00032078,
            0x00000000, 0x00200015, 0x0836106c, 0x2070c800, // mov oPos.xy, v0.xy
            0x00000000, 0x002000bf, 0x0836106c, 0x2070c848, // mov oT0.xy, v0.zw
            0x00000000, 0x0020061b, 0x0836106c, 0x2070f819  // mov oD0, v3
        };

        // Create the vertex shader
        if( FAILED( D3DDevice::CreateVertexShader( dwFontVertexDecl, 
                                                   dwFontVertexShaderInstructions, 
                                                   &m_dwFontVertexShader,
                                                   D3DUSAGE_PERSISTENTDIFFUSE ) ) )
        return E_FAIL;

    }

    // Create the pixel shader
    if( 0L == m_dwFontPixelShader )
    {
        D3DPIXELSHADERDEF psd;
        ZeroMemory( &psd, sizeof(psd) );
        psd.PSCombinerCount = PS_COMBINERCOUNT( 1, 0 );
        psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D, 0, 0, 0 );

        //------------- Stage 0 -------------
        psd.PSRGBInputs[0]    = PS_COMBINERINPUTS( PS_REGISTER_T0|PS_CHANNEL_RGB,   PS_REGISTER_V0|PS_CHANNEL_RGB,   0, 0 );
        psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_T0|PS_CHANNEL_ALPHA, PS_REGISTER_V0|PS_CHANNEL_ALPHA, 0, 0 );
        psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_R0, 0, 0, PS_COMBINEROUTPUT_AB_MULTIPLY );
        psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_R0, 0, 0, PS_COMBINEROUTPUT_AB_MULTIPLY );

        //------------- Final combiner -------------
        psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_C0, PS_REGISTER_R0, PS_REGISTER_ZERO, PS_REGISTER_C1 );
        psd.PSFinalCombinerInputsEFG  = PS_COMBINERINPUTS( 0, 0, PS_REGISTER_R0|PS_CHANNEL_ALPHA, 0 );

        // Set constants to scale output to an NTSC-safe range
        // NTSC_MAX = 0.8549f => 0x00dadada
        // NTSC_MIN = 0.0625f => 0x00101010
        psd.PSFinalCombinerConstant0 = 0x00dadada;
        psd.PSFinalCombinerConstant1 = 0x00101010;
        psd.PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(0,1,PS_GLOBALFLAGS_NO_TEXMODE_ADJUST);

        // Create the pixel shader, as defined above.
        if( FAILED( D3DDevice::CreatePixelShader( &psd, &m_dwFontPixelShader ) ) )
            return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the font's internal objects (texture and array of glyph info)
//       using the XPR packed resource file
//-----------------------------------------------------------------------------
HRESULT CXBFont::Create( string strFontResourceFileName )
{
	return Create(strFontResourceFileName.c_str());
}


HRESULT CXBFont::Create( const WCHAR* strFontResourceFileName )
{
	char szBuffer[1000];
	wsprintf(szBuffer, "%ls", strFontResourceFileName);

	return Create(szBuffer);
}


HRESULT CXBFont::Create( const CHAR* strFontResourceFileName )
{
	DWORD dwResourceOffset = 0UL;

    // Load the resource for the font
	if( FAILED( m_xprResource.Create( strFontResourceFileName ) ) )
        return E_FAIL;

    return CreateGlyph( m_xprResource.GetTexture( dwResourceOffset ), 
                   m_xprResource.GetData( dwResourceOffset + sizeof(D3DTexture) ) );
}




//-----------------------------------------------------------------------------
// Name: CreateGlyph()
// Desc: Create the font's internal objects (texture and array of glyph info)
//-----------------------------------------------------------------------------
HRESULT CXBFont::CreateGlyph( D3DTexture* pFontTexture, VOID* pFontData )
{
    m_pFontTexture = pFontTexture;

    BYTE* pData = (BYTE*)pFontData;
    DWORD dwResourceType = ((DWORD*)pData)[0];
    DWORD dwResourceSize = ((DWORD*)pData)[1];
    pData += 2*sizeof(DWORD);
    (VOID)dwResourceType; // not used
    (VOID)dwResourceSize; // not used

    DWORD dwFileVersion = *((DWORD*)pData); pData += sizeof(DWORD);

    // Check version of file (to make sure it matches up with the FontMaker tool)
    if( dwFileVersion == 0x00000004 )
    {
        // Note: Version 0x00000004 is now considered to be the "old" way of
        // doing fonts. Support for this version of font files will be going
        // away, and a more recent version of FontMaker should now be used.
        m_fFontHeight    = (FLOAT)*((DWORD*)pData); pData += sizeof(DWORD);
        m_fFontYAdvance  = m_fFontHeight;
        FLOAT fTexWidth  = (FLOAT)*((DWORD*)pData); pData += sizeof(DWORD);
        FLOAT fTexHeight = (FLOAT)*((DWORD*)pData); pData += sizeof(DWORD);
        DWORD dwBPP      = *((DWORD*)pData);        pData += sizeof(DWORD);
        (VOID)dwBPP; // not used

        // Read the low and high char
        WCHAR cLowChar  = *((WCHAR*)pData); pData += sizeof(WCHAR);
        WCHAR cHighChar = *((WCHAR*)pData); pData += sizeof(WCHAR);

        // Read the glyph attributes from the file
        m_Glyphs = (GLYPH_ATTR*)(pData+4);

        m_cMaxGlyph = cHighChar;
        m_TranslatorTable = new SHORT[m_cMaxGlyph+1];
        m_bTranslatorTableWasAllocated = TRUE;

        for( WORD i=0; i<=m_cMaxGlyph; i++ )
        {
            if( i < cLowChar )
                m_TranslatorTable[i] = 0;
            else
            {
                m_TranslatorTable[i] = i-cLowChar;

                GLYPH_ATTR* pGlyph = &m_Glyphs[m_TranslatorTable[i]];

                pGlyph->tu1 *= ( fTexWidth-1) / fTexWidth;
                pGlyph->tv1 *= (fTexHeight-1) / fTexHeight;
                pGlyph->tu2 *= ( fTexWidth-1) / fTexWidth;
                pGlyph->tv2 *= (fTexHeight-1) / fTexHeight;

                pGlyph->wWidth += 1;
            }
        }

        m_fFontHeight = m_fFontHeight + 1;
    }
    else if( dwFileVersion == 0x00000005 )
    {
        // Note: Version 0x00000005 is considered to be the "new" way of
        // doing fonts, using the most recent version of the FontMaker tool.
        m_fFontHeight        = *((FLOAT*)pData); pData += sizeof(FLOAT);
        m_fFontTopPadding    = *((FLOAT*)pData); pData += sizeof(FLOAT);
        m_fFontBottomPadding = *((FLOAT*)pData); pData += sizeof(FLOAT);
        m_fFontYAdvance      = *((FLOAT*)pData); pData += sizeof(FLOAT);

        // Point to the translator string
        m_cMaxGlyph = ((WORD*)pData)[0];
        pData += sizeof(WORD);
        m_TranslatorTable = (SHORT*)pData;
        pData += sizeof(WCHAR)*(m_cMaxGlyph+1);
        m_bTranslatorTableWasAllocated = FALSE;

        // Read the glyph attributes from the file
        m_dwNumGlyphs = ((DWORD*)pData)[0];
        pData += sizeof(DWORD);
        m_Glyphs = (GLYPH_ATTR*)pData;
    }
    else
    {
        OUTPUT_DEBUG_STRING( "XBFont: Incorrect version number on font file!\n" );

        // Make sure we don't leak memory.
        if( m_bTranslatorTableWasAllocated )
            delete[] m_TranslatorTable;

        m_TranslatorTable    = NULL;
        m_bTranslatorTableWasAllocated = FALSE;
        return E_FAIL;
    }
    
    // Creates the vertex shader for rendering fonts, and a pixel shader for
    // font rendering to ensure NTSC safe colors
    if( FAILED( CreateShaders() ) )
        return E_FAIL;

    // Determine whether we should save/restore state
    D3DDEVICE_CREATION_PARAMETERS d3dcp;
    D3DDevice::GetCreationParameters( &d3dcp );
    m_bSaveState = (d3dcp.BehaviorFlags&D3DCREATE_PUREDEVICE) ? FALSE : TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the font object
//-----------------------------------------------------------------------------
HRESULT CXBFont::Destroy()
{
    m_xprResource.Destroy();

    m_pFontTexture       = NULL;

    m_fFontHeight        = 0.0f;
    m_fFontTopPadding    = 0.0f;
    m_fFontBottomPadding = 0.0f;
    m_fFontYAdvance      = 0.0f;

    m_dwNumGlyphs        = 0L;
    m_Glyphs             = NULL;

    m_cMaxGlyph          = 0;
    
    if( m_bTranslatorTableWasAllocated )
        delete[] m_TranslatorTable;

    m_TranslatorTable    = NULL;
    m_bTranslatorTableWasAllocated = FALSE;

    m_dwNestedBeginCount = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetCursorPosition()
// Desc: Sets the cursor position for drawing text
//-----------------------------------------------------------------------------
VOID CXBFont::SetCursorPosition( FLOAT fCursorX, FLOAT fCursorY )
{
    m_fCursorX = floorf( fCursorX );
    m_fCursorY = floorf( fCursorY );
}




//-----------------------------------------------------------------------------
// Name: SetScaleFactors()
// Desc: Sets X and Y scale factor to make rendered text bigger or smaller.
//       Note that since text is pre-anti-aliased and therefore point-filtered,
//       any scale factors besides 1.0f will degrade the quality.
//-----------------------------------------------------------------------------
VOID CXBFont::SetScaleFactors( FLOAT fXScaleFactor, FLOAT fYScaleFactor )
{
    m_fXScaleFactor = fXScaleFactor;
    m_fYScaleFactor = fYScaleFactor;
}




//-----------------------------------------------------------------------------
// Name: SetSlantFactor()
// Desc: Sets the slant factor for rendering slanted text.
//-----------------------------------------------------------------------------
VOID CXBFont::SetSlantFactor( FLOAT fSlantFactor )
{
    m_fSlantFactor = fSlantFactor;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CXBFont::GetTextExtent( const WCHAR* strText, FLOAT* pWidth, 
                                FLOAT* pHeight, BOOL bFirstLineOnly ) const
{
    // Check parameters
    if( NULL==strText || NULL==pWidth || NULL==pHeight )
        return E_INVALIDARG;

    // Set default text extent in output parameters
    (*pWidth)   = 0.0f;
    (*pHeight)  = 0.0f;

    // Initialize counters that keep track of text extent
    FLOAT sx = 0.0f;
    FLOAT sy = m_fFontHeight;

    // Loop through each character and update text extent
    while( *strText )
    {
        WCHAR letter = *strText++;
    
        // Handle newline character
        if( letter == L'\n' )
        {
            if( bFirstLineOnly )
                break;
            sx  = 0.0f;
            sy += m_fFontYAdvance;
        }

        // Translate unprintable characters
        GLYPH_ATTR* pGlyph;
        if( letter > m_cMaxGlyph || m_TranslatorTable[letter] == 0 )
            pGlyph = &m_Glyphs[0];
        else
            pGlyph = &m_Glyphs[m_TranslatorTable[letter]];

        // Get text extent for this character's glyph
        sx += pGlyph->wOffset;
        sx += pGlyph->wAdvance;

        // Store text extent of string in output parameters
        if( sx > (*pWidth) )   (*pWidth)  = sx;
        if( sy > (*pHeight) )  (*pHeight) = sy;
     }

    // Apply the scale factor to the result
    (*pWidth)  *= m_fXScaleFactor;
    (*pHeight) *= m_fYScaleFactor;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextWidth()
// Desc: Returns the width (in pixels) of a text string
//-----------------------------------------------------------------------------
FLOAT CXBFont::GetTextWidth( const WCHAR* strText ) const
{
    FLOAT fTextWidth  = 0.0f;
    FLOAT fTextHeight = 0.0f;

    GetTextExtent( strText, &fTextWidth, &fTextHeight );
    return fTextWidth;
}

FLOAT CXBFont::GetTextHeight( const WCHAR* strText ) const
{
    FLOAT fTextWidth  = 0.0f;
    FLOAT fTextHeight = 0.0f;

    GetTextExtent( strText, &fTextWidth, &fTextHeight );
    return fTextHeight;
}




//-----------------------------------------------------------------------------
// Name: Begin()
// Desc: Prepares the font vertex buffers for rendering.
//-----------------------------------------------------------------------------
HRESULT CXBFont::Begin()
{
    // Set state on the first call
    if( 0 == m_dwNestedBeginCount )
    {
        // Save state
        if( m_bSaveState )
        {
            // Note, we are not saving the texture, vertex, or pixel shader,
            //       since it's not worth the performance. We're more interested
            //       in saving state that would cause hard to find problems.
            D3DDevice::GetRenderState( D3DRS_ALPHABLENDENABLE, &m_dwSavedState[0] );
            D3DDevice::GetRenderState( D3DRS_SRCBLEND,         &m_dwSavedState[1] );
            D3DDevice::GetRenderState( D3DRS_DESTBLEND,        &m_dwSavedState[2] );
            D3DDevice::GetRenderState( D3DRS_ALPHATESTENABLE,  &m_dwSavedState[3] );
            D3DDevice::GetRenderState( D3DRS_ALPHAREF,         &m_dwSavedState[4] );
            D3DDevice::GetRenderState( D3DRS_ALPHAFUNC,        &m_dwSavedState[5] );
            D3DDevice::GetRenderState( D3DRS_FILLMODE,         &m_dwSavedState[6] );
            D3DDevice::GetRenderState( D3DRS_CULLMODE,         &m_dwSavedState[7] );
            D3DDevice::GetRenderState( D3DRS_ZENABLE,          &m_dwSavedState[8] );
            D3DDevice::GetRenderState( D3DRS_STENCILENABLE,    &m_dwSavedState[9] );
            D3DDevice::GetRenderState( D3DRS_EDGEANTIALIAS,    &m_dwSavedState[10] );
            D3DDevice::GetTextureStageState( 0, D3DTSS_MINFILTER, &m_dwSavedState[11] );
            D3DDevice::GetTextureStageState( 0, D3DTSS_MAGFILTER, &m_dwSavedState[12] );
        }

        // Set the necessary render states
        D3DDevice::SetTexture( 0, m_pFontTexture );
        D3DDevice::SetVertexShader( m_dwFontVertexShader );
        D3DDevice::SetPixelShader( m_dwFontPixelShader );
        D3DDevice::SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        D3DDevice::SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
        D3DDevice::SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
        D3DDevice::SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        D3DDevice::SetRenderState( D3DRS_ALPHAREF,         0x08 );
        D3DDevice::SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
        D3DDevice::SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
        D3DDevice::SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
        D3DDevice::SetRenderState( D3DRS_ZENABLE,          FALSE );
        D3DDevice::SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        D3DDevice::SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
        D3DDevice::SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        D3DDevice::SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

        // Begin the drawing of vertices
        D3DDevice::Begin( D3DPT_QUADLIST );
    }

    // Keep track of the nested begin/end calls.
    m_dwNestedBeginCount++;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DrawTextLine()
// Desc: Draws text as textured polygons.
//		 Single line only cropped at iLength pixels
//-----------------------------------------------------------------------------
HRESULT CXBFont::DrawTextLine( const WCHAR* strText, DWORD dwColor, int iLength, int iFormat )
{
	WCHAR wcBuffer[5000];
	swprintf( wcBuffer, L"%ls", strText );

	int iResult = (int)GetTextWidth( wcBuffer );

	while( iResult > iLength )
	{
		// 0 = Infinite
		if( iLength == 0 ) { break; }

		// Space Too Small
		if ( iResult < 5 ) { return E_FAIL; }

		// Check
		int iLetters = wcslen(wcBuffer);
		
		wcBuffer[ iLetters-1 ] = '\0';
		wcBuffer[ iLetters-2 ] = '.';
		wcBuffer[ iLetters-3 ] = '.';
		wcBuffer[ iLetters-4 ] = '.';

		iResult = (int)GetTextWidth( wcBuffer );
	}

	// Centered??
	if( iFormat == XBFONT_CENTER )
	{
		m_fCursorX -= (float)(iResult/2);
		SetCursorPosition( m_fCursorX, m_fCursorY );
	}

	// Right??
	if( iFormat == XBFONT_RIGHT )
	{
		m_fCursorX -= (float)iResult;
		SetCursorPosition( m_fCursorX, m_fCursorY );
	}

	return DrawText(wcBuffer, dwColor);
}


HRESULT CXBFont::DrawTextLine( const CHAR* strText, DWORD dwColor, int iLength, int iFormat )
{
	WCHAR wcBuffer[5000];
	wsprintfW(wcBuffer, L"%S", strText);
	return DrawTextLine(wcBuffer, dwColor, iLength, iFormat);
}


HRESULT CXBFont::DrawTextLine( string strText, DWORD dwColor, int iLength, int iFormat )
{
	WCHAR wcBuffer[5000];
	wsprintfW(wcBuffer, L"%S", strText.c_str());
	return DrawTextLine(wcBuffer, dwColor, iLength, iFormat);
}


//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws text as textured polygons
//-----------------------------------------------------------------------------
HRESULT CXBFont::DrawText( const CHAR* strText, DWORD dwColor )
{
	WCHAR wcBuffer[5000];
	wsprintfW(wcBuffer, L"%S", strText);

	return DrawText(wcBuffer, dwColor);
}


HRESULT CXBFont::DrawText( string strText, DWORD dwColor )
{
	WCHAR wcBuffer[5000];
	wsprintfW(wcBuffer, L"%S", strText.c_str());

	return DrawText(wcBuffer, dwColor);
}


HRESULT CXBFont::DrawText( const WCHAR* strText, DWORD dwColor )
{
    // Set up stuff (i.e. lock the vertex buffer) to prepare for drawing text
    Begin();

    // Set the common color for all the vertices to follow
    D3DDevice::SetVertexDataColor( D3DVSDE_DIFFUSE, dwColor );

	// Text output
	GlyphOut(strText);

    // Call End() to complete the begin/end pair for drawing text
    End();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DrawTextRect()
// Desc: Draws text as textured polygons To A Specified Rectangle.
//	 If Length Goes Path Rectangle Width, The Text Drops To Next line
//	 Until It Reaches The Maximum Height, Then It Is Truncated.
//-----------------------------------------------------------------------------
HRESULT CXBFont::DrawTextRect( const CHAR* strText, DWORD dwColor, FLOAT fMaxPixelWidth, FLOAT fMaxPixelHeight )
{
	WCHAR wcBuffer[5000];
	wsprintfW(wcBuffer, L"%S", strText);

	return DrawTextRect(wcBuffer, dwColor, fMaxPixelWidth, fMaxPixelHeight);
}


HRESULT CXBFont::DrawTextRect( string strText, DWORD dwColor, FLOAT fMaxPixelWidth, FLOAT fMaxPixelHeight )
{
	WCHAR wcBuffer[5000];
	wsprintfW(wcBuffer, L"%S", strText.c_str());

	return DrawTextRect(wcBuffer, dwColor, fMaxPixelWidth, fMaxPixelHeight);
}


HRESULT CXBFont::DrawTextRect( const WCHAR* strText, DWORD dwColor, FLOAT fMaxPixelWidth, FLOAT fMaxPixelHeight )
{
    // Set up stuff (i.e. lock the vertex buffer) to prepare for drawing text
    Begin();

	FLOAT fOriginX = m_fCursorX;
	FLOAT fOriginY = m_fCursorY;
	
	FLOAT fTempY = m_fCursorY;
	FLOAT fTempSize = GetTextHeight(strText);

    // Set the common color for all the vertices to follow
    D3DDevice::SetVertexDataColor( D3DVSDE_DIFFUSE, dwColor );

    // Adjust for padding
    fOriginY -= m_fFontTopPadding;

	// Font Extent
	FLOAT w, h;
	GetTextExtent(strText, &w, &h);

	// If Not Enough Room For Single Line.. Return
	if( h > fMaxPixelHeight )
	{
		End();
		return E_FAIL;
	}

	// If Text Can Fit In Space Without Edit
	if( w < fMaxPixelWidth )
	{
		GlyphOut(strText);

		End();
		return S_OK;
	}

	// Cant Fit Edit It
	else
	{
		char  *p;
		int   iResult = 0;
		int   iRemains = 0;
		bool  bOneNull = false;
		char  szNewOut[5000];
		char  szNewWord[100];
		char  szRemains[5000];
		char  szTempRem[5000];
		WCHAR wcTempOut[5000];
		WCHAR wcTempWord[100];
		WCHAR wcStringBuffer[5000];
		float fOutSize = 0;
		float fWordSize = 0;

		sprintf(szNewOut, "");
		wsprintf(szRemains, "%ls", strText);

		while(true)
		{
			if( (m_fCursorY+fTempSize) > (fMaxPixelHeight+fTempY) ) {  End(); return S_OK; }

			p = strstr(szRemains, " ");
			iRemains = 0;
			iResult = (int)(p - szRemains + 1);
			if (p == NULL) { iResult = 0; }

			for ( int i = (iResult - 1); i >= 0; i-- )
			{
				szNewWord[i] = szRemains[i];
				szNewWord[iResult - 1] = '\0';
			}

			while (szRemains[iRemains] != '\0')
			{
				szTempRem[iRemains] = szRemains[iResult];
				iRemains++;
				iResult++;
			}
			sprintf(szRemains, "%s", szTempRem);
			szTempRem[iRemains] = '\0';

			wsprintfW(wcTempOut, L"%S", szNewOut);
			wsprintfW(wcTempWord, L"%S", szNewWord);

			GetTextExtent(wcTempOut, &fOutSize, &h);
			GetTextExtent(wcTempWord, &fWordSize, &h);

			if ( (int)(fOutSize + fWordSize) < (int)fMaxPixelWidth )
			{
				strcat(szNewOut, szNewWord);
				if (p != NULL) { strcat(szNewOut, " "); }
				sprintf(szNewWord, "");
			}
			else
			{
				wsprintfW(wcStringBuffer, L"%S", szNewOut);

				GlyphOut(wcStringBuffer);

				m_fCursorX = fOriginX;
				m_fCursorY = fOriginY += ((m_fYScaleFactor * m_fFontHeight));
				m_fCursorY -= 2;

				sprintf(szNewOut, "");
				strcat(szNewOut, szNewWord);
				if (p != NULL) { strcat(szNewOut, " "); }
				sprintf(szNewWord, "");
			}

			if (p == NULL)
			{
				wsprintfW(wcTempOut, L"%S", szNewOut);
				wsprintfW(wcTempWord, L"%S", szRemains);

				GetTextExtent(wcTempOut, &fOutSize, &h);
				GetTextExtent(wcTempWord, &fWordSize, &h);

				if ( (int)(fOutSize + fWordSize) < (int)fMaxPixelWidth )
				{
					wsprintfW(wcStringBuffer, L"%S%S", szNewOut, szRemains);
				
					GlyphOut(wcStringBuffer);
				}

				else
				{
					wsprintfW(wcStringBuffer, L"%S", szNewOut);

					GlyphOut(wcStringBuffer);

					m_fCursorX = fOriginX;
					m_fCursorY += ((m_fYScaleFactor * m_fFontHeight));
					m_fCursorY -= 2;

					wsprintfW(wcStringBuffer, L"%S", szRemains);

					GlyphOut(wcStringBuffer);
				}

				break;
			}
		}
	}

    // Call End() to complete the begin/end pair for drawing text
    End();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: End()
// Desc: Called after Begin(), this function triggers the rendering of the
//       vertex buffer contents filled during calls to DrawText().
//-----------------------------------------------------------------------------
HRESULT CXBFont::End()
{
    // Keep track of nested calls to begin/end.
    if( 0L == m_dwNestedBeginCount )
        return E_FAIL;
    if( --m_dwNestedBeginCount > 0 )
        return S_FALSE;

    // End the drawing of vertex data
    D3DDevice::End();

    // Restore state
    if( m_bSaveState )
    {
        D3DDevice::SetTexture( 0, NULL );
        D3DDevice::SetPixelShader( NULL );
        D3DDevice::SetRenderState( D3DRS_ALPHABLENDENABLE, m_dwSavedState[0] );
        D3DDevice::SetRenderState( D3DRS_SRCBLEND,         m_dwSavedState[1] );
        D3DDevice::SetRenderState( D3DRS_DESTBLEND,        m_dwSavedState[2] );
        D3DDevice::SetRenderState( D3DRS_ALPHATESTENABLE,  m_dwSavedState[3] );
        D3DDevice::SetRenderState( D3DRS_ALPHAREF,         m_dwSavedState[4] );
        D3DDevice::SetRenderState( D3DRS_ALPHAFUNC,        m_dwSavedState[5] );
        D3DDevice::SetRenderState( D3DRS_FILLMODE,         m_dwSavedState[6] );
        D3DDevice::SetRenderState( D3DRS_CULLMODE,         m_dwSavedState[7] );
        D3DDevice::SetRenderState( D3DRS_ZENABLE,          m_dwSavedState[8] );
        D3DDevice::SetRenderState( D3DRS_STENCILENABLE,    m_dwSavedState[9] );
        D3DDevice::SetRenderState( D3DRS_EDGEANTIALIAS,    m_dwSavedState[10] );
        D3DDevice::SetTextureStageState( 0, D3DTSS_MINFILTER, m_dwSavedState[11] );
        D3DDevice::SetTextureStageState( 0, D3DTSS_MAGFILTER, m_dwSavedState[12] );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderToTexture()
// Desc: Creates a texture and renders a text string into it. The texture
//       format defaults to a 32-bit linear texture
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE8 CXBFont::CreateTexture( const WCHAR* strText, 
                                           D3DCOLOR dwBackgroundColor, 
                                           D3DCOLOR dwTextColor, 
                                           D3DFORMAT d3dFormat )
{
    // Calculate texture dimensions
    FLOAT fTexWidth;
    FLOAT fTexHeight;
    GetTextExtent( strText, &fTexWidth, &fTexHeight );
    DWORD dwWidth  = (DWORD)fTexWidth;
    DWORD dwHeight = (DWORD)fTexHeight;

    switch( d3dFormat )
    {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
            // For swizzled textures, make sure the dimensions are a power of two
            for( DWORD wmask=1; dwWidth&(dwWidth-1); wmask = (wmask<<1)+1 )
                dwWidth  = ( dwWidth + wmask ) & ~wmask;
            for( DWORD hmask=1; dwHeight&(dwHeight-1); hmask = (hmask<<1)+1 )
                dwHeight = ( dwHeight + hmask ) & ~hmask;

            // Also, enforce a minimum pitch of 64 bytes
            dwWidth = max( 64/XGBytesPerPixelFromFormat(d3dFormat), dwWidth );

            break;

        case D3DFMT_LIN_A8R8G8B8:
        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_LIN_R5G6B5:
        case D3DFMT_LIN_X1R5G5B5:
            // For linear textures, make sure the stride is a multiple of 64 bytes
            dwWidth  = ( dwWidth + 0x1f ) & ~0x1f;
            break;

        default:
            // All other formats are unsupported as render targets
            return NULL;
    }

    // Create the texture
    LPDIRECT3DTEXTURE8 pTexture;
    if( FAILED( D3DDevice::CreateTexture( dwWidth, dwHeight, 1, 0L, d3dFormat, 
                                          D3DPOOL_DEFAULT, &pTexture ) ) )
        return NULL;
    
    // Get the current backbuffer and zbuffer
    LPDIRECT3DSURFACE8 pBackBuffer, pZBuffer;
    D3DDevice::GetRenderTarget( &pBackBuffer );
    D3DDevice::GetDepthStencilSurface( &pZBuffer );

    // Set the new texture as the render target
    LPDIRECT3DSURFACE8 pTextureSurface;
    pTexture->GetSurfaceLevel( 0, &pTextureSurface );
    D3DVIEWPORT8 vp = { 0, 0, dwWidth, dwHeight, 0.0f, 1.0f };
    D3DDevice::SetRenderTarget( pTextureSurface, NULL );
    D3DDevice::SetViewport( &vp );
    D3DDevice::Clear( 0L, NULL, D3DCLEAR_TARGET, dwBackgroundColor, 1.0f, 0L );

    // Render the text
    DrawText( strText, dwTextColor );

    // Restore the render target
    D3DVIEWPORT8 vpBackBuffer = { 0, 0, 640, 480, 0.0f, 1.0f };
    D3DDevice::SetRenderTarget( pBackBuffer, pZBuffer );
    D3DDevice::SetViewport( &vpBackBuffer );
    SAFE_RELEASE( pBackBuffer );
    SAFE_RELEASE( pZBuffer );
    SAFE_RELEASE( pTextureSurface );

    // Return the new texture
    return pTexture;
}

void CXBFont::AddStringToTexture( LPDIRECT3DTEXTURE8 pTexture, float fPositionX, float fPositionY, const char* strString, DWORD dwTextColor, int iMaxLength, bool bClear )
{
	string sTemp = strString;
	return AddStringToTexture( pTexture, fPositionX, fPositionY, sTemp, dwTextColor, iMaxLength, bClear);
}

void CXBFont::AddStringToTexture( LPDIRECT3DTEXTURE8 pTexture, float fPositionX, float fPositionY, string strString, DWORD dwTextColor, int iMaxLength, bool bClear )
{
	// Backup Existing Buffers
	LPDIRECT3DSURFACE8 pBackBuffer, pZBuffer;
    D3DDevice::GetRenderTarget( &pBackBuffer );
    D3DDevice::GetDepthStencilSurface( &pZBuffer );

	// Get Texture Info
	D3DSURFACE_DESC pDesc;
	pTexture->GetLevelDesc( 0, &pDesc );

	// Set The New Render Target..
	LPDIRECT3DSURFACE8 pTextureSurface;
    pTexture->GetSurfaceLevel( 0, &pTextureSurface );
	D3DVIEWPORT8 vp = { 0, 0, pDesc.Width, pDesc.Height, 0.0f, 1.0f };
    D3DDevice::SetRenderTarget( pTextureSurface, NULL );
	if( bClear )
	{
		D3DDevice::Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0 );
	}
    D3DDevice::SetViewport( &vp );


	// Draw Text
	Begin();
	SetCursorPosition( fPositionX, fPositionY );
	if( iMaxLength > 0 )
	{
		DrawTextLine( strString, dwTextColor, iMaxLength );
	}
	else
	{
		DrawText( strString, dwTextColor );
	}
	End();


	// Restore Backed Up Render Target
	D3DVIEWPORT8 vpBackBuffer = { 0, 0, 640, 480, 0.0f, 1.0f };
    D3DDevice::SetRenderTarget( pBackBuffer, pZBuffer );
    D3DDevice::SetViewport( &vpBackBuffer );

	// Clean Up!
    SAFE_RELEASE( pBackBuffer );
    SAFE_RELEASE( pZBuffer );
    SAFE_RELEASE( pTextureSurface );
}

VOID CXBFont::GlyphOut(const WCHAR* strText)
{
	while( *strText )
	{
		WCHAR letter = *strText++;

		// Translate unprintable characters
		GLYPH_ATTR* pGlyph = &m_Glyphs[ (letter<=m_cMaxGlyph) ? m_TranslatorTable[letter] : 0 ];

		FLOAT fOffset  = m_fXScaleFactor * (FLOAT)pGlyph->wOffset;
		FLOAT fAdvance = m_fXScaleFactor * (FLOAT)pGlyph->wAdvance;
		FLOAT fWidth   = m_fXScaleFactor * (FLOAT)pGlyph->wWidth;
		FLOAT fHeight  = m_fYScaleFactor * m_fFontHeight;

		// Setup the screen coordinates
		m_fCursorX += fOffset;
		FLOAT left1  = m_fCursorX;
		FLOAT left2  = left1 + m_fSlantFactor;
		FLOAT right1 = left1 + fWidth;
		FLOAT right2 = left2 + fWidth;
		FLOAT top    = m_fCursorY;
		FLOAT bottom = top + fHeight;
		m_fCursorX += fAdvance;

		// Draw the quad using SetVertexData
		D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, left1,  bottom, pGlyph->tu1, pGlyph->tv2 );
		D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, left2,  top,    pGlyph->tu1, pGlyph->tv1 );
		D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, right2, top,    pGlyph->tu2, pGlyph->tv1 );
		D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, right1, bottom, pGlyph->tu2, pGlyph->tv2 );
	}
}

HRESULT CXBFont::DrawScaledText( FLOAT originx, FLOAT originy, FLOAT scalex, FLOAT scaley, DWORD dwColor, 
								  const WCHAR* strText, DWORD dwFlags, FLOAT fMaxPixelWidth )
{
	HRESULT rv;
	SetScaleFactors( scalex, scaley );
	SetCursorPosition(originx, originy);
	rv=DrawText( strText, dwColor);//, dwFlags, fMaxPixelWidth );
	SetScaleFactors( 1.0f, 1.0f );
	return rv;
}