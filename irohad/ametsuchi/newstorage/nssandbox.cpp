#include "rel_db_backend.hpp"
#include "keyvalue_db_backend.hpp"
#include <iostream>

using namespace iroha::newstorage;

void rel_test_1() {
  RelDbBackend db("sandbox/sandbox.db", nullptr);
}

void kv_test_1() {
  KeyValueDbBackend db({ "sandbox/str"}, nullptr );
  db.put("xxx", "xxxx");
  db.put("xxx", "aaaaa", "zzzzzzzaasasasaszzzzzzzzzzzzz");
}

void kv_test_2() {
  KeyValueDbBackend db({ "sandbox/num"}, nullptr );
  for (uint64_t i=100500; i<100600; ++i) {
    db.put(i, std::to_string(i));
  }
  std::cout << db.get(100555) << std::endl;
  db.iterate_from(100500, [](uint64_t key, const Slice& value) -> bool {
    std::cout << key << ": " << value.ToString() << "\n";
    return true;
  });
}

int main() {
  using namespace std;

  try {
    kv_test_1();
    kv_test_2();
    rel_test_1();
  } catch (const std::exception& e) {
    cout << e.what() << endl;
  }
}
