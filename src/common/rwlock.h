//=========================================================================
//  RWLOCK.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include <pthread.h>

#include "commondefs.h"

NAMESPACE_BEGIN

class COMMON_API ILock
{
	public:
		virtual ~ILock() {};
		virtual void lock() = 0;
		virtual bool tryLock() = 0;
		virtual void unlock() = 0;
};

class COMMON_API IReadWriteLock
{
	public:
		virtual ~IReadWriteLock() {};
		virtual ILock& readLock() = 0;
		virtual ILock& writeLock() = 0;
};

class COMMON_API ReaderMutex
{
	private:
		IReadWriteLock &lock;

	public:
		ReaderMutex(IReadWriteLock &lock) : lock(lock) { lock.readLock().lock(); }
		ReaderMutex(const IReadWriteLock &lock) : lock(const_cast<IReadWriteLock&>(lock)) { this->lock.readLock().lock(); }
		~ReaderMutex() { lock.readLock().unlock(); }
};

class COMMON_API WriterMutex
{
	private:
		IReadWriteLock &lock;

	public:
		WriterMutex(IReadWriteLock &lock) : lock(lock) { lock.writeLock().lock(); }
		~WriterMutex() { lock.writeLock().unlock(); }
};


class COMMON_API ReentrantReadWriteLock : public IReadWriteLock
{
	private:
		struct ThreadLocalState
		{
			int readLockCount;
			ThreadLocalState() :readLockCount(0) {}
		};

		pthread_rwlock_t rwlock;

		// FIXME a new created for each lock, so the limit (128) can be reached easily
		pthread_key_t key;
		pthread_t writerThread;
		int writeLockCount;

		ThreadLocalState& getThreadLocalState();
		static void deleteThreadLocalState(void *tls);

		void lockRead();
		bool tryLockRead();
		void unlockRead();
		void lockWrite();
		bool tryLockWrite();
		void unlockWrite();

		class ReadLock : public ILock
		{
			private:
				ReentrantReadWriteLock &rwl;
			public:
				ReadLock(ReentrantReadWriteLock &rwl) : rwl(rwl) {}
				virtual void lock() { rwl.lockRead(); }
				virtual bool tryLock() { return rwl.tryLockRead(); }
				virtual void unlock() { rwl.unlockRead(); }
		};

		class WriteLock : public ILock
		{
			private:
				ReentrantReadWriteLock &rwl;
			public:
				WriteLock(ReentrantReadWriteLock &rwl) : rwl(rwl) {}
				virtual void lock() { rwl.lockWrite(); }
				virtual bool tryLock() { return rwl.tryLockWrite(); }
				virtual void unlock() { rwl.unlockWrite(); }
		};

		ReadLock _readLock;
		WriteLock _writeLock;
	public:
		ReentrantReadWriteLock();
		~ReentrantReadWriteLock();
		ILock& readLock() { return _readLock; }
		ILock& writeLock() { return _writeLock; }
};

NAMESPACE_END


#endif


