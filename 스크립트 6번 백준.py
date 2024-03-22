"""
board = []
answer = 0
for n in range(8):
    board.append(list(input()))

for y in range(8):
    for x in range(8):
        if y %2 == 0 and x % 2 == 0 and board[y][x] == 'F':
            answer +=1
        elif y % 2 == 1 and x % 2 == 1 and board[y][x] == 'F' :
            answer +=1

print(answer)
"""


def room(H,W,N):
    answer = 0
    floor = str(N%H)
    room_num = str(N // H + 1)
    
    if len(room_num) <= 1:
        room_num = '0' + room_num
    
    return  str(floor) + str(room_num)

test_num = int(input())

for _ in range(test_num):
    test_case = list(input().split())
    answer = room(int(test_case[0]),int(test_case[1]),int(test_case[2]))
    print(answer)
