import re
import os
import ZViewer

def archiveDebugDatas():
	strCurrentVersion = ZViewer.getCurrentVersionFromNSIS()
	if strCurrentVersion == "Unknown":
		print("[FAILED] Cannot get current version from NSIS")
		return 1
	str7ZipPath = r"C:\Program Files\7-Zip\7z.exe" 
	if True == os.path.isfile(str7ZipPath):
		print("Founded 7z")
	else:
		print("[FAILED] There is no 7zip : " + str7ZipPath)
		return 1

	files = [ "ZViewer.exe", "ZViewer.pdb" ]

	for file in files:
		if False == os.path.isfile(file) and False == os.path.isdir(file):
			print("[FAILED] Cannot find : " + file)
			return 1

	# log current dir
	strPathLogFile = "path.txt"
	f = open(strPathLogFile, "w")
	f.write(os.path.abspath("ZViewer.exe"))
	f.close()

	strCmd = "\"" + str7ZipPath + "\" a ZViewerDebugInfo" + strCurrentVersion + ".zip "
	for file in files:
		strCmd = strCmd + file + " "
	strCmd = strCmd + strPathLogFile

	print(strCmd)
	os.system(strCmd)
	os.unlink(strPathLogFile)

if __name__ == "__main__":
	if 1 == archiveDebugDatas():
		print("[FAILED] Archiving debug datas is failed.")
		exit(1)
	else:
		print("[OK] Archiving debug datas is completed.")
