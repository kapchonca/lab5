#ifndef LAB5_H_THREAD_POOL_H_
#define LAB5_H_THREAD_POOL_H_

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * @brief A simple thread pool implementation.
 */
class ThreadPool {
 public:
  /**
   * @brief Constructs a ThreadPool object with the specified number of threads.
   * 
   * @param numThreads The number of threads to create in the thread pool.
   */
  ThreadPool(size_t numThreads);

  /**
   * @brief Enqueues a task into the thread pool.
   * 
   * @tparam F The type of the function to be enqueued.
   * @tparam Args The types of the arguments of the function.
   * @param f The function to be enqueued.
   * @param args The arguments to be passed to the function.
   */
  template <class F, class... Args>
  bool enqueue(F&& f, Args&&... args) {
    if (tasks_.size() > std::thread::hardware_concurrency()) {
      return false;
    }
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      tasks_.emplace([=] { f(std::forward<Args>(args)...); });
    }
    condition_.notify_one();
    return true;
  }

  /**
   * @brief Joins all threads and destroys the ThreadPool object.
   */
  ~ThreadPool();

 private:
  std::vector<std::thread>
      workers_; /**< The worker threads in the thread pool. */
  std::queue<std::function<void()>>
      tasks_; /**< The queue of tasks to be executed by the threads. */
  std::mutex
      queue_mutex_; /**< Mutex for synchronizing access to the task queue. */
  std::condition_variable
      condition_; /**< Condition variable for signaling changes in the task queue. */
  bool
      stop_; /**< Flag indicating whether the thread pool should stop execution. */
};

#endif  // LAB5_H_THREAD_POOL_H_