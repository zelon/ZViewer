import os
import shutil
import ZViewer

strFinalDes = r"..\_release"

def prepare():

	if False == os.path.isdir(strFinalDes):
		os.mkdir(strFinalDes)

	strVer = ZViewer.getCurrentVersionFromNSIS()
	files = [ "ZViewerPortable" + strVer + ".zip",
				"ZViewerSetup" + strVer + ".exe",
				"ZViewerDebugInfo" + strVer + ".zip" ]

	for file in files:
		if False == os.path.isfile(file):
			print("[FAILED] Cannot find file : " + file)
			return False

	for file in files:
		if False == shutil.move(file, strFinalDes + "\\" + file):
			print("[FAILED] Cannot move file : " + file)
			return False

if __name__ == "__main__":
	if False == prepare():
		exit(1)
	else:
		print("[OK] Release is prepared : " + os.path.abspath(strFinalDes))
