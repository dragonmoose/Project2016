#include "pch.h"
#include "Hawk/Config.h"
#include "Hawk/Core.h"
#include "Hawk/Time.h"
#include "Hawk/Duration.h"
#include "Hawk/Exception.h"

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
			Assert::AreEqual(8, (int)l_Time.ToString().length());
			Assert::AreEqual(19, (int)l_Time.ToString(TimeFormat::DateAndTime).length());
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

	TEST_CLASS(CoreTest)
	{
		public:

			TEST_METHOD(ExceptionTest)
			{
				bool l_bCaught = false;
				try
				{
					THROW_IF(true, "Should always throw");
				}
				catch (Exception&)
				{
					l_bCaught = true;
				}
				Assert::IsTrue(l_bCaught);
			}

			TEST_METHOD(RegisterThreadSystem)
			{
				Core& l_Core = Core::Instance();
				l_Core.RegisterThread("TestThread");
				Assert::ExpectException<Exception>([&l_Core]() { l_Core.RegisterThread("TestThread"); });
			}
	};

	TEST_CLASS(ConfigTest)
	{
	public:

		TEST_METHOD(TestDefault)
		{
			Config::Initialize();
			Assert::AreEqual(123.456f, Config::Get<float>("non.existing", 123.456f));
			Assert::AreNotEqual(123.456f, Config::Get<float>("non.existing", 123.4567f));
		}
	};
}