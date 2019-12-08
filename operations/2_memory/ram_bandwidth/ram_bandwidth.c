#include "../../../utils/utils.h"

#define NUM_ITERATIONS 100
#define N_BYTES (1<<22) // 4MB
#define N_INTS (N_BYTES >> 2)
#define CACHE_SIZE (1536*1024) // 1.5MB

char foo[CACHE_SIZE], bar[CACHE_SIZE];
#define OBLIT_CACHE memcpy(foo, bar, CACHE_SIZE)

int gsum = 0;

uint64_t benchmark_read(int* beg, int* end) {
    struct Timer timer;
    int sum = 0;
    uint64_t t = 0;

    for (int i=0; i < NUM_ITERATIONS; ++i) {
        int *p = beg;
        OBLIT_CACHE;

        tic(timer);
        while (p < end) {
#define OP(i) sum += p[i];
  OP(0)  OP(8)  OP(16)  OP(24)  OP(32)  OP(40)  OP(48)  OP(56)  OP(64)  OP(72)  OP(80)  OP(88)  OP(96)  OP(104)  OP(112)  OP(120)  OP(128)  OP(136)  OP(144)  OP(152)  OP(160)  OP(168)  OP(176)  OP(184)  OP(192)  OP(200)  OP(208)  OP(216)  OP(224)  OP(232)  OP(240)  OP(248)  OP(256)  OP(264)  OP(272)  OP(280)  OP(288)  OP(296)  OP(304)  OP(312)  OP(320)  OP(328)  OP(336)  OP(344)  OP(352)  OP(360)  OP(368)  OP(376)  OP(384)  OP(392)  OP(400)  OP(408)  OP(416)  OP(424)  OP(432)  OP(440)  OP(448)  OP(456)  OP(464)  OP(472)  OP(480)  OP(488)  OP(496)  OP(504)  OP(512)  OP(520)  OP(528)  OP(536)  OP(544)  OP(552)  OP(560)  OP(568)  OP(576)  OP(584)  OP(592)  OP(600)  OP(608)  OP(616)  OP(624)  OP(632)  OP(640)  OP(648)  OP(656)  OP(664)  OP(672)  OP(680)  OP(688)  OP(696)  OP(704)  OP(712)  OP(720)  OP(728)  OP(736)  OP(744)  OP(752)  OP(760)  OP(768)  OP(776)  OP(784)  OP(792)  OP(800)  OP(808)  OP(816)  OP(824)  OP(832)  OP(840)  OP(848)  OP(856)  OP(864)  OP(872)  OP(880)  OP(888)  OP(896)  OP(904)  OP(912)  OP(920)  OP(928)  OP(936)  OP(944)  OP(952)  OP(960)  OP(968)  OP(976)  OP(984)  OP(992)  OP(1000)  OP(1008)  OP(1016)
p += 1024;
        }
        toc(timer);
        t += timer_diff(timer);
    }
    gsum += sum;
    return t / NUM_ITERATIONS - READING_TIME_OVERHEAD;
}
#undef OP

uint64_t benchmark_write(int* beg, int* end) {
    struct Timer timer;
    uint64_t t = 0;

    for (int i=0; i < NUM_ITERATIONS; ++i) {
        int *p = beg;
        OBLIT_CACHE;

        tic(timer);
        while (p < end) {
#define OP(i) p[i] = 1;
  OP(0)  OP(8)  OP(16)  OP(24)  OP(32)  OP(40)  OP(48)  OP(56)  OP(64)  OP(72)  OP(80)  OP(88)  OP(96)  OP(104)  OP(112)  OP(120)  OP(128)  OP(136)  OP(144)  OP(152)  OP(160)  OP(168)  OP(176)  OP(184)  OP(192)  OP(200)  OP(208)  OP(216)  OP(224)  OP(232)  OP(240)  OP(248)  OP(256)  OP(264)  OP(272)  OP(280)  OP(288)  OP(296)  OP(304)  OP(312)  OP(320)  OP(328)  OP(336)  OP(344)  OP(352)  OP(360)  OP(368)  OP(376)  OP(384)  OP(392)  OP(400)  OP(408)  OP(416)  OP(424)  OP(432)  OP(440)  OP(448)  OP(456)  OP(464)  OP(472)  OP(480)  OP(488)  OP(496)  OP(504)  OP(512)  OP(520)  OP(528)  OP(536)  OP(544)  OP(552)  OP(560)  OP(568)  OP(576)  OP(584)  OP(592)  OP(600)  OP(608)  OP(616)  OP(624)  OP(632)  OP(640)  OP(648)  OP(656)  OP(664)  OP(672)  OP(680)  OP(688)  OP(696)  OP(704)  OP(712)  OP(720)  OP(728)  OP(736)  OP(744)  OP(752)  OP(760)  OP(768)  OP(776)  OP(784)  OP(792)  OP(800)  OP(808)  OP(816)  OP(824)  OP(832)  OP(840)  OP(848)  OP(856)  OP(864)  OP(872)  OP(880)  OP(888)  OP(896)  OP(904)  OP(912)  OP(920)  OP(928)  OP(936)  OP(944)  OP(952)  OP(960)  OP(968)  OP(976)  OP(984)  OP(992)  OP(1000)  OP(1008)  OP(1016)
p += 1024;
        }
        toc(timer);
        t += timer_diff(timer);
    }
    return t / NUM_ITERATIONS - READING_TIME_OVERHEAD;
}
#undef OP

int main() {
    uint64_t tw, tr;
    int arr[N_INTS];

    tr = benchmark_read(arr, arr + N_INTS);
    tw = benchmark_write(arr, arr + N_INTS);

#ifdef _DEBUG
    printf("%llu, %llu\n", tr, tw);
#endif

// bandwidth: GB/s = GB * cycles/s * 1/cycles
#define t2b(t) (N_BYTES*1.0 / (1<<30) * 2.7e9 / (t))

    printf("Read bandwidth: %.5f (GB/s); Write bandwidth: %.5f (GB/s)\n", t2b(tr), t2b(tw));

#undef t2b
}
