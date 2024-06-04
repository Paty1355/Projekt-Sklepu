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

    virtual void search() = 0;
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

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

    virtual void remove() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string remove_sql, select_sql;
        const char* data = "Callback function called"; //zastanow sie nad tym

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

//class Product : public Category {
//    int price;
//    string productName;
//public:
//    Product(int price, string nameP, string nameC) :
//        price(price), productName(nameP), Category(nameC) {}
//
//    virtual void add() override {
//        sqlite3* db;
//        char* zErrMsg = 0;
//        int rc;
//        string table_sql, insert_sql;
//        const char* data = "Callback function called";
//
//        /* Open database */
//        rc = sqlite3_open("shop.db", &db);
//
//        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//
//        /* Creating Tables*/
//        table_sql = "CREATE TABLE IF NOT EXISTS products("
//            "id INTEGER PRIMARY KEY,"
//            "productName TEXT NOT NULL UNIQUE,"
//            "price INTEGER,"
//            "productCategory TEXT,"
//            "FOREIGN KEY(productCategory) REFERENCES categories(categoryName))";
//
//        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);
//
//        if (rc != SQLITE_OK) {
//            fprintf(stderr, "SQL error: %s\n", zErrMsg);
//            sqlite3_free(zErrMsg);
//        }
//
//        insert_sql = "INSERT OR IGNORE INTO products (productName, price, productCategory) VALUES ('" + productName + "' ," + to_string(price) +", '" + categoryName + "');";
//        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);
//
//        if (rc != SQLITE_OK) {
//            fprintf(stderr, "SQL error: %s\n", zErrMsg);
//            sqlite3_free(zErrMsg);
//        }
//
//        /* Create SQL statement */
//        string select_sql = "SELECT * from products";
//
//        /* Execute SQL statement */
//        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);
//
//        sqlite3_close(db);
//    }
//
//    virtual void remove() override {
//        sqlite3* db;
//        char* zErrMsg = 0;
//        int rc;
//        string remove_sql, select_sql;
//        const char* data = "Callback function called"; //zastanow sie nad tym
//
//        /* Open database */
//        rc = sqlite3_open("shop.db", &db);
//
//        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//
//        /* Creating Tables*/
//        remove_sql = "DELETE FROM products "
//            "WHERE productName='" + productName + "' ";
//
//        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg);
//
//        if (rc != SQLITE_OK) {
//            fprintf(stderr, "SQL error: %s\n", zErrMsg);
//            sqlite3_free(zErrMsg);
//        }
//        /* Create SQL statement */
//        select_sql = "SELECT * from products";
//
//        /* Execute SQL statement */
//        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);
//        sqlite3_close(db);
//    }
//};






class Warehouse: public Category {
    friend class Cart;
protected:
    int price;
    int quantity;
    string productName;
public:
    Warehouse(int priceP, int quantityP, string nameP, string nameC) :
        price(priceP), quantity(quantityP), productName(nameP), Category(nameC) {}

    virtual void add() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, insert_sql;
        const char* data = "Callback function called";

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        table_sql = "CREATE TABLE IF NOT EXISTS warehouse("
            "id INTEGER PRIMARY KEY,"
            "productName TEXT NOT NULL UNIQUE,"
            "price INT,"
            "quantity INT,"
            "productCategory TEXT,"
            "FOREIGN KEY (productCategory) REFERENCES categories(categoryName);";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO warehouse (productName, price, productCategory, quantity) VALUES ('" + productName + "' ," + to_string(price) + ", '" + categoryName + "' ," + to_string(quantity) + ");";
        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        /* Create SQL statement */
        string select_sql = "SELECT * FROM warehouse";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);

        sqlite3_close(db);
    }

    virtual void remove() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string remove_sql, select_sql;
        const char* data = "Callback function called"; //zastanow sie nad tym

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        remove_sql = "DELETE FROM warehouse "
            "WHERE productName='" + productName + "' ";

        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        /* Create SQL statement */
        select_sql = "SELECT * FROM warehouse";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);
        sqlite3_close(db);
    }

    virtual void search() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, search_sql;

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

       // search_sql = "SELECT quantity FROM warehouse WHERE productName = '" + searched_item + "'";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

};
class Cart :public Abstract {
    int cartId;
    int idProduct;
public:
    Cart(int id, int product) :
        cartId(id), idProduct(product) {}

    virtual void add() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, insert_sql;
        const char* data = "Callback function called";

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        table_sql = "CREATE TABLE IF NOT EXISTS cart("
            "id INTEGER PRIMARY KEY,"
            "idProduct INTEGER,"
            "FOREIGN KEY (idProduct) REFERENCES warehouse(id));";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO cart (idProduct) VALUES ('" + to_string(idProduct) + "');";
        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        /* Create SQL statement */
        string select_sql = "SELECT * FROM cart";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);

        sqlite3_close(db);
    }
    virtual void remove() override {}
    virtual void search() override {}
};

class Client :public Abstract {
    int clientId, cartId;
    string clientLogin, clientPassword;
public:
    Client(int idClient, int idCart, string login, string password) :
        clientId(idClient),cartId(idCart), clientLogin(login), clientPassword(password){}

    virtual void add() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, insert_sql;
        const char* data = "Callback function called";

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        table_sql = "CREATE TABLE IF NOT EXISTS client("
            "id INTEGER PRIMARY KEY,"
            "clientLogin TEXT NOT NULL UNIQUE,"
            "clientPassword TEXT NOT NULL,"
            "idCart INTEGER UNIQUE,"
            "FOREIGN KEY (idCart) REFERENCES cart(id));"; 

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO client (id, clientLogin, clientPassword, idCart) VALUES ('"+to_string(clientId) + "' ,'" + clientLogin + "' , '" + clientPassword+ "' , '" +to_string(cartId)+ "');";
        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        /* Create SQL statement */
        string select_sql = "SELECT * FROM client";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);

        sqlite3_close(db);
    }
    virtual void remove() override {}
    virtual void search() override {}
};

int main(int argc, char* argv[]) {
    string searched_item;
    Warehouse warehouse();

    Client klient1(1,1,"klient1", "haslo1");
    klient1.add();

    Cart koszyk1(1,1);
    koszyk1.add();

    //Category zywnosc("banan");
   // zywnosc.add();
   // Category zywnosc2("truskawka");
   // zywnosc2.remove();
   // Product produkt1(2, "Truskawka", "banan");
  //  produkt1.add();
   // produkt1.remove();
    return 0;
}
