import re
import sys

# takes a file with alternating lines of vertex coordinates and normal coordinates
# and returns the results as two seperate lists

def convert_alt_to_pbrt(filepath):
    positions = []
    normals = []

    try:
        with open(filepath, 'r') as f:
            # Read all non-empty lines, ignoring the "every sixth line" space
            lines = [line.strip() for line in f if line.strip()]

        # Iterate in steps of 2: Line i is Vertex, Line i+1 is Normal
        for i in range(0, len(lines) - 1, 2):
            v_parts = re.findall(r"[-+]?\d*\.\d+|\d+", lines[i].replace(',', ' '))
            n_parts = re.findall(r"[-+]?\d*\.\d+|\d+", lines[i+1].replace(',', ' '))

            if len(v_parts) >= 3 and len(n_parts) >= 3:
                positions.append(f"{v_parts[0]} {v_parts[1]} {v_parts[2]}")
                normals.append(f"{n_parts[0]} {n_parts[1]} {n_parts[2]}")

        # pbrt-v4 TriangleMesh construction
        indices = list(range(len(positions)))

        print('Shape "trianglemesh"')
        print(f'    "integer indices" [ {" ".join(map(str, indices))} ]')
        print(f'    "point3 P" [ {" ".join(positions)} ]')
        print(f'    "normal N" [ {" ".join(normals)} ]') # Corrected to "normal N"

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        convert_alt_to_pbrt(sys.argv[1])
    else:
        print("Usage: python script.py data.txt")
