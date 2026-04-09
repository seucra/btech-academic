import csv
from pathlib import Path

class RMS:
    total_records = 0  # Class variable to track total records

    def __init__(self, columns, fname="employees.csv"):
        if not columns:
            raise ValueError("Columns list cannot be empty")

        self.file = Path(fname)
        self.columns = columns
        self.pk = columns[0]

        # Create file with header if it does not exist
        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                writer.writerow(columns)

        self._update_count()

    def _update_count(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = list(csv.reader(f))
            RMS.total_records = max(0, len(reader) - 1)

    def add_record(self, record):
        record = [str(r).strip() for r in record]

        if len(record) != len(self.columns):
            raise ValueError("Record does not match schema")

        if self.search_record(record[0], silent=True):
            raise ValueError(f"Record with EmployeeID {record[0]} already exists")

        with open(self.file, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(record)

        self._update_count()

    def delete_record(self, record_id):
        record_id = str(record_id).strip()
        deleted = False
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))

        new_rows = []
        for row in rows:
            if row[self.pk].strip() == record_id:
                deleted = True
            else:
                new_rows.append(row)

        if not deleted:
            raise KeyError(f"Record with EmployeeID {record_id} not found")

        with open(self.file, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=self.columns)
            writer.writeheader()
            writer.writerows(new_rows)

        self._update_count()

    def view_records(self, start=1, end=None):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = list(csv.reader(f))
        if len(reader) <= 1:
            print("No records found")
            return

        if end is None:
            end = len(reader) - 1
        if start < 1 or start > end:
            raise ValueError("Invalid record range")

        print("\n" + "=" * 40)
        print("\t".join(self.columns))
        print("=" * 40)
        for row in reader[start:end + 1]:
            print("\t".join(row))

    def search_record(self, record_id, silent=False):
        record_id = str(record_id).strip()
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                if row[self.pk].strip() == record_id:
                    if not silent:
                        print("\n🔍 Record Found:")
                        for k, v in row.items():
                            print(f"{k}: {v}")
                    return row
        if not silent:
            print(f"Record with EmployeeID {record_id} not found")
        return None

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
    columns = ["EmployeeID", "Name", "Department", "Salary"]
    system = RMS(columns, "employees.csv")

    while True:
        print("\n===== Employee RMS =====")
        print("1. Add Employee")
        print("2. View Employees")
        print("3. Sum Columns")
        print("4. Total Records")
        print("5. Search Employee")
        print("6. Delete Employee")
        print("7. Exit")

        choice = input("Enter choice: ").strip()

        try:
            if choice == "1":
                record = [input(f"{col}: ").strip() for col in columns]
                system.add_record(record)
                print("✅ Employee Added")

            elif choice == "2":
                system.view_records()

            elif choice == "3":
                cols = input("Enter numeric columns to sum (comma separated): ").split(",")
                cols = [c.strip() for c in cols]
                result = system.sum_records(cols)
                print("📊 Sums:", dict(zip(cols, result)))

            elif choice == "4":
                print(f"📦 Total Records: {RMS.total_records}")

            elif choice == "5":
                eid = input("Enter EmployeeID to search: ").strip()
                system.search_record(eid)

            elif choice == "6":
                eid = input("Enter EmployeeID to delete: ").strip()
                system.delete_record(eid)
                print("🗑️ Employee Deleted")

            elif choice == "7":
                print("👋 Exiting...")
                break

            else:
                print("❌ Invalid choice")

        except Exception as e:
            print(f"⚠️ Error: {e}")


if __name__ == "__main__":
    main()
