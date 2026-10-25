# Simple CSV converter: reads brewlogger.csv and writes to data.csv
import csv

def main():
	input_file = 'brewlogger.csv'
	output_file = 'data.csv'
	
    # Constants not found in the input data
	const_format_version = 1
	const_format_type = "Gravitymon"
	const_format_source = "HTTP Post"
	const_format_id = "AABBCC"
	const_format_tx_power = 0
	const_format_token = ''
	const_format_interval = 900

    # Format of the outputfile for gravity data
    # -----------------------------------------
    # 0, Format Version (1)
    # 1, Type
    # 2, Source
    # 3, Created (timestamp)
    # 4, ID
    # 5, Name
    # 6, Token
    # 7, Temperature (C)
    # 8, Gravity (SG)
    # 9, Angle
    # 10, Battery
    # 11, Tx Power
    # 12, Rssi
    # 13, Interval

	# Specify the columns you want in the output (by name or new columns)
	output_columns = ['FormatVersion', 'Type', 'Source', 'Created', 'ID', 'Name', 'Token', 'Temperature', 'Gravity', 'Angle', 'Battery', 'TxPower', 'RSSI', 'Interval'] 

	def compute_format_version(row):
		return const_format_version

	def compute_type(row):
		return const_format_type 

	def compute_source(row):
		return const_format_source

	def compute_id(row):
		return const_format_id

	def compute_tx_power(row):
		return const_format_tx_power

	def compute_token(row):
		return const_format_token

	def compute_interval(row):
		return const_format_interval

	new_column_funcs = {
		'FormatVersion': compute_format_version,
		'Source': compute_source,
		'Type': compute_type,
		'ID': compute_id,
		'TxPower': compute_tx_power,
		'Interval': compute_interval,
		'Token': compute_token,
	}

	with open(input_file, newline='', encoding='utf-8') as csv_in:
		reader = csv.DictReader(csv_in)
		rows = list(reader)

	# Validate columns (ignore new columns)
	missing = [col for col in output_columns if col not in reader.fieldnames and col not in new_column_funcs]
	if missing:
		raise ValueError(f"Missing columns in input or new column functions: {missing}")

	with open(output_file, 'w', newline='', encoding='utf-8') as csv_out:
		writer = csv.DictWriter(csv_out, fieldnames=output_columns)
		for row in rows:
			out_row = {}
			for col in output_columns:
				if col in row:
					out_row[col] = row[col]
				elif col in new_column_funcs:
					out_row[col] = new_column_funcs[col](row)
				else:
					out_row[col] = ''  # Default empty if not found
			writer.writerow(out_row)

if __name__ == '__main__':
	main()
