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
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

Php::Value fetchDataFromDB(string schema, string query)
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
    res = stmt->executeQuery(query);

    //get result set metadata
    sql::ResultSetMetaData *res_meta = res->getMetaData();
    int columns = res_meta->getColumnCount();

    //Loop for each row
    int key = 0;
    Php::Value array;
    while (res->next()) {
        array[key] = {};
        for (int i = 1; i <= columns; i++) {

            string column = res_meta->getColumnName(i);
            string columnTypeName = res_meta->getColumnTypeName(i);

            if (columnTypeName == "INT UNSIGNED" || columnTypeName == "TINYINT") {
                int integer = res->getInt(i);
                array[key][column] = integer;
            }

            if (columnTypeName != "INT UNSIGNED" && columnTypeName != "TINYINT") {
                string value = res->getString(i);
                array[key][column] = value;
            }
        }
        array[key]["meta"]["schema"] = schema;
        key++;
    }

    delete res;
    delete stmt;
    delete con;

    return array;
}

/**
 *  Example function that shows how to generate output
 */
Php::Value exec(Php::Parameters &params)
{
    std::vector<string> schemas = params[0];
    string query = params[1];

    Php::Value array;
    for (size_t i = 0; i < schemas.size(); i++)
    {
        int key = sizeof(array) / sizeof(array[0]);
        if (key == 1) {
            key = 0;
        }

        Php::Value data = fetchDataFromDB(schemas[i], query);
        for (auto &items: data)
        {
            array[key] = items.second;
            key++;
        }
    }

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

        extension.add<exec>("helloWorld", {
                Php::ByVal("schemas", Php::Type::Array),
                Php::ByVal("query", Php::Type::String)
        });

            // return the extension
        return extension;
    }
};