from abc import ABC, abstractmethod
import csv
from pathlib import Path

# ---------- Abstract Base ----------
class Payment(ABC):
    def __init__(self, pay_id, amount):
        self.pay_id = pay_id
        self.amount = float(amount)

    @abstractmethod
    def validate_payment(self):
        pass

    @abstractmethod
    def process_payment(self):
        pass

# ---------- Subclasses ----------
class CreditCardPayment(Payment):
    def __init__(self, pay_id, amount, card_number):
        super().__init__(pay_id, amount)
        self.card_number = card_number

    def validate_payment(self):
        if not self.card_number.isdigit() or len(self.card_number) != 16:
            raise ValueError("Invalid credit card number")
        if self.amount <= 0:
            raise ValueError("Amount must be positive")
        return True

    def process_payment(self):
        self.validate_payment()
        print(f"Credit Card Payment Successful: {self.amount}")
        return True

class UPIPayment(Payment):
    def __init__(self, pay_id, amount, upi_id):
        super().__init__(pay_id, amount)
        self.upi_id = upi_id

    def validate_payment(self):
        if "@" not in self.upi_id:
            raise ValueError("Invalid UPI ID")
        if self.amount <= 0:
            raise ValueError("Amount must be positive")
        return True

    def process_payment(self):
        self.validate_payment()
        print(f"UPI Payment Successful: {self.amount}")
        return True

# ---------- Payment Manager ----------
class PaymentManager:
    def __init__(self, fname="payments.csv"):
        self.file = Path(fname)
        self.columns = ["ID", "Type", "Amount", "Detail"]
        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                csv.writer(f).writerow(self.columns)

    def make_payment(self, payment_obj: Payment):
        payment_obj.process_payment()  # polymorphic call
        pay_type = type(payment_obj).__name__
        detail = getattr(payment_obj, 'card_number', getattr(payment_obj, 'upi_id', ''))
        with open(self.file, "a", newline="", encoding="utf-8") as f:
            csv.writer(f).writerow([payment_obj.pay_id, pay_type, payment_obj.amount, detail])

    def view_payments(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))
            if not rows:
                print("No payments found")
                return
            print("\nPayments:")
            for row in rows:
                print(row)

# ----- Menu Driven -----
def run_payment_system():
    pm = PaymentManager()
    while True:
        print("\n--- Payment Gateway System ---")
        print("1. Make Payment")
        print("2. Display All Payments")
        print("3. Exit")
        choice = input("Enter your choice: ")

        try:
            if choice == "1":
                pay_id = input("Enter Payment ID: ")
                pay_type = input("Enter Type (CREDITCARD/UPI): ").upper()
                amount = input("Enter Amount: ")

                if pay_type == "CREDITCARD":
                    card_number = input("Enter 16-digit Credit Card Number: ")
                    payment = CreditCardPayment(pay_id, amount, card_number)
                elif pay_type == "UPI":
                    upi_id = input("Enter UPI ID (must contain @): ")
                    payment = UPIPayment(pay_id, amount, upi_id)
                else:
                    print("Invalid payment type")
                    continue

                pm.make_payment(payment)

            elif choice == "2":
                pm.view_payments()

            elif choice == "3":
                print("Exiting...")
                break
            else:
                print("Invalid choice")
        except Exception as e:
            print("Error:", e)

run_payment_system()  # Uncomment to run
