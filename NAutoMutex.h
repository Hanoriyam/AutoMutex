#pragma once
#include <string>
#include <string_view>

enum class AUTO_MUTEX_TYPE
{
	GLOBAL,
	LOCAL,
	NAMED,
	UNNAMED,
};

constexpr const wchar_t* const AUTO_MUTEX_DEFAULT_NAME{ L"N_AUTO_MUTEX_" };

class NAutoMutex
{
public:
	NAutoMutex(const std::wstring_view wsvMutexName, AUTO_MUTEX_TYPE mutexType = AUTO_MUTEX_TYPE::UNNAMED);
	~NAutoMutex();
	operator bool() const;

private:
	NAutoMutex(const NAutoMutex&) = delete;
	NAutoMutex& operator=(const NAutoMutex&) = delete;
	NAutoMutex(NAutoMutex&&) noexcept = delete;
	NAutoMutex& operator=(NAutoMutex&&) noexcept = delete;

	BOOL TryCreate();
	BOOL Wait();

private:
	std::wstring m_strMutexName;
	HANDLE m_hMutex;
	AUTO_MUTEX_TYPE m_mutexType;
	BOOL m_bInit;
};