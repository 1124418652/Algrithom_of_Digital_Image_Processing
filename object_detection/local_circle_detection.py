import cv2
import numpy as np
from collections import deque

def find_circles(image, mask=None, threshold=300, nmax=100,
                 rmin=5, rmax=50, rstd=1, rpan=4):
    if rmin < rpan + 1: raise ValueError

    if mask is not None:
        rmin = max(rmin, int(np.ceil(np.min(mask) - rstd)))
        rmax = min(rmax, int(np.floor(np.max(mask) + rstd)))
    if rmin > rmax: return [], []

    # generate gradient
    Dx = cv2.Scharr(image, cv2.CV_32F, 1, 0)          # x 方向梯度
    Dy = cv2.Scharr(image, cv2.CV_32F, 0, 1)          # y 方向梯度
    Da = np.arctan2(Dy, Dx) * 2                     # 求梯度对应的theta角度
    Ds = np.log1p(np.hypot(Dy, Dx))                 # np.hypot 求解直角三角形对应的斜边
    Du = np.sum(np.cos(Da) * Ds, axis=-1)
    Dv = np.sum(np.sin(Da) * Ds, axis=-1)

    # calculate likelihood for each (x, y, r) pair
    #     based on: gradient changes across circle
    def iter_scores():
        queue = deque()
        for radius in range(rmin - rpan, rmax + rpan + 1):
            r = int(np.ceil(radius + 6 + rstd * 4))
            Ky, Kx = np.mgrid[-r:r+1,-r:r+1]
            Ka = np.arctan2(Ky, Kx) * 2
            Ks = np.exp(np.square(np.hypot(Ky, Kx) - radius) /
                        (-2 * rstd**2)) / np.sqrt(radius)
            Ku = np.cos(Ka) * Ks
            Kv = np.sin(Ka) * Ks
            queue.append(cv2.filter2D(Du, cv2.CV_32F, Ku) +
                         cv2.filter2D(Dv, cv2.CV_32F, Kv))
            if len(queue) > rpan * 2:
                yield (radius - rpan, queue[rpan] -
                       (np.fmax(0, queue[0]) + np.fmax(0, queue[rpan*2])))     # np.fmax()比较两个数组中的值，返回的矩阵中包含两个数组中较大的值
                queue.popleft()

    # choose best (x, y, r) for each (x, y)
    radiuses = np.zeros(image.shape[:2], dtype=int)
    scores = np.full(image.shape[:2], -np.inf)
    for radius, score in iter_scores():               # 调用 iter_scores()
        sel = (score > scores)
        if mask is not None:
            sel &= (mask > radius - rstd) & (mask < radius + rstd)
        scores[sel] = score[sel]
        radiuses[sel] = radius

    # choose top n circles
    circles = []
    weights = []
    for _ in range(nmax):
        y, x = np.unravel_index(np.argmax(scores), scores.shape)
        score = scores[y,x]
        if score < threshold: break
        r = radiuses[y,x]
        circles.append((x, y, r))
        weights.append(score)
        cv2.circle(scores, (x, y), r, 0, -1)

    return circles, weights

def choose_local_circle(circles, weights):
    pass

def main():
    image_path = 'object_detection/img/timg2.jpg'
    src_img = cv2.imread(image_path)
    # circles, weights = find_circles(src_img)

    gray_img = cv2.cvtColor(src_img, cv2.COLOR_BGR2GRAY)
    circles = cv2.HoughCircles(gray_img, cv2.HOUGH_GRADIENT, 2, 2, 0, 100, 130, 30, 60)

    print(circles.shape)

    for i in circles[0, :]:
        cv2.circle(src_img, (i[0], i[1]), i[2], (0, 255, 0), 1)

    cv2.imshow("gray_img", gray_img)
    cv2.imshow("src_img", src_img)
    cv2.waitKey(0)

if __name__ == '__main__':
    main()