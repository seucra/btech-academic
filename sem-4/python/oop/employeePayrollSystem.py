import csv
from pathlib import Path

class EmployeeManager:
    def __init__(self, fname="employees.csv"):
        self.file = Path(fname)
        self.columns = ["ID", "Name", "Salary", "Rating"]
        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                csv.writer(f).writerow(self.columns)

    def add_employee(self, emp_id, name, salary, rating):
        salary = float(salary)
        rating = int(rating)
        if salary <= 0:
            raise ValueError("Salary must be > 0")
        if not (1 <= rating <= 5):
            raise ValueError("Rating must be 1-5")

        with open(self.file, "a", newline="", encoding="utf-8") as f:
            csv.writer(f).writerow([emp_id, name, salary, rating])

    def calculate_net_salary(self, emp_id):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                if row["ID"] == emp_id:
                    salary = float(row["Salary"])
                    rating = int(row["Rating"])
                    bonus = {5:0.2, 4:0.15, 3:0.1, 2:0.05, 1:0}[rating]
                    return salary * (1 + bonus)
        raise KeyError("Employee not found")

    def view_employees(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))
            if not rows:
                print("No employees found")
                return
            print("\nEmployees:")
            for row in rows:
                print(row)

# ----- Menu Driven -----
def run_employee_system():
    em = EmployeeManager()
    while True:
        print("\n--- Employee Payroll System ---")
        print("1. Add Employee")
        print("2. Calculate Net Salary")
        print("3. Display Employee Details")
        print("4. Exit")
        choice = input("Enter your choice: ")

        try:
            if choice == "1":
                emp_id = input("Enter Employee ID: ")
                name = input("Enter Name: ")
                salary = input("Enter Salary: ")
                rating = input("Enter Rating (1-5): ")
                em.add_employee(emp_id, name, salary, rating)
                print("Employee Added Successfully")
            elif choice == "2":
                emp_id = input("Enter Employee ID: ")
                net = em.calculate_net_salary(emp_id)
                print(f"Net Salary: {net}")
            elif choice == "3":
                em.view_employees()
            elif choice == "4":
                print("Exiting...")
                break
            else:
                print("Invalid choice")
        except Exception as e:
            print("Error:", e)

run_employee_system()  # Uncomment to run
