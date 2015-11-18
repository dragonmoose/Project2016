#include "pch.h"
#include "Hawk/Config.h"
#include "Hawk/Core.h"
#include "Hawk/Logger.h"
#include "Hawk/Time.h"
#include "Hawk/Duration.h"
#include "Hawk/Exception.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Hawk;

std::wstring ToWString(const std::string& p_Source)
{
	std::wstring l_Dest(p_Source.length(), L' ');
	std::copy(p_Source.begin(), p_Source.end(), l_Dest.begin());
	return l_Dest;
}

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Duration>(const Duration& t)
{
	return ToWString(t.ToString());
}

namespace HawkUnitTests
{
	TEST_CLASS(TimeTest)
	{
	public:

		TEST_METHOD(Precision)
		{
			Assert::AreEqual(Duration(2000, Duration::Precision::Millisecond), Duration(2, Duration::Precision::Second));
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
				Core l_Core(false);
				LOG_INFO("testing info");
				l_Core.RegisterThread("TestThread");
				Assert::ExpectException<Exception>([&l_Core]() { l_Core.RegisterThread("TestThread"); });
			}
	};

	TEST_CLASS(ConfigTest)
	{
	public:

		TEST_METHOD(TestDefault)
		{
			Assert::AreEqual(123.456f, Config::Instance().Get<float>("non.existing", 123.456f));
			Assert::AreNotEqual(123.456f, Config::Instance().Get<float>("non.existing", 123.4567f));
		}
	};
}