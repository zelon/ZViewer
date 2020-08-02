#include "stdafx.h"

#include "../commonSrc/CommonFunc.h"

using namespace std;

TEST(Basic, GetOnlyFileNameWithoutExt)
{
	tstring strFilename = TEXT("C:\\test\\aaa.bmp");
	tstring strResult = GetOnlyFileNameWithoutExt(strFilename);

	EXPECT_EQ(strResult, TEXT("aaa"));
}

TEST(Basic, toString)
{
	ASSERT_EQ(toString(500), TEXT("500"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
