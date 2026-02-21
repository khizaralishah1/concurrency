#include <future>
#include <map>
#include <cassert>

std::promise<int> p;
std::future<int> f(p.get_future());
assert(f.valid());
std::shared_future<int> sf(std::move(f));
assert(!f.valid());
assert(sf.valid());

typedef std::map<Index, Data, Comparator, Allocator>::iterator MyMapIterator;
std::promise<MyMapIterator> p;
auto sf = p.get_future().share();

int main() {


  return EXIT_SUCCESS;
}