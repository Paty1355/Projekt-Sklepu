#include <iostream>
#include <string>
#include "sqlite/sqlite3.h"

using namespace std;

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

class Abstract {
public:
    virtual void add() = 0;
    virtual void remove() = 0;
};

class Category :public Abstract {
protected:
    string categoryName;
public:
    Category(string name) :
        categoryName(name) {}

    virtual void add() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, insert_sql;
        static int licz = 1;

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        table_sql = "CREATE TABLE IF NOT EXISTS categories("
            "id INTEGER PRIMARY KEY,"
            "categoryName TEXT NOT NULL UNIQUE);";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO categories (categoryName) VALUES ('" + categoryName + "');";
        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);

        licz++;

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            licz--;
        }
        sqlite3_close(db);
    }

    virtual void remove() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string remove_sql, select_sql;
        const char* data = "Callback function called"; 

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        remove_sql = "DELETE FROM categories "
            "WHERE categoryName='" + categoryName + "' ";

        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        /* Create SQL statement */               
        select_sql = "SELECT * from categories";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);
        sqlite3_close(db);
    }
};

class Product : public Category {
    int price;
    string productName;

public:
    Product(int price, string nameP, string nameC) :
        price(price), productName(nameP), Category(nameC) {}

    // Implement add and remove methods for Product
};

int main(int argc, char* argv[]) {
    Category zywnosc("banan");
    zywnosc.add();
    Category zywnosc2("truskawka");
    zywnosc2.remove();
    return 0;
}
