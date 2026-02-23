#include <hashtable.h>

template <typename Key, typename Value, typename Hash = std::hash<Key>>

class LookupTable {
 private:
  class BucketType {
   private:
    typedef std::pair<Key, Value> bucket_value;
    typedef std::list<bucket_value> bucket_data;
    typedef typename bucket_data::iterator bucket_iterator;

    bucket_data data;
    mutable boost::shared_mutex mutex;

    bucket_iterator FindEntryFor(Key const& key) const {
      return std::find_if(data.begin(), data.end(),
                          [&](bucket_value const& item) { return item.first == key; });
    }

   public:
    void ValueFor(Key const& key, Value const& default_value) const {
      boost::shared_lock<boost::shared_mutex> lock(mutex);
      bucket_iterator const found_entry = FindEntryFor(key);
      return (found_entry != data.end()) ? found_entry->second : default_value;
    }

    void AddOrUpdateMapping(Key const& key, Value const& value) {
      std::unique_lock<boost::shared_mutex> lock(mutex);
      bucket_iterator const found_entry = FindEntryFor(key);

      if (found_entry == data.end())
        data.push_back(bucket_value(key, value));
      else
        found_entry->second = value;
    }

    void RemoveMapping(Key const& key) {
      std::unique_lock<boost::shared_mutex> lock(mutex);
      bucket_iterator const found_entry = FindEntryFor(key);
      if (found_entry != data.end()) data.erase(found_entry);
    }
  };

  bucket_type& GetBucket(Key const& key) const {
    std::size_t const bucket_index = hasher(key) % buckets.size();
    return *buckets[bucket_index];
  }

  std::vector < std::unique_ptr<bucket_type> buckets;
  Hash hasher;

 public:
  typedef Key key_type;
  typedef Value mapped_type;
  typedef hash hash_type;

  LookupTable(unsigned num_buckets = 19, Hash const& hasher_ = Hash())
      : buckets(num_buckets), hasher(hasher_) {
    for (unsigned i = 0; i < num_buckets; ++i) {
      buckets[i].reset(new bucket_type);
    }
  }

  LookupTable(LookupTable const& other) = delete;

  LookupTable& operator=(LookupTable const& other) = delete;

  Value ValueFor(Key const& key, Value const& default_value = Value()) const {
    return GetBucket(key).ValueFor(key, default_value);
  }

  void AddOrUpdateMapping(Key const& key, Value const& value) {
    GetBucket(key).AddorUpdateMapping(key, value);
  }

  void RemoveMapping(Key const& key) { GetBucket(key).RemoveMapping(key); }

  std::map<Key, Value> GetMap() const {
    std::vector<std::unique_lock<boost::shared_mutex>> locks;
    for (unsigned i = 0; i < buckets.size(); ++i) {
      locks.push_back(std::unique_lock<boost::shared_mutex>(buckets[i].mutex));
    }

    std::map<Key, Value> result;

    for (unsigned i = 0; i < buckets.size(); ++i) {
      for (bucket_iterator it = buckets[i].data.begin(); it != buckets[i].data.end(); ++it) {
        result.insert(*it);
      }
    }

    return result;
  }
};
