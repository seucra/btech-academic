import csv
from pathlib import Path


class RMS:
    def __init__(self, columns, fname="RMS.csv"):
        if not columns:
            raise ValueError("Columns list cannot be empty")

        self.file = Path(fname)
        self.columns = columns
        self.pk = columns[0]  # first column = primary key

        # Create file with header if not exists
        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                writer.writerow(columns)

    # ---------- CREATE ----------
    def add_record(self, record):
        if len(record) != len(self.columns):
            raise ValueError("Record does not match schema")

        if self.search_record(record[0], silent=True):
            raise ValueError("Record with this ID already exists")

        with open(self.file, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(record)

    # ---------- READ ALL ----------
    def view_records(self, start=1, end=None):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = list(csv.reader(f))

        if len(reader) <= 1:
            raise ValueError("No records found")

        if end is None:
            end = len(reader) - 1

        if start < 1 or start > end:
            raise ValueError("Invalid record range")

        print("\n" + "=" * 40)
        print("\t".join(self.columns))
        print("=" * 40)

        for row in reader[start:end + 1]:
            print("\t".join(row))

    # ---------- SEARCH ----------
    def search_record(self, record_id, silent=False):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = csv.DictReader(f)

            for row in reader:
                if row[self.pk] == str(record_id):
                    if not silent:
                        print("\n🔍 Record Found:")
                        for k, v in row.items():
                            print(f"{k}: {v}")
                    return row

        if not silent:
            raise KeyError("Record not found")
        return None

    # ---------- UPDATE ----------
    def update_record(self, record_id, updates: dict):
        updated = False

        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = list(csv.DictReader(f))

        for row in reader:
            if row[self.pk] == str(record_id):
                for key, value in updates.items():
                    if key not in self.columns:
                        raise KeyError(f"Invalid column: {key}")
                    row[key] = value
                updated = True

        if not updated:
            raise KeyError("Record not found")

        with open(self.file, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=self.columns)
            writer.writeheader()
            writer.writerows(reader)

    # ---------- DELETE ----------
    def delete_record(self, record_id):
        deleted = False

        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = list(csv.DictReader(f))

        new_rows = []
        for row in reader:
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

def main():
    columns = ["id", "name", "course", "fees"]
    system = RMS(columns)

    while True:
        print("\n===== RMS =====")
        print("1. Add")
        print("2. View")
        print("3. Search")
        print("4. Update")
        print("5. Delete")
        print("6. Exit")

        choice = input("Enter choice: ")

        try:
            if choice == "1":
                record = [input(f"{col}: ") for col in columns]
                system.add_record(record)
                print("✅ Added")

            elif choice == "2":
                system.view_records()

            elif choice == "3":
                rid = input("Enter ID: ")
                system.search_record(rid)

            elif choice == "4":
                rid = input("Enter ID: ")
                updates = {}
                for col in columns[1:]:
                    val = input(f"New {col} (leave blank to skip): ")
                    if val:
                        updates[col] = val

                system.update_record(rid, updates)
                print("✅ Updated")

            elif choice == "5":
                rid = input("Enter ID: ")
                system.delete_record(rid)
                print("🗑️ Deleted")

            elif choice == "6":
                break

            else:
                print("❌ Invalid choice")

        except Exception as e:
            print(f"⚠️ Error: {e}")


if __name__ == "__main__":
    main()
