import csv
from pathlib import Path

class VehicleManager:
    def __init__(self, fname="vehicles.csv"):
        self.file = Path(fname)
        self.columns = ["RegNo", "Type", "Brand", "RatePerDay", "Extra"]
        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                csv.writer(f).writerow(self.columns)

    def add_vehicle(self, reg_no, vtype, brand, rate, extra=0):
        rate = float(rate)
        extra = float(extra)
        vtype = vtype.upper()
        if rate <= 0:
            raise ValueError("Rate must be > 0")
        if vtype == "BIKE" and extra <= 0:
            raise ValueError("Engine CC must be > 0 for Bike")
        if vtype not in ["CAR", "BIKE"]:
            raise ValueError("Type must be CAR or BIKE")

        with open(self.file, "a", newline="", encoding="utf-8") as f:
            csv.writer(f).writerow([reg_no, vtype, brand, rate, extra])

    def calculate_rental(self, reg_no, days):
        days = int(days)
        if days <= 0:
            raise ValueError("Days must be > 0")
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                if row["RegNo"] == reg_no:
                    rate = float(row["RatePerDay"])
                    extra = float(row["Extra"])
                    if row["Type"] == "BIKE" and extra > 500:
                        rate *= 1.1  # 10% surcharge
                    return rate * days
        raise KeyError("Vehicle not found")

    def delete_vehicle(self, reg_no):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))

        new_rows = [r for r in rows if r["RegNo"] != reg_no]
        if len(new_rows) == len(rows):
            raise KeyError("Vehicle not found")

        with open(self.file, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=self.columns)
            writer.writeheader()
            writer.writerows(new_rows)

    def view_vehicles(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))
            if not rows:
                print("No vehicles found")
                return
            print("\nVehicles:")
            for row in rows:
                print(row)

# ----- Menu Driven -----
def run_vehicle_system():
    vm = VehicleManager()
    while True:
        print("\n--- Vehicle Rental System ---")
        print("1. Add Vehicle")
        print("2. Display All Vehicles")
        print("3. Calculate Rental Cost")
        print("4. Delete Vehicle")
        print("5. Exit")
        choice = input("Enter your choice: ")

        try:
            if choice == "1":
                reg_no = input("Enter Reg No: ")
                vtype = input("Enter Type (CAR/BIKE): ")
                brand = input("Enter Brand: ")
                rate = input("Enter Rate per Day: ")
                extra = input("Enter Extra (Engine CC for Bike, 0 for Car): ")
                vm.add_vehicle(reg_no, vtype, brand, rate, extra)
                print("Vehicle Added Successfully")
            elif choice == "2":
                vm.view_vehicles()
            elif choice == "3":
                reg_no = input("Enter Reg No: ")
                days = input("Enter Days: ")
                cost = vm.calculate_rental(reg_no, days)
                print(f"Rental Cost: {cost}")
            elif choice == "4":
                reg_no = input("Enter Reg No to delete: ")
                vm.delete_vehicle(reg_no)
                print(f"Vehicle {reg_no} deleted successfully")
            elif choice == "5":
                print("Exiting...")
                break
            else:
                print("Invalid choice")
        except Exception as e:
            print("Error:", e)

# run_vehicle_system()  # Uncomment to run
