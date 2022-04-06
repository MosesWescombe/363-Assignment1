file = open("points.csv", "r")
lines = file.readlines()

result = "float points[" + str(len(lines) * 2) + "] = { "

for i, line in enumerate(lines):
    if i > 0:
        result += ", "
    result += line.strip()

result += " };"

print(result);