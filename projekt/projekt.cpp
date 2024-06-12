#include <iostream> 
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <limits>
#include <algorithm>
#include "sqlite/sqlite3.h"
#include <windows.h>

using namespace std;

static int callback(void* NotUsed, int argc, char** argv, char** azColName) { //Funkcja używana do wypisania zawartości danego zapytania z bazy danych na ekran
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

class Abstract { //Klasa abstrakcyjna
public:
    virtual void add() = 0; //Metoda dodawania rekordów do bazy danych
    virtual void remove() = 0; //Metoda usuwania rekordów do bazy danych
    virtual void search() = 0; //Metoda przeszukiwania rekordów do bazy danych
};

class Category :public Abstract { //klasa Kategorii produktów, dziedziczy publicznie po klasie abstrakcyjnej
public:
    string categoryName; //nazwa kategorii
    Category() {}; //konstruktor domyślny
    Category(string name) : //konstruktor parametryczny
        categoryName(name) {};

    virtual void add() override { //Metoda przeciążona - dodawanie rekordów do bazy danych
        sqlite3* db; //tworzenie bazy
        char* zErrMsg = 0; //komunikat błędu
        int rc;
        string table_sql, insert_sql; //zapytania do bazy danych

        rc = sqlite3_open("shop.db", &db); //otworzenie bazy danych

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db)); //sprawdzanie czy baza została poprawnie otworzona

        table_sql = "CREATE TABLE IF NOT EXISTS categories(" //zapytanie tworzenia tabeli
            "id INTEGER PRIMARY KEY,"
            "categoryName TEXT NOT NULL UNIQUE);";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg); //wykonanie zapytania tworzenia tabeli

        if (rc != SQLITE_OK) { //sprawdzenie czy zapytanie wykonało się poprawnie
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO categories (categoryName) VALUES ('" + categoryName + "');"; //zapytanie dodania rekordów do tabeli
        rc = sqlite3_exec(db, insert_sql.c_str(), callback, 0, &zErrMsg); //wykonanie zapytania

        if (rc != SQLITE_OK) {//sprawdzenie czy zapytanie wykonało się poprawnie
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        sqlite3_close(db); //zamknięcie bazy danych
    }

    virtual void remove() override { //Metoda przeciążona - usuwanie rekordów z bazy danych
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string remove_sql, select_sql;
        const char* data = "Callback function called"; 

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        remove_sql = "DELETE FROM categories " //zapytanie usuwania rekordu
            "WHERE categoryName='" + categoryName + "' ";

        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg); //wykonanie zapytania

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        select_sql = "SELECT * from categories";

        rc = sqlite3_exec(db, select_sql.c_str(), callback, (void*)data, &zErrMsg);
        sqlite3_close(db);
    }

    virtual void search() override {}
};

class Product : public Category {
    friend class Cart; //
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

        cout << "Your product has been successfully added!" << endl;
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
            "WHERE productName='" + product.productName + "' ";

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



    void removeP(string product) {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string remove_sql, select_sql;
        const char* data = "Callback function called";

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        /* Creating Tables*/
        remove_sql = "DELETE FROM warehouse "
            "WHERE productName='" + product + "' ";

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

        search_sql = "SELECT productName, price, quantity, productCategory FROM warehouse;";
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
        //search_sql = "SELECT quantity FROM warehouse WHERE EXISTS(SELECT 1 FROM warehouse WHERE productName = '" + searched_item + "')";



        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

};

class Cart :public Product { //klasa koszyk dziedzicząca po produkcie
    friend class Client; //zadeklarowanie przyjaźni z klasa Client
    int cartId; //id koszyka
    Product product;//tworzenie obiektu produktu
public:
    Cart(){}; //konstruktor domyślny

    void setValueProduct(const Product& p) {//Zapisywanie wartości produktu
        product = p;
    }
    void setCartId(int id) {//Zapisywanie wartości koszyka
        cartId = id;
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
        table_sql = "CREATE TABLE IF NOT EXISTS cart("
            "id INTEGER,"
            "nameProduct TEXT,"
            "quantityProduct INTEGER,"
            "FOREIGN KEY (nameProduct) REFERENCES warehouse(productName));";

        rc = sqlite3_exec(db, table_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        insert_sql = "INSERT OR IGNORE INTO cart (id,nameProduct, quantityProduct) VALUES ("+to_string(cartId)+", '" + product.productName + "', '" + to_string(product.quantity) + "');";
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
            "WHERE nameProduct='" + product.productName + "'";

        rc = sqlite3_exec(db, remove_sql.c_str(), callback, 0, &zErrMsg);

        sqlite3_close(db);
    }

    virtual void search() override {}

    void updateWarehouseAddCart() { //Funkcja aktualizująca stan magazynu
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sqlQuantity, sqlUpdate;
        int quantity=0;

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product.productName + "';";
        rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr); //Przygotowanie zapytania

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            quantity = sqlite3_column_int(stmt, 0);//zapisanie wartości zwróconej przez zapytanie do zmiennej quantity
        }
        sqlite3_finalize(stmt);

        if (quantity >= 1) {
            sqlUpdate = "UPDATE warehouse set quantity = " + to_string(quantity - product.quantity) + " where productName='" + product.productName + "'; ";
            rc = sqlite3_exec(db, sqlUpdate.c_str(), callback, 0, &zErrMsg);
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

    void updateWarehouseDeleteCart() {
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sqlQuantity, sqlUpdate;
        int quantity=0;

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product.productName + "';";
        rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            quantity = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);

        sqlUpdate = "UPDATE warehouse set quantity = " + to_string(quantity + product.quantity) + " where productName='" + product.productName + "'; ";
        rc = sqlite3_exec(db, sqlUpdate.c_str(), callback, 0, &zErrMsg);
        
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        
        sqlite3_close(db);
    }

};

int checkCartId(string login) {//Funkcja zwracająca id koszyka klienta
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* zErrMsg = 0;
    int rc;
    string sqlCartId;
    int id=0;

    rc = sqlite3_open("shop.db", &db);
    sqlCartId = "SELECT id FROM client WHERE clientLogin = '" + login + "'LIMIT 1;";

    rc = sqlite3_prepare_v2(db, sqlCartId.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id;
}

int selectNewCartId() {//funkcja zwracająca kolejne w kolejności id koszyka z bazy danych
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* zErrMsg = 0;
    int rc;
    string sqlCartId;
    int id = 0;

    rc = sqlite3_open("shop.db", &db);
    sqlCartId = "SELECT id FROM client ORDER BY id DESC LIMIT 1;";

    rc = sqlite3_prepare_v2(db, sqlCartId.c_str(), -1, &stmt, nullptr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id + 1;
}

class Client :public Abstract { //klasa Client dziedzicząca po klasie Abstrakcyjnej
    string clientLogin, clientPassword; //login, hasło klienta
    Cart cart; //Tworzenie obiektu Koszyk
public:
    Client() {}; //konstruktor domyślny

    void setValue(string login, string password) {//Ustawanie wartości loginu oraz hasła obiektu
        clientLogin = login;
        clientPassword = password;
    } 
    virtual void add() override {
        cart.setCartId(selectNewCartId()); //ustawienie wartości id koszyka
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

        insert_sql = "INSERT OR IGNORE INTO client (id, clientLogin, clientPassword, idCart) VALUES (" + to_string(cart.cartId) +",'" + clientLogin + "' , '" + clientPassword + "'," + to_string(cart.cartId) + ");";

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

        search_sql = "SELECT nameProduct, quantityProduct FROM cart WHERE id=" + to_string(cart.cartId) + ";";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

    bool checkData() { //Metoda sprawdzająca poprawność wprowadzonych danych logowania
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
        if (clientLogin == clientLoginResult) {
            cart.setCartId(checkCartId(clientLogin));
            system("cls");
            cout << "Login Successfuly" << endl;
            return true;
        } 
        return false;
    }

    void addToCart() {//Metoda odpowiadająca za dodawanie produktów do koszyka
        string product_name;
        int number;
        cout << "Choose product, wchich you wanna add to the Cart:" << endl; 
        cin >> product_name;
        cout << "How many: " << endl;
        cin >> number;
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sqlProductCategory, sqlPrice, sqlId, category, sqlQuantity;
        int price=0, quantity = 0;

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        cout << product_name << endl;

        sqlProductCategory = "SELECT productCategory FROM warehouse WHERE productName ='" + product_name + "';";
        sqlPrice = "SELECT price FROM warehouse WHERE productName ='" + product_name + "';";
        sqlQuantity = "SELECT quantity FROM warehouse WHERE productName ='" + product_name + "';";


        rc = sqlite3_prepare_v2(db, sqlProductCategory.c_str(), -1, &stmt, nullptr); 
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { //pobranie danych o produkcie z bazy danych
            category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }

        rc = sqlite3_prepare_v2(db, sqlPrice.c_str(), -1, &stmt, nullptr); 
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { //pobranie danych o produkcie z bazy danych
            price = sqlite3_column_int(stmt, 0);
        }
        rc = sqlite3_prepare_v2(db, sqlQuantity.c_str(), -1, &stmt, nullptr);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { //pobranie danych o produkcie z bazy danych
            quantity = sqlite3_column_int(stmt, 0);
        }
        if (number < quantity) {
            Product product(price, number, product_name, category); //Utworzenie produktu
            cart.setValueProduct(product); //ustawienie wartości produktu w koszyku
            cart.add(); //dodawanie produktów do koszyka
            cart.updateWarehouseAddCart(); //zaaktualizowanie stanu magazynowego
        }
        else {
            cout << "Not that many product in warehouse" << endl;
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    void deleteFromCart() {
        string product_name;
        int number=0;
        cout << "Choose product, wchich you wanna delete from the Cart:" << endl;
        cin >> product_name;
        sqlite3* db;
        sqlite3_stmt* stmt;
        char* zErrMsg = 0;
        int rc;
        string sqlProductCategory, sqlPrice, sqlId, category,sqlNumber;
        int quantity=0, price=0;

        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sqlProductCategory = "SELECT productCategory FROM warehouse WHERE productName ='" + product_name + "';";
        sqlPrice = "SELECT price FROM warehouse WHERE productName ='" + product_name + "';";
        sqlNumber = "SELECT quantityProduct FROM cart WHERE nameProduct ='" + product_name + "';";

        rc = sqlite3_prepare_v2(db, sqlProductCategory.c_str(), -1, &stmt, nullptr);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }

        rc = sqlite3_prepare_v2(db, sqlPrice.c_str(), -1, &stmt, nullptr);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            price = sqlite3_column_int(stmt, 0);
        }

        rc = sqlite3_prepare_v2(db, sqlNumber.c_str(), -1, &stmt, nullptr);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            number = sqlite3_column_int(stmt, 0);
        }

        Product product(price, number, product_name, category);
        cart.setValueProduct(product);
        cart.remove();
        cart.updateWarehouseDeleteCart();
        
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};

void pause_program() {
    cout << "Press ENTER to continue...";
    cin.ignore(); //ignores any input
    cin.get(); //wait for enter key
}

char choose_option() {
    char option;
    cout << "1. Add category and product" << endl;
    cout << "2. Delete product" << endl;
    cout << "3. Check product quantity" << endl;
    cout << "4. Search products" << endl; 
    cout << "9. Return to main menu" << endl;
    cin >> option;
    return option;
}

int main() {
    int loginSuccessfuly = 0;
    char choice = '0';
    string password, category_name, product_name, login;
    double price;
    int quantity;
    char answer = ' ';
    bool adminRunning = false, adminOptionrunning = false;
    bool userRunning = false, userOptionRunning = false;

    Warehouse warehouse;
    Client klient;

    do {
        cout << "-------------------MENU---------------------" << endl;
        cout << "1. Admin" << endl;
        cout << "2. User" << endl;
        cout << "9. Exit" << endl;
        cin >> choice;
        if (choice != '9' && choice != '2' && choice != '1') {
            system("cls");
            cout << "Invalid choice. Please select 1, 2 or 9: " << endl;
            pause_program();
            choice = '0';
        }
        if (choice == '1') {
            do {
                adminRunning = true;
            system("cls");
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

                adminOptionrunning = true;
                do {

                    system("cls");
                    //adminOptionrunning = false;
                    choice = choose_option();
                    switch (choice) {
                    case '1':
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
                    case '2':
                        cout << "Enter the name of the product that you want to delete:" << endl;
                        cin >> product_name;
                        //Product product1(price, quantity, product_name, category_name);
                        //warehouse.set(product1);
                        warehouse.removeP(product_name);
                        pause_program();
                        break;
                    case '3':
                        cout << "What product do you want to find information about?" << endl;
                        cin >> product_name;
                        warehouse.checkQuantity(product_name);
                        pause_program();
                        break;
                    case '4':
                        warehouse.search();
                        pause_program();
                        break;
                    case '9':
                        adminRunning = false;
                        adminOptionrunning = false;
                        choice = '0';
                        break;
                    default:
                        cout << "Invalid choice. Please select a valid option." << endl;
                        pause_program();
                        break;

                    }
                } while (adminOptionrunning);
            }
            else if (password == "9") {
                choice = '0';
                adminOptionrunning = false;
                adminRunning = false;
                continue;
            }
            else {
                cout << "Incorrect password. Try again." << endl;
                //choice = '1';
                pause_program();
                adminRunning = true;
            }
            } while (adminRunning);
        }
        else if (choice == '2') {
            do {
            userRunning = true;
            system("cls");
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "9. Exit" << endl;
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '9') {
                cout << "Invalid choice. Please select 1, 2 or 9:" << endl;
                cin >> choice;
            }
            if (choice == '9') { 
                choice = '0';
                userRunning = false;
                system("cls");
                continue;
            }
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
                if (choice == '1') {
                    klient.setValue(login, password);
                    klient.add();
                    cout << "Register Successfuly, Login" << endl;
                    pause_program();
                }
                else if (choice == '2') {
                    klient.setValue(login, password);
                    loginSuccessfuly = klient.checkData();
                    if (loginSuccessfuly == false) {
                        choice = '9';
                    }
                    do {
                        if (loginSuccessfuly == 1) {
                            userOptionRunning = true;
                            cout << "1. Check Cart" << endl;
                            cout << "2. Show Product" << endl; //Search
                            cout << "3. Delete Product" << endl;
                            cout << "9. Exit" << endl;
                            cin >> choice;
                            while (choice != '1' && choice != '2' && choice!='3' && choice != '9') {
                                cout << "Invalid choice. Please select 1, 2, 3 or 9:" << endl;
                                cin >> choice;
                            }
                            if (choice != '9') {
                                if (choice == '1') {
                                    klient.search();
                                }
                                else if (choice == '2') {
                                    warehouse.search();
                                    cout << "1. Add Product" << endl;
                                    cout << "9. Exit" << endl;
                                    cin >> choice;
                                    while (choice != '1' && choice != '9') {
                                        cout << "Invalid choice. Please select 1 or 9:" << endl;
                                        cin >> choice;
                                    }
                                    if (choice == '1') {
                                        klient.addToCart();
                                        choice = '0';
                                        cout << "Are you wanna add more products t/n?" << endl;
                                        cin >> answer;
                                        while (answer != 't' && answer != 'n') {
                                            cout << "Invalid choice. Please choose t or n" << endl;
                                            cin >> answer;
                                        }
                                        if (answer == 't') {
                                            cout << "How many products ?" << endl;
                                            cin >> choice;
                                            for (int i = 1;i <= static_cast<int>(choice)-48;i++) {
                                                klient.addToCart();
                                            }
                                        }
                                    }
                                    else if(choice == '9') {
                                        choice = '0';
                                        system("cls");
                                    }
                                }
                                else if (choice == '3') {
                                    klient.search();
                                    cout << "1. Delete Product" << endl;
                                    cout << "9. Exit" << endl;
                                    cin >> choice;
                                    while (choice != '1' && choice != '9') {
                                        cout << "Invalid choice. Please select 1 or 9:" << endl;
                                        cin >> choice;
                                    }
                                    if (choice == '1') {
                                        klient.deleteFromCart();
                                        cout << "Are you wanna add delete more products t/n?" << endl;
                                        cin >> answer;
                                        while (answer != 't' && answer != 'n') {
                                            cout << "Invalid choice. Please choose t or n" << endl;
                                            cin >> answer;
                                        }
                                        if (answer == 't') {
                                            cout << "How many ?" << endl;
                                            cin >> choice;
                                            for (int i = 1;i <= static_cast<int>(choice) - 48;i++) {
                                                klient.deleteFromCart();
                                            }
                                        }
                                    }
                                    else if (choice == '9') {
                                        choice = '0';
                                        system("cls");
                                    }
                                }
                            }
                            else {
                                userOptionRunning = false;
                            }
                        }
                    } while (userOptionRunning);
                }
                else {
                    cout << "Data incorect" << endl;
                    choice = '0';
                    userRunning = false;
                }  
            } while (userRunning);
        }
        else if (choice == '9') {
            break;
        }
        else {
            continue;
        }
    } while (choice != '9' && choice != '2' && choice != '1');
    return 0;

}
