import re
import sys

# splits a .txt file that has vertex, normal, and uv coordinates listed as [XYZ NXNYNZ UV]
# and returns the results as three seperate lists

def convert_to_pbrt(filepath):
    try:
        with open(filepath, 'r') as f:
            content = f.read()

        # Step 1: Replace all commas with spaces and find every number (int or float)
        # This handles mixed "1, 2, 3" and "4 5 6" formats seamlessly.
        numbers = re.findall(r"[-+]?\d*\.\d+|\d+", content.replace(',', ' '))

        # Step 2: Separate data into P, N, and uv lists
        positions = []
        normals = []
        uvs = []

        # Data stride is 8: [x, y, z, nx, ny, nz, u, v]
        for i in range(0, len(numbers) - 7, 8):
            positions.append(f"{numbers[i]} {numbers[i+1]} {numbers[i+2]}")
            normals.append(f"{numbers[i+3]} {numbers[i+4]} {numbers[i+5]}")
            uvs.append(f"{numbers[i+6]} {numbers[i+7]}")

        # Step 3: Create sequential indices (0, 1, 2, 3...)
        indices = list(range(len(positions)))

        # Step 4: Output to PBRT v4 format
        print('Shape "trianglemesh"')
        print(f'    "integer indices" [ {" ".join(map(str, indices))} ]')
        print(f'    "point3 P" [ {" ".join(positions)} ]')
        print(f'    "normal N" [ {" ".join(normals)} ]')
        print(f'    "point2 uv" [ {" ".join(uvs)} ]')

    except Exception as e:
        print(f"Error reading file: {e}")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        convert_to_pbrt(sys.argv[1])
    else:
        print("Usage: python script.py your_data_file.txt")
