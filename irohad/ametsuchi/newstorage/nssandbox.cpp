#include "wsv_sqlite_db.hpp"
#include "sqlite_wrapper.hpp"
#include "keyvalue_db_backend.hpp"
#include <iostream>

using namespace iroha::newstorage;

void rel_test_1() {
  using namespace std;
  WsvSqliteDB db(SqliteWrapper::create("sandbox/sandbox.db"), nullptr);

  db.addPeer("pk1", "ad1");
  db.addPeer("pk2", "ad2");
  db.addPeer("pk3", "ad3");
  db.loadPeers([](const string& pk, const string& a) {
    cout << pk << " : " << a << '\n';
  });
  //etc
}

void kv_test_1() {
  KeyValueDbBackend db;
  db.create({ "sandbox/str"} );
  db.put("xxx", "xxxx");
  db.put("xxx", "aaaaa", "zzzzzzzaasasasaszzzzzzzzzzzzz");
}

void kv_test_2() {
  KeyValueDbBackend db;
  db.create({ "sandbox/num"} );
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
