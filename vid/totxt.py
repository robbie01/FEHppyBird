import numpy as np
import os
import imageio as iio
import sys

folder = sys.argv[1]
total = 0
i = 0
for filename in os.listdir(f"{folder}/frames"):
    total += 1

print(total)
    
for i in range(total-1):
    print(f"converting {i}.jpg")
    x = np.uint32(iio.imread(f"{folder}/frames/{i}.jpg", pilmode="RGB"))
    y = (x[..., 0] << 16) | (x[..., 1] << 8) | x[..., 2]
    with open(f"{folder}/frames/{i}.txt", "w") as f:
        f.write('\n'.join(str(z) for z in np.ravel(y)))
    i+=1

