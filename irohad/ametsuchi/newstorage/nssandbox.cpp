#include "rel_db_backend.hpp"
#include <iostream>

using namespace iroha::newstorage;

int main() {
  using namespace std;

  try {
    RelDbBackend db("sandbox.db", nullptr);
  } catch (const std::exception& e) {
    cout << e.what() << endl;
  }
}
