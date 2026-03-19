import csv
from pathlib import Path


class RMS:
    total_records = 0   # class variable

    def __init__(self, columns, fname="RMS.csv"):
        if not columns:
            raise ValueError("Columns list cannot be empty")

        self.file = Path(fname)
        self.columns = columns
        self.pk = columns[0]

        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                writer.writerow(columns)

        self._update_count()

    # ---------- INTERNAL ----------
    def _update_count(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = list(csv.reader(f))
            RMS.total_records = max(0, len(reader) - 1)

    # ---------- CREATE ----------
    def add_record(self, record):
        if len(record) != len(self.columns):
            raise ValueError("Record does not match schema")

        if self.search_record(record[0], silent=True):
            raise ValueError("Record with this ID already exists")

        with open(self.file, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(record)

        self._update_count()

    # ---------- DELETE ----------
    def delete_record(self, record_id):
        deleted = False

        with open(self.file, "r", newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))

        new_rows = []
        for row in rows:
            if row[self.pk] == str(record_id):
                deleted = True
            else:
                new_rows.append(row)

        if not deleted:
            raise KeyError("Record not found")

        with open(self.file, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=self.columns)
            writer.writeheader()
            writer.writerows(new_rows)

        self._update_count()

    # ---------- SUM ----------
    def sum_records(self, cols):
        if not cols:
            raise ValueError("No columns provided")

        for col in cols:
            if col not in self.columns:
                raise KeyError(f"Invalid column: {col}")

        sums = {col: 0.0 for col in cols}

        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = csv.DictReader(f)

            for row in reader:
                for col in cols:
                    try:
                        sums[col] += float(row[col])
                    except ValueError:
                        raise ValueError(f"Non-numeric value in column '{col}'")

        return [sums[col] for col in cols]

def main():
    columns = ["ProductID", "ProductName", "Stock", "Price"]
    system = RMS(columns, "inventory.csv")

    while True:
        print("\n===== Inventory Tracking System =====")
        print("1. Add Product")
        print("2. View Inventory")
        print("3. Search Product")
        print("4. Update Stock")
        print("5. Exit")

        choice = input("Enter choice: ")

        try:
            # ---------- ADD ----------
            if choice == "1":
                record = [input(f"{col}: ") for col in columns]

                # Validate numeric fields
                record[2] = str(int(record[2]))   # Stock
                record[3] = str(float(record[3])) # Price

                system.add_record(record)
                print("✅ Product added")

            # ---------- VIEW ----------
            elif choice == "2":
                system.view_records()

            # ---------- SEARCH ----------
            elif choice == "3":
                pid = input("Enter Product ID: ")
                system.search_record(pid)

            # ---------- UPDATE STOCK ----------
            elif choice == "4":
                pid = input("Enter Product ID: ")

                # get current record
                record = system.search_record(pid, silent=True)
                if not record:
                    raise KeyError("Product not found")

                print(f"Current Stock: {record['Stock']}")

                change = int(input("Enter stock change (+/-): "))

                new_stock = int(record["Stock"]) + change
                if new_stock < 0:
                    raise ValueError("Stock cannot be negative")

                system.update_record(pid, {"Stock": str(new_stock)})
                print("✅ Stock updated")

            # ---------- EXIT ----------
            elif choice == "5":
                print("👋 Exiting...")
                break

            else:
                print("❌ Invalid choice")

        except Exception as e:
            print(f"⚠️ Error: {e}")


if __name__ == "__main__":
    main()
