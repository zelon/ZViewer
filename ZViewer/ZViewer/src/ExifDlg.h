/* ------------------------------------------------------------------------
 *
 * ExifDlg.h
 *
 * 2008.10.27 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include <map>

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

protected:
	std::map < tstring, tstring > m_exifMap;

};

