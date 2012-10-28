filename=raw_input('Input the file name:')

mult = [3,3,3]

f=open(filename)

temp=f.readline()
temp=temp.strip()
nPart = int(temp)

temp=f.readline()
temp=temp.strip()
nTypes = int(temp)

temp=f.readline()
temp=temp.strip()
temp=temp.split()
box = []
for i in range(len(temp)):
    box.append(float(temp[i]))

positions = []
ids = []
rotations = []
for i in range(nPart):
    temp = f.readline()
    temp = temp.strip()
    temp = temp.split()
    ids.append(int(temp[0]))
    position = [float(x) for x in temp[1:4]]
    positions.append(position)
    rotations.append(temp[4:])

rest = f.read()
f.close()

fout=open(filename[:-4]+'_mult.dat','w')
n = nPart * mult[0] * mult[1] * mult[2]
fout.write(str(n)+"\n")
fout.write(str(nTypes)+"\n")

for i in range(3):
    for j in range(3):
        newb = box[3*i + j] * mult[j]
        fout.write(str(newb) + "\t")
fout.write("\n")

for i in range(mult[0]):
    for j in range(mult[1]):
        for k in range(mult[2]):
            for m in range(nPart):
                fout.write(str(ids[m])+"\t")
                newPos = 3*[0]
                for n in range(3):
                    newPos[n] = positions[m][n] + i*box[3*n + 0] + j*box[3*n + 1] + k*box[3*n + 2]
                    fout.write(str(newPos[n]) + "\t")
                for rot in rotations[m]:
                    fout.write(str(rot)+"\t")
                fout.write("\n")

fout.write(rest)
    

fout.close()
