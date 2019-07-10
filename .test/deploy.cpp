/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <vector>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce the build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

fetchDataFromDB(string schema)
{
  sql::Driver *driver;
  sql::Connection *con;
  sql::Statement *stmt;
  sql::ResultSet *res;

  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://127.0.0.1:3306", "root", "A0WK51YS");

  try
  {
    cout << "Connecting to: " << schema << endl;
    con->setSchema(schema);

    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM invoices where date >= '2017-04-01'"); // replace with dynamic statement statement
    while (res->next())
    {
      //cout << "\t... MySQL replies (" << schema << "):";

      /* Access column fata by numeric offset, 1 is the first column */
      cout << res->getString("date") << endl;

      //return res->getString("date");
    }

    return res->~ResultSet();
  }
  catch (sql::SQLException &e)
  {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION_l_ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
  }

  delete res;
  delete stmt;
  delete con;
}

int main(void)
{
cout << endl;
cout << "Running get rows from multiple schemas" << endl;

string schemas[2] = {"clockwork", "clockwork2"};
vector<future<string>> results;

for (unsigned int a = 0; a < sizeof(schemas) / sizeof(schemas[0]); a = a + 1)
{
  results.push_back(
    async(fetchDataFromDB, schemas[a])
  );

  cout << "loop done" << endl;
}

for (auto &e : results) {
  cout << e.get() << endl;
}

// cout << results;

// cout << endl;

return EXIT_SUCCESS;
}

