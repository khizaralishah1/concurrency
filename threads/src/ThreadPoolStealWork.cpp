#include <queue>
#include <memory>
#include <future>

/*
  ThreadPool: multiple threads

    work_queues:
      thread_local_queue: w1, w2, w3, work4       [AVOIDS CACHE PING PONG]
      pool_queue: [TAKE WORK FROM GLOBAL QUEUE ONLY IF THERE IS NO WORK IN THREAD'S OWN QUEUE]


      * work stealing: if thread has no work in local queue, none in global (pool) if can steal
  work from other's queue
*/
class FunctionWrapper {};



template <typename T>
class ThreadSafeQueue {};