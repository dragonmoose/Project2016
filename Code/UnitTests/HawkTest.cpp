#include "pch.h"
#include "Hawk/System/Config.h"
#include "Hawk/Base/Core.h"
#include "Hawk/Console/Logger.h"
#include "Hawk/System/Time.h"
#include "Hawk/System/Duration.h"
#include "Hawk/System/Exception.h"

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

			TEST_METHOD(Modules)
			{
				CoreSettings l_Settings;
				l_Settings.m_bConsole = false;
				Core l_Core(l_Settings);
				l_Core.Initialize();
				LOG("testing info", "test", Info);
				l_Core.CreateModuleThread("TestThread");
				Assert::ExpectException<Exception>([&l_Core]() { l_Core.CreateModuleThread("testthread"); });
				Assert::ExpectException<Exception>([&l_Core]() { l_Core.CreateModuleThread("TestThread"); });
				Assert::ExpectException<Exception>([&l_Core]() { l_Core.CreateModuleThread("TESTTHREAD"); });
				Assert::ExpectException<Exception>([&l_Core]() { l_Core.CreateModuleThread(Thread::sc_MainThreadName); });
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