import matplotlib.pyplot as plt
import numpy as np
import math 

## charge_per = 123 - 123./(1+(v./3.7).^80).^0.165
def fact(x):
    accum = 1
    while x>0:
        accum = x*accum
        x -= 1
    return accum
base = 0x04000000
testing_it = []
def exp(val):
    N = (10 + (abs(val) >> 26)*5)
    mod = val
    accum = 1*base + mod
    for x in range(2, N):
        mod = int(mod*(val/x)) >> 26
        accum += mod
        testing_it.append(accum/base)
        print(accum/base)
    return accum


cell_num = 1
volt = np.linspace(3.5*cell_num, 4.2*cell_num, 1000)
#part 1
p1 = (lambda v:(v/(3.7*cell_num))**80)
#part 2
p2 = (lambda v:(1 + p1(v))**.165)
#part 3
p3 = (lambda v: 123 - 123/p2(v))


percent1 = [p1(v) for v in volt]
percent2 = [p2(v) for v in volt]
percent3 = [p3(v) for v in volt]
plt.plot(volt, percent1)
plt.plot(volt, percent2)
plt.plot(volt, percent3)

plt.show(block=False)
breakpoint()

voltage = 3.6
x = int((.9)*base)
b = int(1*base)>>1 
y = int(0)

while (x < 1*base): # normalizing to 1 <= x
    x <<= 1
    y -= int(1*base)

while (x >= 2*base): # normalizing to x < 2
    x >>= 1
    y += int(1*base)

z = x

for x in range(0,26): 
    z = int(z * z / base)
    if (z >= 2*base):
        z >>= 1
        y += b
    b >>= 1

y = int(((y*0x58b90bfc)/0x80000000))
print(y/base)
breakpoint()
y = y*80
breakpoint()
exp(y)
breakpoint()