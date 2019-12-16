#include <phpcpp.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <vector>
#include <typeinfo>
#include <any>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

bool isNumber(string s)
{
    for (unsigned int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == false)
            return false;

    return true;
}

int getKey(Php::Value array)
{
    return array.size();
}

Php::Value fetchDataFromDB(string schema, string query, vector<Php::Value> bindings)
{
    sql::Driver *driver;
    sql::Connection *con;
    sql::ResultSet *res;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSetMetaData *res_meta;

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "A0WK51YS");
    con->setSchema(schema);

    prep_stmt = con->prepareStatement(query);

    int i = 1;
    for (auto &binding : bindings) {
        if (isNumber(binding)) {
            prep_stmt->setInt(i++, binding);
        } else {
            string val = binding;
            prep_stmt->setString(i++, val);
        }
    }

    res = prep_stmt->executeQuery();
    delete prep_stmt;

    res_meta = res->getMetaData();
    int columns = res_meta->getColumnCount();

    //Loop for each row
    int key = 0;
    Php::Value array;
    while (res->next()) {
        Php::Object object;
        for (int i = 1; i <= columns; i++) {
            string column = res_meta->getColumnName(i);
            string columnTypeName = res_meta->getColumnTypeName(i);

            if (columnTypeName == "INT UNSIGNED" || columnTypeName == "TINYINT") {
                int integer = res->getInt(i);
                object[column] = integer;
            }

            if (columnTypeName != "INT UNSIGNED" && columnTypeName != "TINYINT") {
                string value = res->getString(i);
                object[column] = value;
            }
        }
        object["schema"] = schema;

        array[key++] = object;
    }

    delete res;
    delete con;

    return array;
}

/**
 *
 * @param params
 * @return Php::Value array
 */
Php::Value exec(Php::Parameters &params)
{
    vector<string> schemas = params[0];
    vector<Php::Value> bindings = params[2];
    string query = params[1];

    std::vector<std::future<Php::Value>> futures;

    Php::Value array;
    for (size_t i = 0; i < schemas.size(); i++)
    {
        futures.push_back (std::async(fetchDataFromDB, schemas[i], query, bindings));
//        Php::Value data = fetchDataFromDB(schemas[i], query, bindings);
//
//        // Append the results to the existing array.
//        int key = getKey(array);
//        for (auto &items: data)
//        {
//            array[key++] = items.second;
//        }
    }

    for(auto &e : futures) {
        Php::Value data = e.get();

        int key = getKey(array);
        for (auto &items: data)
        {
            array[key++] = items.second;
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
        static Php::Extension extension("query_executor", "1.0");

        extension.add<exec>("runSelectCpp", {
                Php::ByVal("schemas", Php::Type::Array),
                Php::ByVal("query", Php::Type::String),
                Php::ByVal("bindings", Php::Type::Array)
        });

        return extension;
    }
};