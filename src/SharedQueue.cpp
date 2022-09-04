#include <mutex>
#include <queue>
#include <condition_variable>

class SharedQueue {

private:
  std::queue<Mat*> q;
  std::mutex m;
  std::condition_variable c; 
public:

  atomic<bool> framesFinished = false;

  Mat* pop()  {
    std::unique_lock<std::mutex> l(m);
    c.wait(l, [&]{ return framesFinished || !q.empty();});
      
    if(q.empty() == false){
      Mat* ret = q.front();
      q.pop();
      return ret;
    }
    return nullptr;
  }

  void push(Mat* v) {
    std::unique_lock<std::mutex> l(m);
    q.push(v);
    c.notify_one();
  }

  void finish(){
    framesFinished = true;
    c.notify_all();
  }
  
};