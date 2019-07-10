#include <phpcpp.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <vector>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

Php::Value fetchDataFromDB(char* schema, string query)
{
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "A0WK51YS");

    /* Connect to the MySQL so_test database */
    con->setSchema(schema);
    stmt = con->createStatement();

    // Select all columns/rows from example_table
    res = stmt->executeQuery("SELECT * FROM invoices");

    //get result set metadata
    sql::ResultSetMetaData *res_meta = res -> getMetaData();
    int columns = res_meta -> getColumnCount();

    //Loop for each row
    int key = 0;
    while (res->next()) {
        for (int i = 1; i <= columns; i++) {
            Php::out << res->getString(i) << "|" ;
        }
        Php::out << endl;
        key++;
    }

    delete res;
    delete stmt;
    delete con;
}

void createDataMap()
{
    map<string, int> result;
}

/**
 *  Example function that shows how to generate output
 */
Php::Value exec(Php::Parameters &params)
{
    Php::Value schemas = params[0];
    string query = params[1];

    for (int i = 0; i < sizeof(schemas); i++) {
        Php::out << schemas[i] << endl;
    }

    Php::out << "Running query: " << query << endl;
//
//    // Start a timer
//    auto start = std::chrono::steady_clock::now();
//
//    vector<future<Php::Value>> futures;
//    // Loop over the provided schemas to execute the queries.
//    for (unsigned int a = 0; a < sizeof(schemas) / sizeof(schemas[0]); a = a + 1)
//    {
//        futures.push_back(
//            async(fetchDataFromDB, schemas[a], params[1])
//        );
//    }
//
//    for (auto &e : futures)
//    {
//        Php::out << e.get() << endl;
//    }
//
//    auto end = std::chrono::steady_clock::now();
//    Php::out << "Script finished in: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << std::endl;

    Php::Value array;
    array[0] = "test";
    array[1] = "test2";

    return array;
}

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C"
{
/**
 *  Function that is called by PHP right after the PHP process
 *  has started, and that returns an address of an internal PHP
 *  strucure with all the details and features of your extension
 *
 *  @return void*   a pointer to an address that is understood by PHP
 */
PHPCPP_EXPORT void *get_module()
{
    // static(!) Php::Extension object that should stay in memory
    // for the entire duration of the process (that's why it's static)
    static Php::Extension extension("query_executor", "1.0");

    extension.add("helloWorld", exec, {
            Php::ByVal("schemas", Php::Type::Array),
            Php::ByVal("query", Php::Type::String)
    });

    // return the extension
    return extension;
}
};