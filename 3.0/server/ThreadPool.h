/* 
 * File:   ThreadPool.h
 * Author: phpwind
 *
 * Created on 2010年12月21日, 上午10:48
 */

#ifndef THREADPOOL_H
#define	THREADPOOL_H
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "FifoList.h"
#include "ThreadTask.h"

class ThreadPool {
public:

    ThreadPool(const int num_threads) :
    mDone(false) {
        mThreads.reserve(num_threads);
    }

    virtual ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mDone = true;
        }
        for (int idx = 0; idx < mThreads.size(); ++idx) {
            mThreads[idx]->join();
            delete mThreads[idx];
            mThreads[idx] = NULL;
        }
    }

    virtual void Start() {
        while (mThreads.capacity() - mThreads.size() > 0) {
            std::thread *t = new std::thread(std::bind(&ThreadPool::Loop, this));
            mThreads.push_back(t);
        }
    }

    virtual void ExecuteAsync(ThreadTask* task) {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mWork.push(task);
        }
        cond.notify_one();
    }

private:

    void Loop() {
        while (true) {
            ThreadTask *task = NULL;
            {
                std::unique_lock<std::mutex> lock(mMutex); // 一个{}之后结束锁定
                while (mWork.empty() && !mDone) {
                    cond.wait(lock);
                }
                if (mDone) {
                    break;
                }
                task = mWork.pop();
            }
            task->run();
            delete task;
        }
    }

    std::vector<std::thread*> mThreads;
    FifoList<ThreadTask*> mWork;
    std::mutex mMutex;
    std::condition_variable cond;
    bool mDone;
};

#endif	/* THREADPOOL_H */

