/*********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include <vector>

#include "CommonFunc.h"
#include "OptionFile.h"
#include "StringToValue.h"

class ZOption
{
	ZOption();

	void _InsertSaveOptionSetting(TCHAR * str, bool * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

	void _InsertSaveOptionSetting(TCHAR * str, int * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

public:
	static ZOption & GetInstance();

	~ZOption()
	{
		SaveToFile();
	}

	/// 湲곕낯 ?듭뀡???뚯씪濡쒕???遺덈윭?⑤떎.
	void LoadOption();

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}

	void SetDontSave() { m_bDontSaveInstance = true; }
	void SetFullScreen(bool bFullScreen) { m_bFullScreen = bFullScreen; }

	int GetMaxCacheMemoryMB() const { return m_iMaximumCacheMemoryMB; }
	void SetMaxCacheMemoryMB(const int iMB) { m_iMaximumCacheMemoryMB = iMB; }

	const bool IsLoopImages() const { return m_bLoopImages; }
	void ToggleLoopImages() { m_bLoopImages = !m_bLoopImages; }

	const bool IsUseDebug() const { return m_bUseDebug;}

	const bool IsBigToSmallStretchImage() const { return m_bBigToSmallStretchImage; }
	void ToggleBigToSmallStretchImage() { m_bBigToSmallStretchImage = !m_bBigToSmallStretchImage; }

	const bool IsSmallToBigStretchImage() const { return m_bSmallToBigStretchImage; }
	void ToggleSmallToBigStretchImage() { m_bSmallToBigStretchImage = !m_bSmallToBigStretchImage; }

	const bool IsUseAutoRotation() const { return m_bUseAutoRotation; }
	void SetAutoRotation(const bool bUseAutoRotation) { m_bUseAutoRotation = bUseAutoRotation; }

	const bool IsUsingPreviewModeInShell() const { return m_bUsePreviewInShell; }
	const bool IsUsingOpenCMDInShell() const { return m_bUseOpenCMDInShell; }
	//-----------------------------------------------------------
	// ?꾨옒???꾨줈洹몃옩 援щ룞 ?꾩뿉 ?꾩떆濡??곗씠???듭뀡?? ?뚯씪????ν븯吏 ?딅뒗??

	//// 洹몃┝???쒖옉 ?꾩튂. ?쇰낯 留뚰솕??寃쎌슦 ?곗륫 ?곷떒遺???쒖옉?섍린 ?꾪빐??..
	bool m_bRightTopFirstDraw;

	/// ???붾㈃??2?μ쓽 洹몃┝??蹂댁뿬以?寃껋씤媛.
	bool m_bTwoInSaveView;

	/// 理쒕? 罹먯떆???대?吏 媛?닔. ?ш린???덈컲諛섑겮 ?쇱そ, ?ㅻⅨ履쎌쑝濡?媛꾨떎.
	int m_iMaxCacheImageNum;

	/// ?щ씪?대뱶 紐⑤뱶?멸?
	bool m_bSlideMode;

	/// ?щ씪?대뱶 紐⑤뱶???쒓컙(mili seconds)
	DWORD m_iSlideModePeriodMiliSeconds;

	DWORD m_dwLastSlidedTime;

	/// Always on Top window
	bool m_bAlwaysOnTop;
protected:

	/// Is Full screen mode?
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// ?꾨옒??吏?띿쟻?쇰줈 ??λ릺???듭뀡?? ?뚯씪????ν븳??

	/// exif ?뺣낫???곕Ⅸ ?먮룞 ?뚯쟾????寃껋씤媛.
	bool m_bUseAutoRotation;

	/// 留덉?留?洹몃┝ ?뚯씪?먯꽌 ?ㅼ쓬?쇰줈 媛硫?泥섏쓬 洹몃┝ ?뚯씪濡?媛?寃껋씤媛?
	bool m_bLoopImages;

	/// ?붾쾭洹??뺣낫瑜??④린??
	bool m_bUseDebug;

	/// ?붾㈃蹂대떎 ??洹몃┝???붾㈃??留욊쾶 異뺤냼??寃껋씤媛
	bool m_bBigToSmallStretchImage;

	/// ?붾㈃蹂대떎 ?묒? 洹몃┝???붾㈃??留욊쾶 ?뺣???寃껋씤媛.
	bool m_bSmallToBigStretchImage;

	/// 罹먯떆?섎뒗 理쒕? 硫붾え由щ웾
	int m_iMaximumCacheMemoryMB;

	tstring m_strOptionFilename;

	/// Shell ?먯꽌 誘몃━ 蹂닿린瑜??ъ슜??寃껋씤媛.
	bool m_bUsePreviewInShell;

	/// Shell ?먯꽌 cmd 李??닿린瑜??ъ슜??寃껋씤媛.
	bool m_bUseOpenCMDInShell;

	/// 湲곕낯?곸씤 ?듭뀡???ㅼ젙?대몦?? ?ㅼ튂 ??泥섏쓬 ?ㅽ뻾?섏뿀??????媛믪쓣 湲곗??쇰줈 zviewer.ini ?뚯씪??留뚮뱾?댁쭊??
	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

	/// ??硫ㅻ쾭?ㅼ? ???대쫫?쇰줈 ??λ릺怨? 遺덈젮???⑤떎?쇨퀬 ?ㅼ젙
	void SetSaveOptions();

	/// ??媛믪씠 false ?대㈃ singleton ???앸궪 ??媛믪쓣 ??ν븯吏 ?딅뒗??ZviewerAgent ??醫낅즺 ?쒖젏???뺥솗???????녾린 ?뚮Ц)
	bool m_bDontSaveInstance;

protected:
	/// ?뚯씪濡쒕????듭뀡???쎌뼱?ㅼ뿬??諛붾??듭뀡???덈굹.
	bool m_bOptionChanged;

	std::vector < StringToValue > m_saveOptions;

};

