import csv
import json
from pathlib import Path


class RMS:
    def __init__(self, columns, fname="RMS.csv", rules=None, types=None):
        if not columns:
            raise ValueError("Columns required")

        self.file = Path(fname)
        self.columns = columns
        self.pk = columns[0]

        self.rules = rules or {}
        self.types = types or {}
        self.computed = {}
        self.index = {}

        if not self.file.exists():
            with open(self.file, "w", newline="", encoding="utf-8") as f:
                csv.writer(f).writerow(columns)

        self._update_count()
        self.build_index()

    # ---------- INTERNAL ----------
    def _update_count(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            self.total_records = max(0, len(list(csv.reader(f))) - 1)

    def _validate(self, record):
        for col, rule in self.rules.items():
            if col in record and not rule(record[col]):
                raise ValueError(f"Validation failed for {col}")

    def _cast(self, record):
        for col, typ in self.types.items():
            if col in record:
                record[col] = typ(record[col])
        return record

    def _read_all(self):
        with open(self.file, "r", newline="", encoding="utf-8") as f:
            return list(csv.DictReader(f))

    def _write_all(self, rows):
        with open(self.file, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=self.columns)
            writer.writeheader()
            writer.writerows(rows)

    # ---------- CREATE ----------
    def add_record(self, record):
        if len(record) != len(self.columns):
            raise ValueError("Schema mismatch")

        record_dict = dict(zip(self.columns, record))
        record_dict = self._cast(record_dict)
        self._validate(record_dict)

        if self.fast_search(record[0]):
            raise ValueError("Duplicate ID")

        with open(self.file, "a", newline="", encoding="utf-8") as f:
            csv.writer(f).writerow(record_dict.values())

        self._update_count()
        self.build_index()

    # ---------- READ ----------
    def view_records(self):
        rows = self._read_all()
        if not rows:
            raise ValueError("No records")

        headers = self.columns + list(self.computed.keys())

        print("\n" + "=" * 60)
        print("\t".join(headers))
        print("=" * 60)

        for row in rows:
            row = self._cast(row)
            computed_vals = [str(func(row)) for func in self.computed.values()]
            print("\t".join([str(row[c]) for c in self.columns] + computed_vals))

    # ---------- SEARCH ----------
    def search_record(self, record_id):
        row = self.fast_search(record_id)
        if not row:
            raise KeyError("Record not found")

        row = self._cast(row)

        print("\n🔍 Record Found:")
        for k, v in row.items():
            print(f"{k}: {v}")

        return row

    def fast_search(self, record_id):
        return self.index.get(str(record_id))

    def build_index(self):
        self.index = {}
        for row in self._read_all():
            self.index[row[self.pk]] = row

    # ---------- UPDATE ----------
    def update_record(self, record_id, updates):
        rows = self._read_all()
        found = False

        for row in rows:
            if row[self.pk] == str(record_id):
                row.update(updates)
                row = self._cast(row)
                self._validate(row)
                found = True

        if not found:
            raise KeyError("Record not found")

        self._write_all(rows)
        self.build_index()

    # ---------- DELETE ----------
    def delete_record(self, record_id):
        rows = self._read_all()
        new_rows = [r for r in rows if r[self.pk] != str(record_id)]

        if len(rows) == len(new_rows):
            raise KeyError("Record not found")

        self._write_all(new_rows)
        self._update_count()
        self.build_index()

    # ---------- FILTER ----------
    def filter_records(self, condition_fn):
        results = []
        for row in self._read_all():
            row = self._cast(row)
            if condition_fn(row):
                results.append(row)
        return results

    # ---------- SORT ----------
    def sort_records(self, col, reverse=False):
        rows = self._read_all()
        return sorted(rows, key=lambda r: float(r[col]), reverse=reverse)

    # ---------- AGGREGATE ----------
    def aggregate(self, col, op):
        values = []
        for row in self._read_all():
            values.append(float(row[col]))

        if not values:
            raise ValueError("No data")

        if op == "sum":
            return sum(values)
        elif op == "avg":
            return sum(values) / len(values)
        elif op == "min":
            return min(values)
        elif op == "max":
            return max(values)
        else:
            raise ValueError("Invalid operation")

    # ---------- COMPUTED ----------
    def add_computed(self, name, func):
        self.computed[name] = func

    # ---------- EXPORT ----------
    def export_json(self):
        return json.dumps(self._read_all(), indent=2)

    # ---------- PYTHONIC ----------
    def __len__(self):
        return self.total_records

    def __iter__(self):
        yield from self._read_all()
