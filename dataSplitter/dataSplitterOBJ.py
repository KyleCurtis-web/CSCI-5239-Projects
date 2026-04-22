import bpy

# Set your OBJ filepath
file_path = r"C:\Users\metalmonstr\Downloads\dataSplitter\Meeple.obj"
bpy.ops.wm.obj_import(filepath=file_path)

obj = bpy.context.active_object
mesh = obj.data

p_list, n_list, uv_list, indices = [], [], [], []
unique_verts = {}

if mesh.uv_layers:
    uv_layer = mesh.uv_layers.active.data
    for poly in mesh.polygons:
        for loop_idx in poly.loop_indices:
            v_idx = mesh.loops[loop_idx].vertex_index
            pos = mesh.vertices[v_idx].co
            norm = mesh.corner_normals[loop_idx].vector
            uv = uv_layer[loop_idx].uv

            key = (tuple(round(v, 6) for v in pos),
                   tuple(round(v, 6) for v in norm),
                   tuple(round(v, 6) for v in uv))

            if key not in unique_verts:
                unique_verts[key] = len(p_list) // 3
                p_list.extend([pos.x, pos.y, pos.z])
                n_list.extend([norm.x, norm.y, norm.z])
                uv_list.extend([uv.x, uv.y])

            indices.append(unique_verts[key])

    # Build the string
    p_str = " ".join(f"{x:.6f}" for x in p_list)
    n_str = " ".join(f"{x:.6f}" for x in n_list)
    uv_str = " ".join(f"{x:.6f}" for x in uv_list)
    idx_str = " ".join(map(str, indices))

    print('\nShape "trianglemesh"')
    print(f'    "integer indices" [ {idx_str} ]')
    print(f'    "point3 f p" [ {p_str} ]')
    print(f'    "normal3 f n" [ {n_str} ]')
    print(f'    "point2 f uv" [ {uv_str} ]')
