import os
import re

def Conevrt():
	strVCProjFile = r"ZViewer\ZViewer.vcproj"
	if False == os.path.isfile(strVCProjFile):
		print("[FAILED] Cannot find " + strVCProjFile)
	f = open(strVCProjFile, "r")
	datas = f.read()
	f.close()

	#print(datas)
	strRE = r"<File.*?RelativePath=\"(.*?)\".*?</File>"
	finds = re.findall(strRE, datas, re.I | re.S | re.M)
	print("SRCS\t="),
	for find in finds:
		if find.endswith(".cpp"):
			if False == find.endswith("stdafx.cpp"):
				if find.startswith(".\\"):
					find = find.replace('.\\', "")
					print("ZViewer\\" + find + "\t"),
				elif find.startswith("..\\"):
					find = find.replace("..\\", "")
					print(find + "\t"),

if __name__ == "__main__":
	Conevrt()
