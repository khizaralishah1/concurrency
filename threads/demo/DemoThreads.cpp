#include <mutex>
#include <thread>
#include <vector>

#include "InterruptibleThread.h"

std::mutex config_mutex;
std::vector<InterruptibleThread> background_threads;

int disk_1 = 1, disk_2 = 2;

void BackgroundThread(int disk_id) {
  while (true) {
    InterruptionPoint();
    fs_change fsc = get_fs_changes(disk_id);
    if (fsc.has_changes()) {
      UpdateIndex(fsc);
    }
  }
}

void StartBackgroundProcessing() {
  background_threads.push_back(InterruptibleThread(BackgroundThread, disk_1));
  background_threads.push_back(InterruptibleThread(BackgroundThread, disk_2));
}

int main() {
  StartBackgroundProcessing();
  ProcessGUIUntilExit();

  std::unique_lock<std::mutex> lock(config_mutex);
  for (unsigned i = 0; i < background_threads.size(); ++i) {
    background_threads[i].Interrupt();
  }
  for (unsigned i = 0; i < background_threads.size(); ++i) {
    background_threads[i].Join();
  }
}