
vertex = [0]
vertex_normal = [0]
face = []
texture_coord = [0]

while True:
    try:
        inp = input()
    except: break;
    inp = inp.split()
    if not inp: continue

    if inp[0] == "v":
        #vertex.append([float(i) for i in inp[1:]])
        vertex.append(' '.join([i for i in inp[1:]]))
    elif inp[0] == "vn":
        vertex_normal.append([float(i) for i in inp[1:]])
    elif inp[0] == "vt":
        texture_coord.append([float(i) for i in inp[1:]])
    elif inp[0] == "f":
        for i in range(3,len(inp)):
            a = inp[1].split("/")[0]
            b = inp[i-1].split("/")[0]
            c = inp[i].split("/")[0]
            a,b,c = int(a),int(b),int(c)
            face.append([a,b,c])

print("150 100 0 0 0 0 600 1.7777 45")
print("pointlight 0 0 0\n\n")

print("3\nmetal 0.855 solidcolor 0.35 0.35 0.35")
print("pointlight 10 10 10")
print("lambertian solidcolor 0.15 0.15 0.15")

print(len(face)+2)
print("parallelogram -50 100 -50 100 0 0 0 0 100")
print("plane 0 0 0 1 0 0 0 0 1")
for (i,j,k) in face:
    print("triangle_point "+' '.join([vertex[i], vertex[j], vertex[k]]))

print(len(face)+2)
print("0 1")
print("1 2")
for i in range(2,len(face)+2):
    print(str(i)+" 0")
