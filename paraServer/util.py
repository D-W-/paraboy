import random
import math
import numpy as np

MinNumber = 6
Equipment = 54321

'''
随机生成一个装备值
'''


def generateEquipment():
    global Equipment
    Equipment = random.randint(1, 9999999)
    print("Equipment:", Equipment)


'''
根据人数,生成每个人的tuple(x,y)
'''


def generateTuples(N):
    factor = []
    factor.append(Equipment)

    for i in range(1, MinNumber):
        factor.append(random.randint(1, 1000))
        # print(i, "  ", factor)

        # print("everyone:")

    vis = {}
    tuples = []
    for i in range(N):
        x = -1
        while x == -1 or x in vis:
            x = random.randint(1, 20)
        vis[x] = 1
        y = 0
        for j in range(MinNumber):
            y += factor[j] * math.pow(x, j)

        # print(x, "  ",y)
        tuples.append((x, int(y)))

    return tuples


'''
输入:
[(x1,y1),(x2,y2)]
得到装备号,失败返回失败信息
'''


def getEquipment(tuples):
    k = len(tuples)
    print("当前人数:", k)
    if k < MinNumber:
        return {"message": "人数不足" + str(MinNumber) + "人", "equipment": -1}

    res = 0
    for i in range(k):
        temp = 1
        for j in range(k):
            if i != j:
                temp *= tuples[j][0] / (tuples[j][0] - tuples[i][0])
        res += temp * tuples[i][1]

    return {"message": "成功获取装备", "equipment": round(res)}

if __name__ == '__main__':
    generateEquipment()
    tuples = generateTuples(10)
    # print(tuples)
    tuples2 = tuples[0:6]
    # print(tuples2)
    print(getEquipment(tuples2))
