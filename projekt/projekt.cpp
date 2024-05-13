#include <iostream>
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
    virtual void add() {};
    virtual void remove() {};
};

class Category:public Abstract {
protected:
    string categoryName;
public:
    Category(string name) :
        categoryName(name) {}

    virtual void add() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        const char* sql;

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        else fprintf(stdout, "Opened database successfully\n");

        /* Creating Tables*/
        sql = "CREATE TABLE IF NOT EXISTS categories("  \
            "id int AUTO_INCREMENT PRIMARY KEY," \
            "categoryName VARCHAR(255) NOT NULL UNIQUE);" \
            "INSERT INTO categories (categoryName) "  \
            "VALUES ('test') ON DUPLICATE KEY UPDATE categoryName = 'delete';" //\ nie ma w sqlite sprobuj upsert obczaic
            "DELETE from categories where categoryName='delete';";
            
        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else {
            fprintf(stdout, "Table created successfully\n");
        }
        sqlite3_close(db);
    }
    virtual void remove() override {

    }
};
class Product : protected Category {
    int price;
    string productName;

    //"CREATE TABLE IF NOT EXISTS products("  \
    //    "id int AUTO_INCREMENT PRIMARY KEY," \
    //    "productName TEXT NOT NULL," \
    //    "price int NOT NULL);" \

public:
    Product(int price, string nameP, string nameC) :
        price(price), productName(nameP), Category(nameC) {}
};

int main(int argc, char* argv[]) {
    Category zywnosc("zywnosc");
    zywnosc.add();
    return 0;
}