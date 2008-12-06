import re
import os

def getCurrentVersionFromNSIS():
	
	file = open(r"ZViewer.nsi")
    
	lines = file.readlines()
	file.close()
    
	outputs = []    
	for i in lines:
    	
		m = re.match(r"!define\sPRODUCT_VERSION\s\"(.*)\"", i)
    	
		if m != None:
			"""outputs.append("!define PRODUCT_VERSION \"" + str(major) + "." + str(minor) + "." + str(patch) + str(postfix) + "\"\n")"""
			return (m.group(1))
		else:
			pass
	return "Unknown"

def testmain():
	strCurrentVersion = getCurrentVersionFromNSIS()
	if strCurrentVersion == "Unknown":
		print("[FAILED] Cannot get current version from NSIS")
		return 1
	str7ZipPath = r"C:\Program Files\7-Zip\7z.exe" 
	if True == os.path.isfile(str7ZipPath):
		print("Founded 7z")
	else:
		print("[FAILED] There is no 7zip : " + str7ZipPath)
		return 1

	files = ["language", "_Zviewer.png", "FreeImage.dll", "FreeImagePlus.dll", "License.txt", "LoadError.png", "ReadMe.txt", "ZViewer.exe", "ZViewerAgent.dll", "ZViewerIcons.dll"]

	for file in files:
		if False == os.path.isfile(file) and False == os.path.isdir(file):
			print("[FAILED] Cannot find : " + file)
			return 1

	strCmd = "\"" + str7ZipPath + "\" a ZViewerPortable" + strCurrentVersion + ".zip "
	for file in files:
		strCmd = strCmd + file + " "

	print strCmd
	os.system(strCmd)

if __name__ == "__main__":
	if 1 == testmain():
		exit(1)
