#include "wsv_sqlite_db.hpp"
#include "sqlite_wrapper.hpp"
#include "keyvalue_db_backend.hpp"
#include <iostream>

#include <stdio.h>

using namespace iroha::newstorage;

void rel_test_1() {
  using namespace std;
  WsvSqliteDB db(SqliteWrapper::create(SqliteWrapper::Options("sandbox/sandbox.db", "sb "), nullptr));

  db.addPeer("pk1", "ad1");
  db.addPeer("pk2", "ad2");
  db.addPeer("pk3", "ad3");
  db.loadPeers([](const string& pk, const string& a) {
    cout << pk << " : " << a << '\n';
  });
  //etc
}

void rel_test_2() {
  using namespace std;
  remove("rel_test_2.db");
  WsvSqliteDB db(SqliteWrapper::create(SqliteWrapper::Options("rel_test_2.db", "sb "), nullptr));

  db.addPeer("pk1", "ad1");
  db.addPeer("pk2", "ad2");
  db.addPeer("pk3", "ad3");

  cout << __LINE__ << ": " << db.removeAccountSignatory("a1", "s1") << "\n";
  cout << __LINE__ << ": " << db.removePeer("pk1") << "\n";
  cout << __LINE__ << ": " << db.removePeer("pk1") << "\n";
  cout << __LINE__ << ": " << db.dropPeers() << "\n";

  db.createRole("r", "1010101");
  db.createDomain("d", "r");
  db.createAccount("a", "d", 11);
  cout << __LINE__ << ": " << db.updateAccountPermissions("a", "11111000") << "\n";
  cout << __LINE__ << ": " << db.updateAccountPermissions("a", "11111000") << "\n";
  cout << __LINE__ << ": " << db.updateAccountPermissions("z", "11111000") << "\n";
  cout << __LINE__ << ": " << db.setQuorum("a", 33) << "\n";
  db.createAccount("b", "d", 22);
  cout << __LINE__ << ": " << db.updateGrantablePermissions("a", "b", "1010") << "\n";
  cout << __LINE__ << ": " << db.createRole("r2", "101") << "\n";
  cout << __LINE__ << ": " << db.createRole("r3", "11010") << "\n";
  cout << __LINE__ << ": " << db.attachAccountRole("a", "r2") << "\n";
  cout << __LINE__ << ": " << db.attachAccountRole("a", "r3") << "\n";
  cout << __LINE__ << ": " << db.detachAccountRole("a", "r2") << "\n";
  cout << __LINE__ << ": " << db.updateGrantablePermissions("b", "a", "1110") << "\n";
  cout << __LINE__ << ": " << db.addAccountSignatory("a", "pk2") << "\n";
  cout << __LINE__ << ": " << db.addAccountSignatory("b", "pk2") << "\n";
  cout << __LINE__ << ": " << db.addAccountSignatory("a", "pk3") << "\n";
  cout << __LINE__ << ": " << db.addAccountSignatory("b", "pk3") << "\n";
  cout << __LINE__ << ": " << db.removeAccountSignatory("b", "pk2") << "\n";
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
    //kv_test_1();
    //kv_test_2();
    //rel_test_1();
    rel_test_2();
  } catch (const std::exception& e) {
    cout << e.what() << endl;
  }
}
