#pragma once
#include "Dispatcher.h"
#include "System/Types.h"
#include <atomic>
#include <thread>
#include <functional>
#include <string>
#include <memory>

namespace Hawk {

class HAWK_DLL_EXPORT Thread final
{
public:
	using UpdateFunc = std::function<void(void)>;
	using FrameBeginCallback = std::function<void(void)>;
	using FrameEndCallback = std::function<void(void)>;

	Thread(const std::string& p_Name, UpdateFunc p_UpdateFunc);

	void RegisterFrameBegin(FrameBeginCallback p_Callback);
	void RegisterFrameEnd(FrameBeginCallback p_Callback);

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	void Start();
	void Stop();
	const std::string& GetName() const;
	ThreadID GetID() const;
	std::shared_ptr<Dispatcher> GetDispatcher() const;
	static void Sleep();

	static const std::string sc_MainThreadName;
	static const ThreadID sc_MainThreadID;

private:
	void Run();

	std::string m_Name;
	std::shared_ptr<Dispatcher> m_Dispatcher;
	std::thread m_Thread;
	UpdateFunc m_UpdateFunc;
	FrameBeginCallback m_FrameBegin;
	FrameEndCallback m_FrameEnd;
	std::atomic_bool m_bStopSignal;

	static std::atomic_uint s_uiNextThreadID;
	ThreadID m_ID;
};

}