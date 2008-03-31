"""
 이 파일은 각 언어에 맞는 언어 파일을 테스트하는 파일입니다.
"""
import os
import glob

def testmain():
	files = glob.glob("output/language/*.txt")

	""" 모든 텍스트 파일을 열면서 key(=왼쪽부분) 의 모든 종류를 모으고
		다시 모든 파일을 돌면서 key 가 없는 파일을 찾음 """
	for file in files:

		f = open(file)

		contents = f.readlines()

		for content in contents:
			print "|" + content + "|"
		print file

if __name__ == "__main__":
	testmain()
