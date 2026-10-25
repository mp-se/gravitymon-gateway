# Validate that all lines in data1.csv have the same number of parameters
import csv

def main():
	input_file = 'data1.csv'
	
	with open(input_file, newline='', encoding='utf-8') as f:
		reader = csv.reader(f)
		row_lengths = [len(row) for row in reader]

	if not row_lengths:
		print('No data found.')
		return

	expected_length = row_lengths[0]
	errors = []
	for idx, length in enumerate(row_lengths, start=1):
		if length != expected_length:
			errors.append((idx, length))

	if errors:
		print(f'Validation failed: Expected {expected_length} columns.')
		for line_num, length in errors:
			print(f'  Line {line_num} has {length} columns.')
	else:
		print('Validation passed: All lines have the same number of columns.')

if __name__ == '__main__':
	main()
