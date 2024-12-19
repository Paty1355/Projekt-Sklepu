# Object-Oriented Shop System in C++ with SQLite Database

## Overview

This project is a fully functional shop management system developed using object-oriented principles in C++. It integrates an SQLite database to manage data persistently and is designed with a clear division between Administrator and User roles. The system includes core functionalities such as product management, inventory control, and a shopping cart.

### General Features:
- **SQLite Database Integration**: Persistent storage of categories, products, inventory, and user data.
- **Role-Based Access**:
  - **Administrator**: Full control over the system, including category, product, and inventory management.
  - **User**: Can browse products, add items to the cart, and complete purchases.
 
### Classes:
1. **Product**: 
   - Represents individual items in the shop.
   - Stores attributes like name, price, and category ID.
2. **Category**: 
   - Organizes products into groups.
   - Facilitates product filtering and navigation.
3. **Warehouse**: 
   - Manages stock levels for each product.
   - Allows administrators to add, update, or remove stock.
4. **Cart**: 
   - Enables users to add, remove, and view products before purchasing.
   - Each user has their own unique cart, ensuring personalized shopping experiences.

### Additional Features:
- **User-Specific Shopping Carts**: Every user is assigned an individual cart, allowing multiple users to shop simultaneously without data overlap.



