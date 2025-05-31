"""
Converts a CSV (Comma Separated Values) file containing CAN (Controller Area Network)
bus messages into a BLF (Binary Logging Format) file.

The script reads CAN message data from a specified CSV file, expecting a specific
column format, and writes each message to a new BLF file. It handles common
data conversion errors by skipping invalid rows and printing error messages to
stderr.

Usage:
  python csv_to_blf.py <input_csv_file> <output_blf_file>

Arguments:
  input_csv_file   : Path to the input CSV file.
  output_blf_file  : Path where the output BLF file will be saved.

Example:
  python csv_to_blf.py can_log.csv can_log.blf

Expected CSV Format:
The script assumes the CSV file has a header row, which will be skipped.
Subsequent rows should contain CAN message data with the following columns,
in order:

1.  Timestamp (float): The timestamp of the message in seconds (e.g., 1678886400.123).
2.  ArbitrationID (hex string): The message arbitration ID in hexadecimal format
    (e.g., "10A", "7FF", "1A2B3C4D").
3.  IsExtendedID (boolean string): Specifies if the arbitration ID is an extended
    format (29-bit). Should be "True" or "False" (case-insensitive).
4.  IsRemoteFrame (boolean string): Specifies if the message is a remote frame (RTR).
    Should be "True" or "False" (case-insensitive).
5.  DLC (string/integer): Data Length Code. While this column is expected, its value
    is currently NOT used by the script as `python-can` infers DLC from the
    length of the data field. It can be an empty string or a number.
6.  Data (hex string): The message payload data as a hexadecimal string
    (e.g., "0102AABB", "DEADBEEF"). For remote frames with no data, this
    should be an empty string.

Rows that do not conform to this format or contain invalid data will be skipped.
The script requires the `python-can` library to be installed.
"""
import csv
import sys
import argparse
import can # Import the python-can library

def main():
    """
    Main function to convert CSV to BLF.
    """
    parser = argparse.ArgumentParser(
        description="Convert a CSV file containing CAN messages to a BLF file.",
        epilog="""Expected CSV columns: Timestamp (float), ArbitrationID (hex), IsExtendedID (bool), IsRemoteFrame (bool), DLC (int/str), Data (hex string)"""
    )
    parser.add_argument("input_csv", help="Path to the input CSV file.")
    parser.add_argument("output_blf", help="Path to the output BLF file.")
    args = parser.parse_args()

    print(f"Input CSV file: {args.input_csv}")
    print(f"Output BLF file: {args.output_blf}")

    try:
        with open(args.input_csv, 'r', newline='') as infile, \
             can.BLFWriter(args.output_blf) as blf_writer:

            csv_reader = csv.reader(infile)
            try:
                header = next(csv_reader)  # Assuming the first row is the header
                print(f"CSV Header: {header}")
            except StopIteration:
                print("Error: CSV file is empty or contains only a header.", file=sys.stderr)
                sys.exit(1)

            # Define expected CSV columns based on common CAN log formats
            # Example: Timestamp,ArbitrationID,IsExtendedID,IsRemoteFrame,DLC,Data
            # Adjust indices if your CSV format is different.
            # Timestamp (float), Arbitration ID (hex str), Is Extended ID (bool str),
            # Is Remote Frame (bool str), DLC (int str), Data (hex str)

            print("Starting CSV to BLF conversion...")
            messages_converted = 0
            for i, row in enumerate(csv_reader, start=1): # Start row count from 1 for user messages
                try:
                    # Strip whitespace from all cells in the row first
                    row = [cell.strip() for cell in row]

                    timestamp_str = row[0]
                    arbitration_id_str = row[1]
                    is_extended_id_str = row[2]
                    is_remote_frame_str = row[3]
                    # dlc_str = row[4] # DLC is often derived from data length or explicit
                    data_hex_str = row[5]

                    timestamp = float(timestamp_str)
                    arbitration_id = int(arbitration_id_str, 16) # Assuming hex ID
                    is_extended_id = is_extended_id_str.lower() == 'true'
                    is_remote_frame = is_remote_frame_str.lower() == 'true'

                    # Convert hex string data to bytes
                    # Handle empty data string for remote frames
                    data = bytes.fromhex(data_hex_str) if data_hex_str else b''

                    # DLC can be explicitly set or derived from data length
                    # For simplicity, python-can's Message class infers DLC from data length
                    # if dlc is not provided or is None.

                    msg = can.Message(
                        timestamp=timestamp,
                        arbitration_id=arbitration_id,
                        is_extended_id=is_extended_id,
                        is_remote_frame=is_remote_frame,
                        # dlc=int(dlc_str) # Or let it be inferred
                        data=data
                    )
                    blf_writer.on_message_received(msg)
                    messages_converted += 1
                except ValueError as ve:
                    print(f"Skipping row {i+1} (CSV line {i+1+1}) due to data conversion error: {ve}. Row: {row}", file=sys.stderr)
                except IndexError as ie:
                    print(f"Skipping row {i+1} (CSV line {i+1+1}) due to missing columns: {ie}. Row: {row}", file=sys.stderr)
                except Exception as e:
                    print(f"Skipping row {i+1} (CSV line {i+1+1}) due to unexpected error: {e}. Row: {row}", file=sys.stderr)

            if messages_converted == 0 and i > 0 :
                 print("Warning: No messages were successfully converted. Check CSV format and content.", file=sys.stderr)
            else:
                print(f"Successfully converted {messages_converted} messages from CSV to BLF.")

            print(f"BLF file generated: {args.output_blf}")

    except FileNotFoundError:
        print(f"Error: Input CSV file not found: {args.input_csv}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
