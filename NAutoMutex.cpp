#include "stdafx.h"
#include "NAutoMutex.h"

NAutoMutex::NAutoMutex(const std::wstring_view wsvMutexName, AUTO_MUTEX_TYPE mutexType/*= AUTO_MUTEX_TYPE::UNNAMED*/) :
	m_bInit{ FALSE },
	m_hMutex{ nullptr },
	m_mutexType{ mutexType }
{
	if (wsvMutexName.empty())
	{
		m_mutexType = AUTO_MUTEX_TYPE::UNNAMED;
	}
	else
	{
		if (m_mutexType == AUTO_MUTEX_TYPE::UNNAMED)
		{
			m_mutexType = AUTO_MUTEX_TYPE::NAMED;
		}
	}

	switch (m_mutexType)
	{
		case AUTO_MUTEX_TYPE::GLOBAL:
		{
			m_strMutexName = L"Global\\";
			m_strMutexName += AUTO_MUTEX_DEFAULT_NAME;
			m_strMutexName += wsvMutexName;
			break;
		}

		case AUTO_MUTEX_TYPE::LOCAL:
		{
			m_strMutexName = L"Local\\";
			m_strMutexName += AUTO_MUTEX_DEFAULT_NAME;
			m_strMutexName += wsvMutexName;
			break;
		}

		case AUTO_MUTEX_TYPE::NAMED:
		{
			m_strMutexName = AUTO_MUTEX_DEFAULT_NAME;
			m_strMutexName += wsvMutexName;
			break;
		}

		case AUTO_MUTEX_TYPE::UNNAMED:
		{
			m_strMutexName.clear();
			break;
		}
	}

	if (!TryCreate())
	{
		return;
	}

	if (!Wait())
	{
		return;
	}

	m_bInit = TRUE;
}

NAutoMutex::~NAutoMutex()
{
	if (m_hMutex)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		m_hMutex = nullptr;
	}
}

NAutoMutex::operator bool() const
{
	return m_bInit;
}

BOOL NAutoMutex::TryCreate()
{
	switch (m_mutexType)
	{
		case AUTO_MUTEX_TYPE::GLOBAL:
		{
			SECURITY_DESCRIPTOR sd{ 0, };
			if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
			{
				return FALSE;
			}

			if (!SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
			{
				return FALSE;
			}

			SECURITY_ATTRIBUTES sa{ 0, };
			sa.nLength = sizeof(sa);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = &sd;
			m_hMutex = CreateMutexW(&sa, FALSE, m_strMutexName.c_str());
			break;
		}

		case AUTO_MUTEX_TYPE::LOCAL:
		{
			m_hMutex = CreateMutexW(nullptr, FALSE, m_strMutexName.c_str());
			break;
		}

		case AUTO_MUTEX_TYPE::NAMED:
		{
			m_hMutex = CreateMutexW(nullptr, FALSE, m_strMutexName.c_str());
			break;
		}

		case AUTO_MUTEX_TYPE::UNNAMED:
		{
			m_hMutex = CreateMutexW(nullptr, FALSE, nullptr);
			break;
		}
	}

	return TRUE;
}

BOOL NAutoMutex::Wait()
{
	while (true)
	{
		DWORD dwWait{ WaitForSingleObject(m_hMutex, INFINITE) };
		switch (dwWait)
		{
			case WAIT_FAILED:
				return FALSE;

			case WAIT_TIMEOUT:
				return FALSE;

			case WAIT_ABANDONED:
				return TRUE;

			case WAIT_OBJECT_0:
				return TRUE;

			default:
				return FALSE;
		}
	}
}