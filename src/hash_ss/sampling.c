#include <iostream>
#include <cstdlib>
#include <algorithm>

typedef unsigned char color;
typedef struct Color Color;
#define SUB_PIXEL_SIZE 4

typedef struct Color {
    float percent;
    color r, g, b;
    Color() : percent(0), r(0), g(0), b(0) {}
    Color(float percent, color r, color g, color b) : percent(percent), r(r), g(g), b(b) {}

    Color operator+(const Color & ref) const {
        return Color(percent + ref.percent, r + ref.r, g + ref.g, b + ref.b);
    }

    Color & operator+=(const Color & ref) {
        percent += ref.percent;
        r += ref.r;
        g += ref.g;
        b += ref.b;
        return *this;
    }

    Color operator/(int i) const {
        return Color(percent/i, r/i, g/i, b/i);
    }

    Color & operator/=(int i) {
        percent /= i;
        r /= i;
        g /= i;
        b /= i;
        return *this;
    }
} Color;

void process(color* data, unsigned int width, unsigned int height) {
    color* tempdata = (color*) std::malloc(sizeof(color) * width * height * 3);
    memcpy(tempdata, data, sizeof(color) * width * height * 3);

    for (unsigned int index = 0; index < width*height; ++index) {
        int count = 0;
        Color c(0, 0, 0, 0), tmp;
        int r = 0, g = 0, b = 0;

        for (int i = -SUB_PIXEL_SIZE; i <= SUB_PIXEL_SIZE; i += SUB_PIXEL_SIZE) {
            for (int j = -SUB_PIXEL_SIZE; j <= SUB_PIXEL_SIZE; j += SUB_PIXEL_SIZE) {
                if (getColor(tempdata, index+width*j+i, width*height, tmp)) {
                    ++count;
                    r += tmp.r;
                    g += tmp.g;
                    b += tmp.b;
                }
            }
        }

        data[index*3 + 0] = r / count;
        data[index*3 + 1] = g / count;
        data[index*3 + 2] = b / count;
    }

    free(tempdata);
}

bool getColor(color* data, int index, unsigned int size, Color& c) {
if (index < 0 || (unsigned int) index >= size) return false;

c.r = data[index*3 + 0];
c.g = data[index*3 + 1];
c.b = data[index*3 + 2];
return true;
}

extern "C" int superSample(color* data,
                           unsigned int s_width, unsigned int s_height,
                           unsigned int t_width, unsigned int t_height);

/* Block-Thread structure for super sampling
 * o - o ... o    Keys
 * | X |  X  :    ----
 * o - o ... o    o = Data point
 * : X :  X  :
 * o - o ... o
 *
 * We group N*N data points into blocks of threads.
 * The amount of blocks is relative to the ratio of sampling
 * from the source resolution to the target resolution.
 *
 * S_r = source res (width*height)
 * T_r = target resolution (width * height)
 * T_c = Block thread count
 * B_c = Block count
 * M_t = Max block thread count
 * M_b = Max blocks count
 *
 * B_c = Math.min(T_r, M_b)
 * T_c = Math.min(Math.floor(S_r / T_r), M_t)
 */

#define BLOCKS (2048 * 2048)
#define THREADS_PER_BLOCK 512

#define MAX_BLOCKS 65535
#define MAX_THREADS 1024

unsigned int calcBlockCount(unsigned int sourceRes) {
    return std::min(sourceRes, MAX_BLOCKS);
}

unsigned int calcThreadCount(unsigned int sourceRes, unsigned int targetRes) {
    return std::min(sourceRes / targetRes, MAX_THREADS);
}

int superSample(color* data,
                unsigned int s_width, unsigned int s_height,
                unsigned int t_width, unsigned int t_height) {

    unsigned int blockCount = calcBlockCount(s_width * s_height);
    unsigned int threadCount = calcThreadCount(s_width * s_height, t_width * t_height);

    int a[blockCount], b[blockCount], c[blockCount], i;
    int *dev_a, *dev_b, *dev_c;

    cudaMalloc((void**) &dev_c, blockCount * sizeof(int));
    cudaMalloc((void**) &dev_b, blockCount * sizeof(int));
    cudaMalloc((void**) &dev_a, blockCount * sizeof(int));

    cudaMemcpy(dev_a, a, blockCount * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, blockCount * sizeof(int), cudaMemcpyHostToDevice);

    kernel<<<blockCount, threadCount>>>(dev_a, dev_b, dev_c,);
    cudaMemcpy(c, dev_c, blockCount * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(dev_c);
    cudaFree(dev_b);
    cudaFree(dev_a);
    return 0;
}