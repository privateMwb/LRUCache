// Embedding LRUCache inside a larger class.
//
// Demonstrates:
// - Wrapping LRUCache as a private implementation detail
// - Exposing a domain-specific API instead of the cache directly
// - invalidate() built on erase(), hit-rate reporting built on hitRate()

#include <support/framework.h>

using namespace CachePro;

namespace {

// A stand-in service that looks up user names by id. Callers never see
// LRUCache directly — just nameFor()/invalidate(), the vocabulary that
// makes sense for this class. The cache is an implementation detail that
// could be swapped out later without changing this class's public API.
class UserProfileService {
  public:
    explicit UserProfileService(std::size_t cacheCapacity) : cache_(cacheCapacity) {}

    std::string nameFor(int userId) {
        if (std::string* cached = cache_.get(userId)) {
            return *cached;
        }

        std::string name = fetchFromDatabase(userId);
        cache_.put(userId, name);
        return name;
    }

    void invalidate(int userId) {
        (void)cache_.erase(userId);
    }

    double cacheHitRate() const {
        return cache_.hitRate();
    }

  private:
    static std::string fetchFromDatabase(int userId) {
        return "user-" + std::to_string(userId); // stands in for a real lookup
    }

    LRUCache<int, std::string> cache_;
};

} // namespace

static void run_examples() {

    // Consumers of the service only ever call nameFor() — nothing here
    // hints that an LRUCache is involved at all.
    setTitle("Wrapping LRUCache in a Service");

    UserProfileService service(2);

    std::cout << "nameFor(1): " << service.nameFor(1) << " (miss, loads from source)\n";
    std::cout << "nameFor(1): " << service.nameFor(1) << " (hit)\n";
    std::cout << "hit rate  : " << service.cacheHitRate() << "%\n\n";

    // invalidate() is the service's own vocabulary for "forget this
    // entry" — it happens to be erase() underneath, but callers don't
    // need to know that.
    setTitle("Invalidation");

    service.invalidate(1);

    std::cout << "nameFor(1) after invalidate: " << service.nameFor(1) << " (miss again)\n";
    std::cout << "hit rate                   : " << service.cacheHitRate() << "%\n";
}

REGISTER_EXAMPLE_SUITE();
