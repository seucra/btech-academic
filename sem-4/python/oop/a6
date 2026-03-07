def inventory_management():
    inventory = {}
    
    while True:
        print("\n--- Inventory Menu ---")
        print("1. Add new Item")
        print("2. Sell Item")
        print("3. Display Low Stock Items")
        print("4. Calculate Total Inventory Value")
        print("5. Display All Items")
        print("6. Exit")
        
        try:
            choice = int(input("Enter your choice: "))
        except ValueError:
            print("Invalid input. Please enter a number.")
            continue

        if choice == 1:
            item_id = input("Enter Item ID: ")
            if item_id in inventory:
                print("Item already exists")
            else:
                name = input("Enter Item Name: ")
                price = float(input("Enter Price: "))
                quantity = int(input("Enter Quantity: "))
                inventory[item_id] = [name, price, quantity]
                print("Item added successfully")
                
        elif choice == 2:
            item_id = input("Enter Item ID: ")
            if item_id not in inventory:
                print("Item does not exist")
            else:
                qty_to_sell = int(input("Enter Quantity to Sell: "))
                if inventory[item_id][2] >= qty_to_sell:
                    inventory[item_id][2] -= qty_to_sell
                    print(f"{qty_to_sell} items sold. Remaining stock: {inventory[item_id][2]}")
                else:
                    print("Insufficient stock")
                    
        elif choice == 3:
            low_stock = {k: v for k, v in inventory.items() if v[2] < 5}
            if not low_stock:
                print("No low stock items")
            else:
                print("Low Stock Items:")
                for k, v in low_stock.items():
                    print(f"ID: {k}, Name: {v[0]}, Stock: {v[2]}")
                    
        elif choice == 4:
            total_value = sum(item[1] * item[2] for item in inventory.values())
            print(f"Total Inventory Value: {total_value}")
            
        elif choice == 5:
            if not inventory:
                print("Inventory is empty")
            else:
                print("-" * 47)
                print(f"{'ID':<5} | {'Name':<15} | {'Price':<7} | {'Quantity'}")
                print("-" * 47)
                for k, v in inventory.items():
                    print(f"{k:<5} | {v[0]:<15} | {v[1]:<7} | {v[2]}")
                print("-" * 47)
                
        elif choice == 6:
            print("Exiting Program...")
            break
        else:
            print("Invalid choice! Please select between 1 and 6.")

if __name__ == "__main__":
    inventory_management()