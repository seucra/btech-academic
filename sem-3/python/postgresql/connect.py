from faker import Faker
import psycopg2 as p
from random import randint

fake = Faker()

#connect
conn = p.connect(database="cllg", user="postgres", password="seucra", host="localhost", port="5432")
cur = conn.cursor()

usedprns = set()
usedrolls = set()
usedcontacts = set()

for _ in range(1000):
	prn = randint(10000, 99999)
	while prn in usedprns:
		prn = randint(10000, 99999)
	usedprns.add(prn)

	rollno = randint(1, 5000)
	while rollno in usedrolls:
		rollno = randint(1, 5000)
	usedrolls.add(rollno)

	contact = randint(7000000000,9999999999)
	while contact in usedcontacts:
		contact = randint(7000000000, 9999999999)
	usedcontacts.add(contact)

	firstname = fake.first_name()
	lastname = fake.last_name()
	email = f"{firstname.lower()}.{lastname.lower()}@example.com"

	address = fake.address().replace('\n', ', ')
	dob = fake.date_of_birth(minimum_age=18, maximum_age=25)
	majorid = randint(1,10)

	cur.execute("""
		INSERT INTO students (prn, rollno, firstname, lastname, email, address, dob, majorid, contact)
		VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s	)
		""", (prn, rollno, firstname, lastname, email, address, dob, majorid, contact))

conn.commit()

cur.execute("Select * from students;")

rows =cur.fetchall()
for row in rows:
	print(row)

cur.close()
conn.close()
