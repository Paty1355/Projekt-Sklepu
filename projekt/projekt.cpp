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
    string categoryName;
public:
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
protected:
    int price;
    int quantity;
    string productName;

public:
    Product(): productName(""), price(0), quantity(0) {};
    Product(int priceP, int quantityP, string nameP, string nameC) :
        price(priceP), quantity(quantityP), productName(nameP), Category(nameC) {} //Category(nameC)
};


class Warehouse : public Product {
    //friend class Cart;
    friend class Category;
protected:
    Product product;
public:

    Warehouse(const Product& p) : product(p) {};
    Warehouse() {};

    void set(const Product& p) {
        product = p;
    }

    void add(string product_name) {
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
            "WHERE id='" + to_string(cartId) + "' ";

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

        search_sql = "SELECT * FROM warehouse";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
    void buy(Warehouse& object) {
        sqlite3* db;
        char* zErrMsg = 0;
        int rc;
        string sql;

        /* Open database */
        rc = sqlite3_open("shop.db", &db);

        if (rc) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

        sql = "UPDATE warehouse set quantity = " + to_string(object.quantity - 1) + " where ID=" + to_string(idProduct) + "; " \
            "SELECT * from COMPANY";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

};

class Client :public Abstract {
    int clientId, cartId;
    string clientLogin, clientPassword;
public:
    Client();
    Client(int idClient, int idCart, string login, string password) :
        clientId(idClient), cartId(idCart), clientLogin(login), clientPassword(password) {}

    void setValue(int idClient, int idCart, string login, string password) {
        clientId = idClient;
        cartId = idCart;
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

        insert_sql = "INSERT OR IGNORE INTO client (id, clientLogin, clientPassword, idCart) VALUES ('" + to_string(clientId) + "' ,'" + clientLogin + "' , '" + clientPassword + "' , '" + to_string(cartId) + "');";
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
            "WHERE id='" + to_string(clientId) + "' ";

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

        search_sql = "SELECT * FROM client";
        rc = sqlite3_exec(db, search_sql.c_str(), callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
};




//int choose_option() {
//    int choice;
//    do {
//        cout << "1. Add category or/and product\n"
//            << "2. Delete product\n"
//            << "3. Search for product\n"
//            << "4. Show products\n"
//            << "5. MENU\n"
//            << "Please enter your choice: ";
//        cin >> choice;
//
//        if (cin.fail() || choice < 1 || choice > 5) {
//            cin.clear();
//            cin.ignore(10000, '\n'); // Zignoruj do 10000 znaków do napotkania znaku nowej linii
//            cout << "Invalid input, please choose 1, 2, 3, 4, or 5.\n";
//        }
//    } while (choice < 1 || choice > 5);
//    return choice;
//}
//
//void pause_program() {
//    cout << "Press Enter to continue..." << endl;
//    cin.ignore(100000, '\n'); // Ignore any remaining input
//    cin.get(); // Wait for the Enter key
//}
//
//
//
//int main(int argc, char* argv[]) {
//    string category_name, product_name;
//    int price, quantity;
//    int choice;
//    char answer;
//    string password, login;
//
//
//    Warehouse warehouse;
//
//    cout << "-------------------MENU---------------------" << endl;
//    cout << "1. Admin" << endl;
//    cout << "2. User" << endl;
//    cin >> choice;
//
//    if (choice == 1) {
//        cout << "Admin password: " << endl;
//
//        //hide password input
//        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
//        DWORD mode = 0;
//        GetConsoleMode(hStdin, &mode);
//        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
//        cin >> password;
//        SetConsoleMode(hStdin, mode);
//
//        if (password == "admin") {
//            choice = 5;
//            cout << "Login successfully" << endl;
//
//            while (true) {
//                system("cls");
//                choice = choose_option();
//                if (choice == 1) {
//                    cout << "Wprowadz nazwe kategorii: " << endl;
//                    cin >> category_name;
//                    Category category(category_name);
//                    category.add();
//                    cout << "Czy chcesz dodac produkt?(t/n)" << endl;
//                    cin >> answer;
//                    while (answer != 't' && answer != 'n') {
//                        cout << "Please choose only t or n" << endl;
//                        cin >> answer;
//                    }
//                    if (answer == 't') {
//                        cout << "Wprowadz nazwe produktu: " << endl;
//                        cin >> product_name;
//                        cout << "Wprowadz cene produktu: " << endl;
//                        cin >> price;
//                        cout << "Wprowadz ilosc: " << endl;
//                        cin >> quantity;
//                        cout << endl;
//                        Product product(price, quantity, product_name, category_name);
//                        warehouse.set(product);
//                        warehouse.add();
//                        pause_program();
//
//
//                    }
//                    if (answer == 'n') {
//                        //choice = 5;
//                    }
//                }
//                else if (choice == 2) {
//                    cout << "Enter the name of the product that you want to delete:" << endl;
//                    cin >> product_name;
//                    warehouse.remove();
//                    cout << "Press ENTER to continue...";
//                    while (cin.get() != '\n') {} // Czeka na naciśnięcie Enter
//                    choice = 5;
//                }
//                else if (choice == 3) {
//                    cout << "What product do you want to find information about?" << endl;
//                    cin >> product_name;
//                    warehouse.checkQuantity(product_name);
//                    cout << "Press ENTER to continue...";
//                    while (cin.get() != '\n') {} // Czeka na naciśnięcie Enter
//                }
//                else if (choice == 4) {
//                    warehouse.search();
//                    cout << "Press ENTER to continue...";
//                    while (cin.get() != '\n') {} // Czeka na naciśnięcie Enter
//                }
//                else {
//                    break;
//                }
//                cout << choice;
//            }
//
//
//
//            //coś nie działa teraz i się nie zapisuje do bazy danych
//           /* do {
//                system("cls");
//                choice = choose_option();
//                switch (choice) {
//                case 1:
//                    cout << "Wprowadz nazwe kategorii: " << endl;
//                    cin >> category_name;
//                    category.set(category_name);
//                    category.add();
//                    cout << "Czy chcesz dodac produkt?(t/n)" << endl;
//                    cin >> answer;
//                    while (answer != 't' && answer != 'n') {
//                        cout << "Please choose only t or n" << endl;
//                        cin >> answer;
//                    }
//                    if (answer == 't') {
//                        cout << "Wprowadz nazwe produktu: " << endl;
//                        cin >> product_name;
//                        cout << "Wprowadz cene produktu: " << endl;
//                        cin >> price;
//                        cout << "Wprowadz ilosc: " << endl;
//                        cin >> quantity;
//                        cout << endl;
//                        Product product(price, quantity, product_name, category_name);
//                        warehouse.set(product);
//                        warehouse.add();
//                        system("pause");
//                    }
//                    break;
//                case 2:
//                    cout << "Enter the name of the product that you want to delete:" << endl;
//                    cin >> product_name;
//                    warehouse.remove();
//                    system("pause");
//                    break;
//                case 3:
//                    cout << "What product do you want to find information about?" << endl;
//                    cin >> product_name;
//                    warehouse.checkQuantity(product_name);
//                    system("pause");
//                    break;
//                case 4:
//                    warehouse.search();
//                    system("pause");
//                    break;
//                default:
//                    break;
//                }
//            } while (choice != 5);*/
//
//
//        }
//    }
//    else if (choice == 2) {
//        cout << "User login: " << endl;
//        cin >> login;
//        cout << "User password: " << endl;
//        //hide password input
//        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
//        DWORD mode = 0;
//        GetConsoleMode(hStdin, &mode);
//        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
//        cin >> password;
//        SetConsoleMode(hStdin, mode);
//
//        // Client klient[count_person];
//        // klient[count_person].setValue(1, 1, login, password);
//       //  klient[count_person].add();
//         //wyswietlenie produktów z magazynu
//    }
//    else {
//        cout << "Choose 1 or 2" << endl;
//        cin >> choice;
//    }
//    return 0;
//}



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


int main() {
    int choice = 0;
    string password, category_name, product_name;
    double price;
    int quantity;
    char answer;

    Warehouse warehouse;

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
                            warehouse.add(product_name);
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
            string login;
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

            // Process user login here...
            cout << "User logged in successfully." << endl;
            pause_program();
        }
        else if (choice != 1 && choice != 2) {
            cout << "Choose 1, 2, or 3" << endl;
            cin >> choice;
        }
    } while (choice != 3);

    return 0;
}