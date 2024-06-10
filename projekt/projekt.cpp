#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <limits>
#include <algorithm>
#include "sqlite/sqlite3.h"
#include <windows.h>

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
    
public:
    string categoryName;
    Category() {};
    Category(string name) :
        categoryName(name) {};

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

    virtual void search() override {}
};


class Product : public Category {
    friend class Cart;
    friend class Warehouse;
protected:
    int price;
    int quantity;
    string productName;
public:
    Product() {};
    Product(int priceP, int quantityP, string nameP, string nameC) :
        price(priceP), quantity(quantityP), productName(nameP), Category(nameC) {} //Category(nameC)
};


class Warehouse : public Product {
    friend class Cart;
    friend class Category;
protected:
    Product product;
public:
   

    Warehouse(const Product& p) : product(p) {};
    Warehouse() {};

    void set(const Product& p) {
        product = p;
    }

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
            "FOREIGN KEY (productCategory) REFERENCES categories(categoryName));";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO warehouse (productName, price, productCategory, quantity) VALUES ('" + product.productName + "' ," + to_string(product.price) + ", '" + product.categoryName + "' ," + to_string(product.quantity) + ");";
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

        //cout << "Your product has been successfully added!" << endl;
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

        cout << "Your product has been successfully removed!" << endl;
    }

    virtual void search() override {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, search_sql;

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        search_sql = "SELECT * FROM warehouse";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

    void checkQuantity(string searched_item) {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string table_sql, search_sql;

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        search_sql = "SELECT quantity FROM warehouse WHERE productName = '" + searched_item + "'";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

};

int selectCartId(string product_name) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* zErrMsg = 0;
    int rc;
    string sqlCartId;
    int id;

    rc = sqlite3_open("shop.db", &db);
    sqlCartId = "SELECT id FROM cart WHERE nameProduct ='" + product_name + "';";

    rc = sqlite3_prepare_v2(db, sqlCartId.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id;
}

class Cart :public Product {
    friend class Client;
   // int cartId;
   // int cartProducts[20][2];
protected:
    Product product;
public:
    Cart() {};
    Cart(const Product& p, int id) : product(p) {};

    void setValue(const Product& p) {
        product = p;
       // cartId = selectCartId(product.productName);
    }

    /*void setValue(int id, int products[20][2]) {
        cartId = id;
        for (int i = 0;i < 20;i++) {
            for (int j = 0;j < 2;j++) {
                cartProducts[i][j] = products[i][j];
            }
        }
    }*/
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
            "nameProduct TEXT,"
            "quantityProduct INTEGER,"
            "FOREIGN KEY (nameProduct) REFERENCES warehouse(productName));";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        /*for (int i = 0;i < 20;i++) {
                insert_sql = "INSERT OR IGNORE INTO cart (idProduct, quantityProduct) VALUES ('" + to_string(cartProducts[i][0]) + "' '" + to_string(cartProducts[i][1]) + "');";
                rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                break;
            }
        }*/
        insert_sql = "INSERT OR IGNORE INTO cart (nameProduct, quantityProduct) VALUES ('" + product.productName + "', '" + to_string(product.quantity) + "');";
        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg);

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
        remove_sql = "DELETE FROM cart "
            "WHERE nameProduct='" + product.productName + "' ";

        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
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

        search_sql = "SELECT nameProduct, quantityProduct FROM cart INNER JOIN client ON cart.id = client.idCart";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
    void updateWarehouseAddCart(string product) {
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sqlQuantity, sqlUpdate;
        int quantity;

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product + "';";
        rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            quantity = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);

        if (quantity >= 1) {
            for (int i = 0;i < 20;i++) {
                sqlUpdate = "UPDATE warehouse set quantity = " + to_string(quantity - 1) + " where productName=" + product + "; "
                    "SELECT * from COMPANY";
                rc = sqlite3_exec(db, sqlUpdate.c_str(), callback, 0, &zErrMsg);
            }
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
        else {
            cout << "No products in warehouse" << endl;
        }
        sqlite3_close(db);
    }
    void updateWarehouseDeleteCart(string product) {
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sqlQuantity, sqlUpdate;
        int quantity;

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product + "';";
        rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            quantity = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);

        if (quantity >= 1) {
            for (int i = 0;i < 20;i++) {
                sqlUpdate = "UPDATE warehouse set quantity = " + to_string(quantity + 1) + " where productName=" + product + "; "
                    "SELECT * from COMPANY";
                rc = sqlite3_exec(db, sqlUpdate.c_str(), callback, 0, &zErrMsg);
            }
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
        else {
            cout << "No products in warehouse" << endl;
        }
        sqlite3_close(db);
    }

};

class Client :public Abstract {
    //int clientId, cartId;
    string clientLogin, clientPassword;
protected:
    Cart cart;
public:
    Client() {};
    Client(string login, string password) :
        clientLogin(login), clientPassword(password) {}

    void setValue(string login, string password) {
        clientLogin = login;
        clientPassword = password;
    }

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

        insert_sql = "INSERT OR IGNORE INTO client (clientLogin, clientPassword) VALUES ('" + clientLogin + "' , '" + clientPassword+ "');";
       // insert_sql = "INSERT OR IGNORE INTO client (clientLogin, clientPassword, idCart) VALUES ('" + clientLogin + "' , '" + clientPassword + "' , '" + to_string(cart.cartId) + "');";
        //insert_sql = "INSERT OR IGNORE INTO client (id, clientLogin, clientPassword, idCart) VALUES ('" + to_string(clientId) + "' ,'" + clientLogin + "' , '" + clientPassword + "' , '" + to_string(cartId) + "');";
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
        const char* data = "Callback function called";

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        remove_sql = "DELETE FROM client "
            "WHERE clientLogin='" + clientLogin + "' ";

        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
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

        search_sql = "SELECT nameProduct, quantityProduct FROM cart INNER JOIN client ON cart.id = client.idCart";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
    bool checkData() {
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sql;
        rc = sqlite3_open("shop.db", &db);
        string clientLoginResult;

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sql = "SELECT clientLogin FROM client WHERE clientLogin ='" + clientLogin + "' AND clientPassword = '" + clientPassword + "';";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            const unsigned char* login = sqlite3_column_text(stmt, 0);
            if (login) {
                clientLoginResult = reinterpret_cast<const char*>(login);
            }
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        if (clientLogin == clientLoginResult) return true;
        return false;
    }
};
void pause_program() {
    cout << "Press ENTER to continue...";
    cin.ignore(); //ignores any input
    cin.get(); //wait for enter key
}

int choose_option() {
    int option;
    cout << "1. Add category and product" << endl;
    cout << "2. Delete product" << endl;
    cout << "3. Check product quantity" << endl;
    cout << "4. Search products" << endl;
    cout << "5. Return to main menu" << endl;
    cin >> option;
    return option;
}
static int idCart = 3;

Cart cart;

void addToCart() {
    string product_name;
    int number;
    cout << "Choose product, wchich you wanna add to the Cart:" << endl; //coś nie gra z produktu dodawaniem i tu cos sie psuje tez, moze baze ejszcze raz?
    cin >> product_name;
    cout << "How many: " << endl;
    cin >> number;
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* zErrMsg = 0;
    int rc;
    string sqlQuantity, sqlProductCategory, sqlPrice, sqlId, category;
    int quantity, price;

    rc = sqlite3_open("shop.db", &db);

    if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

    cout << product_name << endl;


    sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product_name + "';";
    sqlProductCategory = "SELECT productCategory FROM warehouse WHERE productName ='" + product_name + "';";
    sqlPrice = "SELECT price FROM warehouse WHERE productName ='" + product_name + "';";

    rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        quantity = sqlite3_column_int(stmt, 0);
    }

    rc = sqlite3_prepare_v2(db, sqlProductCategory.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    rc = sqlite3_prepare_v2(db, sqlPrice.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        price = sqlite3_column_int(stmt, 0);
    }

    Product product(price, quantity, product_name, category);
    cart.setValue(product);
    cart.add();
    cart.updateWarehouseAddCart(product_name);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void deleteFromCart() {
    string product_name;
    int number;
    cout << "Choose product, wchich you wanna delete from the Cart:" << endl; //coś nie gra z produktu dodawaniem i tu cos sie psuje tez, moze baze ejszcze raz?
    cin >> product_name;
    cout << "How many: " << endl;
    cin >> number;
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* zErrMsg = 0;
    int rc;
    string sqlQuantity, sqlProductCategory, sqlPrice, sqlId, category;
    int quantity, price;

    rc = sqlite3_open("shop.db", &db);

    if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

    sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product_name + "';";
    sqlProductCategory = "SELECT productCategory FROM warehouse WHERE productName ='" + product_name + "';";
    sqlPrice = "SELECT price FROM warehouse WHERE productName ='" + product_name + "';";

    rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        quantity = sqlite3_column_int(stmt, 0);
    }
    rc = sqlite3_prepare_v2(db, sqlProductCategory.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    rc = sqlite3_prepare_v2(db, sqlPrice.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        price = sqlite3_column_int(stmt, 0);
    }

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        Product product(price, quantity, product_name, category);
        cart.setValue(product);
        cart.remove();
        cart.updateWarehouseDeleteCart(product_name);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main() {
    int choice = 0, loginSuccessfuly = 0,number;
    string password, category_name, product_name, login;
    double price;
    int quantity;
    char answer;

    Warehouse warehouse;
    Client klient;

    do {
        cout << "-------------------MENU---------------------" << endl;
        cout << "1. Admin" << endl;
        cout << "2. User" << endl;
        cout << "3. Exit" << endl;
        cin >> choice;

        if (choice == 1) {
            cout << "Admin password: " << endl;

            // Hide password input
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            DWORD mode = 0;
            GetConsoleMode(hStdin, &mode);
            SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
            cin >> password;
            SetConsoleMode(hStdin, mode);

            if (password == "admin") {
                cout << "Login successfully" << endl;

                bool admin_running = true;
                do {
                    system("cls");
                    choice = choose_option();
                    switch (choice) {
                    case 1:
                        cout << "Enter category name: " << endl;
                        cin >> category_name;
                        {
                            Category category(category_name);
                            category.add();
                        }
                        cout << "Do you want to add a product? (t/n)" << endl;
                        cin >> answer;
                        while (answer != 't' && answer != 'n') {
                            cout << "Please choose only t or n" << endl;
                            cin >> answer;
                        }
                        if (answer == 't') {
                            cout << "Enter product name: " << endl;
                            cin >> product_name;
                            cout << "Enter product price: " << endl;
                            cin >> price;
                            cout << "Enter quantity: " << endl;
                            cin >> quantity;
                            cout << endl;
                            Product product(price, quantity, product_name, category_name);
                            warehouse.set(product);
                            warehouse.add();
                            pause_program();
                        }
                        break;
                    case 2:
                        cout << "Enter the name of the product that you want to delete:" << endl;
                        cin >> product_name;
                        warehouse.remove();
                        pause_program();
                        break;
                    case 3:
                        cout << "What product do you want to find information about?" << endl;
                        cin >> product_name;
                        warehouse.checkQuantity(product_name);
                        pause_program();
                        break;
                    case 4:
                        warehouse.search();
                        pause_program();
                        break;
                    case 5:
                        admin_running = false;
                        break;
                    default:
                        cout << "Invalid choice. Please select a valid option." << endl;
                    }
                } while (admin_running);
            }
            else {
                cout << "Incorrect password." << endl;
            }
        }
        else if (choice == 2) {
            choice = 0;
            system("CLS");
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "3. Exit" << endl;
            cin >> choice;
            cart.add();

            cout << "User login: " << endl;
            cin >> login;
            cout << "User password: " << endl;

            // Hide password input
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            DWORD mode = 0;
            GetConsoleMode(hStdin, &mode);
            SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
            cin >> password;
            SetConsoleMode(hStdin, mode);

            if (choice == 1) {
                klient.setValue(login, password);
                klient.add();
                idCart++;
            }
            else if(choice == 2){
                klient.setValue(login, password);
                loginSuccessfuly = klient.checkData();
                if (loginSuccessfuly == 1) {
                    cout << "Login Successfuly" << endl;
                    choice = 0;
                    cout << "1. Check Cart" << endl;
                    cout << "2. Search Product" << endl;
                    cout << "3. Exit" << endl;
                    cout << "4. Delete Product" << endl;
                    cin >> choice;
                    if (choice == 1) {
                        klient.search();
                    }
                    else if (choice == 2) {
                        warehouse.search();
                        choice = 0;
                        cout << "1. Add Product" << endl;
                        cout << "3. Exit" << endl;
                        cin >> choice;
                        if (choice == 1) {
                            addToCart();
                            choice = 0;
                            cout << "Are you wanna add more products t/n?" << endl;
                            cin >> answer;
                            if (answer == 't') {
                                cout << "How many ?" << endl;
                                cin >> choice;
                                for (int i = 1;i <= choice;i++) {
                                    addToCart();
                                }
                            }  
                        }
                    }
                    else if (choice == 4) {
                        deleteFromCart();
                    }
                }
                else {
                    cout << "Data incorect" << endl;
                    choice = 0;
                }
            }

        }
        else if (choice != 1 && choice != 2) {
            cout << "Choose 1, 2, or 3" << endl;
            break;
            
        }
    } while (choice != 3);
    return 0;

}
