import os

def removeUTF8Bom(filename):
	f = open(filename)

	lines = f.readlines()
	f.close()

	f = open(filename, "w")

	bFoundBom = False
	index = 0
	for line in lines:

		if index == 0 and line[:3] == '\xef\xbb\xbf':
			line = line[3:]
			print(filename + " : removed bom code")
			bFoundBom = True
		
		f.write(line)

		index = index + 1
	f.close()

	if bFoundBom == False:
		print("!!!! " + filename + " : cannot find bom code")

if __name__ == "__main__":
	removeUTF8Bom("FreeImage.h")
	removeUTF8Bom("FreeImagePlus.h")

