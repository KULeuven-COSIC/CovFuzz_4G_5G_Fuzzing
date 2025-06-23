hex_str = ''
while True:
    hex_str = input()
    if hex_str == "Q" or hex_str == "q":
        break
    hex_str_arr = hex_str.strip().split(' ')
    for el in hex_str_arr:
        print(int(el,16), end=",")
    print('')