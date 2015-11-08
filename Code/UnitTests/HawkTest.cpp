#include "pch.h"
#include "Hawk/Time.h"
#include "Hawk/Duration.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Hawk;

namespace HawkUnitTests
{
	TEST_CLASS(TimeTest)
	{
	public:

		TEST_METHOD(ToString)
		{
			Time l_Time;
			Assert::AreEqual(19, (int)l_Time.ToString().length());
		}

		TEST_METHOD(Operators)
		{
			Time l_Time1;
			Time l_Time2(l_Time1);
			Duration l_Duration(1, Duration::Precision::Second);

			l_Time2 += l_Duration;
			Assert::AreNotEqual(l_Time1.ToString(), l_Time2.ToString());
			Assert::IsTrue(l_Time2 > l_Time1);

			l_Time2 -= l_Duration;
			Assert::IsTrue(l_Time2 == l_Time1);

			l_Time2 -= l_Duration;
			Assert::IsTrue(l_Time2 < l_Time1);
		}

	};
}