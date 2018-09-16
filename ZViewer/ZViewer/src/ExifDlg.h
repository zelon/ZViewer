#include "../commonSrc/windowLib/ZWindow.h"
#include "../commonSrc/CommonDefine.h"

class ZImage;

class ExifDlg final : public ZWindow
{
public:
	ExifDlg(){}
	virtual ~ExifDlg(){}

	virtual void SetWndProc();

	void DoResource(HWND hParentHWND);

	void MakeExifMap(ZImage & image);

	void ShowExifMap();

private:
	std::list<TagData> m_exifList;

	tstring convertExifKey(const tstring & strKey) const;
	tstring convertExifValue(const tstring & strKey, tstring & strValue) const;
};

