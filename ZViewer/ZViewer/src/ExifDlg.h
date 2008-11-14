/* ------------------------------------------------------------------------
 *
 * ExifDlg.h
 *
 * 2008.10.27 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include <map>
#include <string>

#include "../commonSrc/windowLib/ZWindow.h"
#include "../commonSrc/ZImage.h"


class ExifDlg : public ZWindow
{
public:
	ExifDlg(){}
	virtual ~ExifDlg(){}

	virtual void SetWndProc();

	void DoResource(HWND hParentHWND);


	void MakeExifMap(ZImage & image);

	void ShowExifMap();

protected:
	std::map < std::string, std::string > m_exifMap;

	tstring convertExifKey(const tstring & strKey) const;
	tstring convertExifValue(const tstring & strKey, tstring & strValue) const;
};

