#숙제 4-1
#숙제 5-39
#숙제 15-3
#숙제 15-4
#숙제 15-19
#숙제 15-20


#숙제 4-24
# import random

# deck = [i for i in range(52)] #52장의 카드 숫자로 생성 0~51
# numbers = ['A',2,3,4,5,6,7,8,9,10, 'J','Q','K'] # 13개 숫자 리스트
# suits = ['스페이드', '다이아몬드', '하트', '클로버'] # 4개 무늬 리스트

# random.shuffle(deck) # 셔플링

# for i in deck:
#     suit = i // 13 # 0,1,2,3
#     number = i % 13 # 0,1,2,...,12
#     print(suits[suit],numbers[number]) 

#숙제 4-27
# x, y = eval(input("x,y 좌표를 입력: "))

# def testT(x,y):
#     if x < 0 or x > 200 or y < 0 or y > 100:
#         print('외부')
#     else:
#         slope = -1/2 # 직선의 방정식: y = slope * x + b
#         b = y - slope * x # y 절편
#         if 0 <= b <= 100:
#             print("내부") 
#         else:
#             print("외부")

# testT(x,y)



#숙제 5-42
# import random
# random.random()

# count = 0
# for _ in range(1000000):
#     x = random.random() * 2 -1 # [-1 : 1)
#     y = random.random() * 2 -1 # [-1 : 1)

#     if x < 0: # 1번 영역
#         count += 1
#     elif 0 <= x <= 1 and 0 <= y <=1: # 2,3번 영역
#         slope = -1
#         #y = slope * x + b
#         b = y - slope * x # y절편
#         if 0 <= b <= 1: # 3번 영역
#             count +=1

# print(count)
# print('확률: ', count/ 10000000)


#숙제 6-3


# def reverse(number):
#     result = 0
#     while number:
#         rem = number % 10
#         result = result * 10 + rem
#         number //=10
#     return result


# def isPalindrome(number):
#     if number == reverse(number):
#         return True
#     else:
#         return False

# print(isPalindrome(123454321))
# print(isPalindrome(12344322))

#숙제 6-4
###
#def reverse(number):
#    result = 0
#    while number:
#        rem = number % 10
#        result = result * 10 +rem
#        number //=10
#    return result
#
#num = eval(input('정수를 입력하세요.: '))
#
#print(reverse(num))


#숙제 6-5
#def displaySortedNumbers(num1, num2, num3):
#    result = sorted([num1,num2,num3])
#    print('정렬된 숫자는 ')
#    for n in result:
#        print(n, end=' ')
#    print('입니다.')
#    return
#
#nums =input('세 개의 수를 입력하세요: ').split(',')
#nList = [eval(n) for n in nums]
#
#displaySortedNumbers(nList[0], nList[1], nList[2])



#숙제 6-12
# def printChars(ch1, ch2, numberPerLine):
#     count = 0
#     for i in range(ord(ch1), ord(ch2)+1):
#         print(chr(i), end = ' ')
#         count += 1
#         if count % numberPerLine == 0:
#             print()

# printChars('1', 'Z',10)


#숙제6-13
# def m(i):
#     if i == 1:
#         return 1/2
#     return m(i-1) + (i/(i+1))

# for i in range(1,21):
#     print(i, '\t', m(i))



#숙제 15-18

cnt = 0
def main():
    n = eval(input("디스크의 개수를 입력하세요: "))
    #해결방법을 재귀적으로 찾는다.
    print("옮기는 순서는 다음과 같습니다: ")
    moveDisks(n, 'A', 'B', 'C')
    print(cnt)
    


def moveDisks(n, fromTower, toTower, auxTower):
    global cnt
    cnt += 1
    if n==1: #정지 조건
        print("디스크", n, "을/를", fromTower, "에서", toTower, "로옮긴다.")
    else:
        moveDisks(n-1, fromTower, auxTower, toTower)
        print("디스크", n, "을/를", fromTower, "에서", toTower, "로옮긴다.")
        moveDisks(n-1, auxTower, toTower, fromTower)
        

main()






               

