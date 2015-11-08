#include "pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameUnitTests
{
	TEST_CLASS(GameTest)
	{
	public:

		TEST_METHOD(GameMethod1)
		{
			Assert::AreEqual(1, 1, L"Are equal");
		}

	};
}