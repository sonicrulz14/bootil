
#include "Bootil/Bootil.h"
#include "tinythreadpp/tinythread.h"
#include "tinythreadpp/fast_mutex.h"



namespace Bootil
{
	namespace Threads
	{
		void Thread::RunInThread( void* aArg )
		{
			Thread* pThread = ( Thread* ) aArg;
			pThread->Lock();
			pThread->m_bClosing = false;
			pThread->m_bRunning = true;
			pThread->Unlock();
			pThread->Run();
			pThread->Lock();
			pThread->m_bRunning = false;
			pThread->Unlock();

			//
			// you can use this function to delete this;
			//
			pThread->OnThreadFinished();
		}

		Thread::Thread()
		{
			m_pThread = NULL;
			m_bRunning = false;
		}

		Thread::~Thread()
		{
			SetClosing( true );
			Join();
		}

		bool Thread::StartInThread()
		{
			if ( m_pThread ) { return false; }

			m_bRunning = true;
			m_bClosing = false;
			m_pThread = new tthread::thread( RunInThread, this );
			return true;
		}

		bool Thread::Running() { Threads::Guard m( &m_Mutex ); return m_bRunning; }
		void Thread::Lock() { m_Mutex.Lock(); }
		void Thread::Unlock() { m_Mutex.Unlock(); }
		void Thread::TryLock() { m_Mutex.TryLock(); }

		bool Thread::WantsToClose() { Threads::Guard m( &m_Mutex ); return m_bClosing; }
		void Thread::SetClosing( bool b ) { Threads::Guard m( &m_Mutex ); m_bClosing = b; }

		void Thread::Join()
		{
			if ( !m_pThread ) { return; }

			m_pThread->join();
			delete m_pThread;
			m_pThread = NULL;
		}
	}
}
