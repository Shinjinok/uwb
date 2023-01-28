/****************************************************************************
 *
 *   Copyright (c) 2020-2022 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include <stdint.h>

static constexpr float SAMPLING_RES = 10;
static constexpr float SAMPLING_MIN_LAT = -90;
static constexpr float SAMPLING_MAX_LAT = 90;
static constexpr float SAMPLING_MIN_LON = -180;
static constexpr float SAMPLING_MAX_LON = 180;

static constexpr int LAT_DIM = 19;
static constexpr int LON_DIM = 37;


// *INDENT-OFF*
// Magnetic declination data in radians * 10^-4
// Model: WMM-2020,
// Version: 0.5.1.11,
// Date: 2023.074,
static constexpr const int16_t declination_table[19][37] {
	//    LONGITUDE:   -180,  -170,  -160,  -150,  -140,  -130,  -120,  -110,  -100,   -90,   -80,   -70,   -60,   -50,   -40,   -30,   -20,   -10,     0,    10,    20,    30,    40,    50,    60,    70,    80,    90,   100,   110,   120,   130,   140,   150,   160,   170,   180,
	/* LAT: -90 */ {  25974, 24228, 22483, 20738, 18992, 17247, 15502, 13756, 12011, 10266,  8520,  6775,  5030,  3285,  1539,  -206, -1951, -3697, -5442, -7187, -8932,-10678,-12423,-14168,-15914,-17659,-19404,-21150,-22895,-24641,-26386,-28131,-29877, 31210, 29464, 27719, 25974, },
	/* LAT: -80 */ {  22539, 20409, 18470, 16697, 15057, 13519, 12055, 10644,  9269,  7919,  6588,  5269,  3960,  2655,  1348,    28, -1314, -2686, -4097, -5550, -7045, -8583,-10161,-11781,-13446,-15165,-16951,-18824,-20807,-22927,-25201,-27636,-30204, 29989, 27372, 24868, 22539, },
	/* LAT: -70 */ {  14980, 13581, 12454, 11491, 10621,  9788,  8946,  8058,  7105,  6086,  5016,  3923,  2838,  1784,   762,  -247, -1285, -2392, -3595, -4897, -6279, -7711, -9160,-10603,-12030,-13447,-14876,-16363,-17989,-19907,-22434,-26242, 30671, 24126, 19621, 16851, 14980, },
	/* LAT: -60 */ {   8431,  8183,  7900,  7624,  7369,  7114,  6804,  6370,  5754,  4934,  3932,  2821,  1702,   676,  -203,  -958, -1687, -2511, -3518, -4720, -6054, -7427, -8749, -9960,-11024,-11923,-12637,-13132,-13304,-12855,-10750, -3465,  4953,  7682,  8446,  8568,  8431, },
	/* LAT: -50 */ {   5493,  5528,  5471,  5380,  5305,  5267,  5231,  5103,  4758,  4093,  3081,  1808,   468,  -708, -1573, -2132, -2525, -2967, -3658, -4680, -5931, -7213, -8355, -9255, -9851,-10086, -9887, -9126, -7614, -5249, -2342,   406,  2516,  3941,  4815,  5289,  5493, },
	/* LAT: -40 */ {   3960,  4053,  4059,  4013,  3952,  3917,  3921,  3909,  3735,  3198,  2172,   728,  -835, -2144, -3000, -3444, -3621, -3678, -3852, -4435, -5425, -6507, -7392, -7926, -8027, -7642, -6746, -5359, -3650, -1951,  -494,   727,  1771,  2639,  3297,  3728,  3960, },
	/* LAT: -30 */ {   2988,  3074,  3103,  3087,  3027,  2948,  2887,  2852,  2725,  2245,  1205,  -318, -1925, -3172, -3904, -4243, -4315, -4097, -3662, -3454, -3836, -4592, -5288, -5631, -5498, -4904, -3940, -2735, -1532,  -593,    81,   681,  1306,  1912,  2424,  2788,  2988, },
	/* LAT: -20 */ {   2346,  2391,  2407,  2407,  2363,  2267,  2158,  2082,  1936,  1438,   377, -1116, -2593, -3648, -4172, -4280, -4070, -3504, -2640, -1851, -1591, -1957, -2607, -3071, -3101, -2736, -2098, -1284,  -514,   -30,   229,   529,   972,  1454,  1880,  2191,  2346, },
	/* LAT: -10 */ {   1952,  1946,  1923,  1917,  1888,  1801,  1689,  1597,  1412,   860,  -206, -1586, -2856, -3678, -3935, -3699, -3119, -2335, -1489,  -739,  -282,  -324,  -798, -1303, -1511, -1418, -1103,  -605,  -113,   123,   168,   326,   699,  1142,  1540,  1831,  1952, },
	/* LAT:   0 */ {   1739,  1705,  1647,  1636,  1624,  1553,  1445,  1330,  1075,   454,  -589, -1816, -2866, -3445, -3435, -2928, -2158, -1377,  -727,  -193,   217,   323,    39,  -378,  -630,  -684,  -586,  -326,   -39,    46,   -19,    67,   411,   859,  1283,  1607,  1739, },
	/* LAT:  10 */ {   1601,  1609,  1565,  1580,  1604,  1553,  1429,  1240,   864,   146,  -875, -1949, -2771, -3104, -2893, -2282, -1505,  -796,  -283,    96,   421,   566,   401,    78,  -155,  -262,  -288,  -209,  -108,  -153,  -296,  -271,    33,   493,   978,  1389,  1601, },
	/* LAT:  20 */ {   1414,  1563,  1623,  1714,  1799,  1777,  1620,  1316,   772,   -84, -1125, -2073, -2665, -2769, -2437, -1833, -1122,  -474,   -16,   292,   550,   694,   601,   357,   159,    40,   -55,  -123,  -206,  -398,  -638,  -697,  -461,   -10,   535,  1056,  1414, },
	/* LAT:  30 */ {   1108,  1475,  1735,  1959,  2118,  2128,  1939,  1516,   789,  -238, -1352, -2225, -2635, -2570, -2169, -1589,  -933,  -316,   144,   449,   679,   824,   802,   652,   504,   378,   217,     1,  -285,  -662, -1032, -1191, -1030,  -601,   -23,   587,  1108, },
	/* LAT:  40 */ {    746,  1331,  1826,  2219,  2469,  2513,  2298,  1765,   855,  -368, -1602, -2463, -2780, -2626, -2180, -1587,  -934,  -305,   204,   568,   837,  1034,  1125,  1114,  1038,   888,   615,   198,  -346,  -954, -1475, -1718, -1599, -1181,  -583,    88,   746, },
	/* LAT:  50 */ {    449,  1195,  1877,  2436,  2805,  2913,  2686,  2034,   897,  -593, -2008, -2918, -3211, -3019, -2529, -1883, -1173,  -476,   139,   644,  1062,  1415,  1693,  1862,  1880,  1689,  1238,   523,  -377, -1280, -1957, -2240, -2113, -1673, -1040,  -312,   449, },
	/* LAT:  60 */ {    244,  1095,  1901,  2599,  3107,  3324,  3116,  2312,   809, -1137, -2844, -3822, -4075, -3817, -3246, -2502, -1676,  -837,   -30,   721,  1412,  2042,  2583,  2976,  3133,  2940,  2293,  1167,  -254, -1573, -2437, -2742, -2576, -2084, -1396,  -601,   244, },
	/* LAT:  70 */ {     -8,   925,  1822,  2622,  3241,  3541,  3301,  2190,    -8, -2686, -4631, -5465, -5483, -5007, -4243, -3312, -2290, -1226,  -152,   907,  1931,  2896,  3763,  4468,  4912,  4938,  4317,  2828,   621, -1489, -2783, -3223, -3065, -2537, -1794,  -930,    -8, },
	/* LAT:  80 */ {   -771,   149,  1000,  1691,  2080,  1915,   776, -1705, -4800, -6913, -7700, -7604, -7000, -6100, -5021, -3830, -2570, -1269,    54,  1379,  2692,  3974,  5202,  6341,  7339,  8100,  8444,  7991,  6025,  2161, -1488, -3200, -3553, -3219, -2541, -1692,  -771, },
	/* LAT:  90 */ { -29638,-27893,-26147,-24402,-22657,-20911,-19166,-17421,-15675,-13930,-12185,-10440, -8695, -6949, -5204, -3459, -1714,    31,  1776,  3521,  5267,  7012,  8757, 10503, 12248, 13993, 15739, 17484, 19229, 20975, 22720, 24466, 26211, 27957, 29702,-31384,-29638, },
};

// Magnetic inclination data in radians * 10^-4
// Model: WMM-2020,
// Version: 0.5.1.11,
// Date: 2023.074,
static constexpr const int16_t inclination_table[19][37] {
	//    LONGITUDE:   -180,  -170,  -160,  -150,  -140,  -130,  -120,  -110,  -100,   -90,   -80,   -70,   -60,   -50,   -40,   -30,   -20,   -10,     0,    10,    20,    30,    40,    50,    60,    70,    80,    90,   100,   110,   120,   130,   140,   150,   160,   170,   180,
	/* LAT: -90 */ { -12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568,-12568, },
	/* LAT: -80 */ { -13652,-13518,-13358,-13177,-12983,-12782,-12578,-12378,-12187,-12011,-11855,-11721,-11610,-11524,-11461,-11419,-11399,-11402,-11429,-11483,-11567,-11681,-11826,-12000,-12198,-12415,-12645,-12878,-13106,-13318,-13504,-13654,-13758,-13809,-13805,-13751,-13652, },
	/* LAT: -70 */ { -14100,-13781,-13462,-13139,-12807,-12464,-12109,-11752,-11409,-11101,-10849,-10666,-10554,-10502,-10488,-10491,-10497,-10505,-10528,-10583,-10692,-10867,-11114,-11429,-11803,-12221,-12669,-13133,-13599,-14052,-14469,-14813,-15000,-14945,-14714,-14416,-14100, },
	/* LAT: -60 */ { -13515,-13161,-12823,-12490,-12147,-11774,-11360,-10905,-10438,-10009, -9680, -9506, -9504, -9641, -9845,-10036,-10159,-10199,-10185,-10169,-10216,-10376,-10668,-11078,-11578,-12135,-12722,-13318,-13906,-14468,-14965,-15254,-15075,-14689,-14282,-13889,-13515, },
	/* LAT: -50 */ { -12494,-12152,-11820,-11498,-11175,-10828,-10429, -9958, -9429, -8909, -8521, -8399, -8603, -9065, -9628,-10137,-10492,-10650,-10616,-10461,-10319,-10327,-10551,-10971,-11515,-12108,-12695,-13233,-13678,-13975,-14083,-14010,-13806,-13522,-13193,-12845,-12494, },
	/* LAT: -40 */ { -11239,-10890,-10542,-10196, -9857, -9519, -9159, -8734, -8214, -7650, -7227, -7191, -7656, -8482, -9403,-10229,-10884,-11313,-11446,-11272,-10927,-10657,-10655,-10942,-11406,-11912,-12356,-12676,-12833,-12838,-12748,-12612,-12433,-12202,-11915,-11587,-11239, },
	/* LAT: -30 */ {  -9602, -9222, -8841, -8450, -8058, -7683, -7328, -6938, -6426, -5815, -5368, -5459, -6238, -7445, -8695, -9786,-10696,-11400,-11791,-11772,-11394,-10886,-10557,-10563,-10818,-11139,-11394,-11504,-11440,-11264,-11091,-10959,-10815,-10609,-10328, -9981, -9602, },
	/* LAT: -20 */ {  -7372, -6929, -6509, -6080, -5636, -5206, -4817, -4407, -3842, -3158, -2716, -2989, -4113, -5732, -7358, -8721, -9779,-10547,-10977,-11002,-10632,-10014, -9458, -9219, -9271, -9434, -9573, -9588, -9414, -9143, -8952, -8869, -8769, -8563, -8247, -7833, -7372, },
	/* LAT: -10 */ {  -4417, -3876, -3419, -2981, -2523, -2073, -1661, -1211,  -588,   113,   467,    25, -1319, -3245, -5217, -6815, -7899, -8523, -8780, -8710, -8293, -7603, -6940, -6599, -6563, -6656, -6771, -6793, -6604, -6309, -6158, -6175, -6141, -5928, -5544, -5014, -4417, },
	/* LAT:   0 */ {   -909,  -280,   189,   593,  1012,  1428,  1816,  2255,  2834,  3407,  3602,  3092,  1782,  -137, -2178, -3815, -4810, -5227, -5279, -5102, -4655, -3933, -3228, -2861, -2801, -2870, -2993, -3065, -2935, -2701, -2652, -2804, -2876, -2693, -2269, -1636,  -909, },
	/* LAT:  10 */ {   2559,  3190,  3628,  3970,  4326,  4692,  5042,  5424,  5871,  6236,  6266,  5767,  4674,  3101,  1413,    48,  -744,  -981,  -883,  -643,  -226,   418,  1051,  1385,  1448,  1406,  1307,  1216,  1258,  1360,  1279,  1003,   803,   871,  1224,  1829,  2559, },
	/* LAT:  20 */ {   5415,  5946,  6328,  6624,  6938,  7281,  7622,  7966,  8295,  8491,  8399,  7930,  7088,  5988,  4865,  3963,  3443,  3332,  3487,  3734,  4068,  4536,  4997,  5251,  5309,  5294,  5247,  5189,  5176,  5156,  4978,  4639,  4335,  4243,  4415,  4840,  5415, },
	/* LAT:  30 */ {   7568,  7943,  8261,  8544,  8853,  9200,  9556,  9895, 10169, 10279, 10131,  9706,  9074,  8362,  7704,  7196,  6910,  6873,  7019,  7233,  7482,  7783,  8072,  8245,  8303,  8318,  8322,  8312,  8288,  8204,  7979,  7623,  7264,  7043,  7027,  7222,  7568, },
	/* LAT:  40 */ {   9266,  9487,  9743, 10028, 10355, 10715, 11082, 11419, 11669, 11749, 11597, 11236, 10762, 10290,  9896,  9615,  9469,  9468,  9581,  9744,  9921, 10106, 10278, 10403, 10482, 10544, 10600, 10633, 10615, 10503, 10259,  9907,  9538,  9253,  9110,  9123,  9266, },
	/* LAT:  50 */ {  10802, 10923, 11124, 11393, 11716, 12069, 12422, 12737, 12958, 13016, 12876, 12578, 12214, 11870, 11596, 11411, 11320, 11318, 11386, 11491, 11606, 11722, 11839, 11955, 12074, 12197, 12311, 12384, 12373, 12247, 11999, 11670, 11330, 11045, 10856, 10776, 10802, },
	/* LAT:  60 */ {  12319, 12391, 12541, 12758, 13027, 13327, 13628, 13894, 14069, 14096, 13961, 13713, 13425, 13155, 12935, 12779, 12690, 12660, 12676, 12725, 12794, 12879, 12986, 13119, 13278, 13455, 13619, 13728, 13732, 13611, 13385, 13106, 12828, 12591, 12421, 12329, 12319, },
	/* LAT:  70 */ {  13758, 13799, 13893, 14034, 14212, 14415, 14622, 14803, 14908, 14890, 14755, 14554, 14338, 14134, 13961, 13827, 13733, 13679, 13660, 13674, 13716, 13789, 13893, 14030, 14197, 14384, 14568, 14708, 14753, 14680, 14518, 14320, 14125, 13961, 13840, 13771, 13758, },
	/* LAT:  80 */ {  14995, 15007, 15043, 15102, 15177, 15261, 15339, 15385, 15369, 15293, 15183, 15060, 14938, 14824, 14725, 14643, 14581, 14541, 14522, 14526, 14553, 14603, 14674, 14767, 14879, 15006, 15142, 15274, 15381, 15425, 15385, 15297, 15201, 15116, 15050, 15010, 14995, },
	/* LAT:  90 */ {  15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, 15397, },
};

// Magnetic strength data in milli-Gauss * 10
// Model: WMM-2020,
// Version: 0.5.1.11,
// Date: 2023.074,
static constexpr const int16_t strength_table[19][37] {
	//    LONGITUDE:  -180, -170, -160, -150, -140, -130, -120, -110, -100,  -90,  -80,  -70,  -60,  -50,  -40,  -30,  -20,  -10,    0,   10,   20,   30,   40,   50,   60,   70,   80,   90,  100,  110,  120,  130,  140,  150,  160,  170,  180,
	/* LAT: -90 */ {  5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, 5448, },
	/* LAT: -80 */ {  6055, 5991, 5912, 5820, 5716, 5605, 5486, 5365, 5242, 5122, 5008, 4903, 4809, 4730, 4667, 4622, 4598, 4596, 4618, 4664, 4735, 4829, 4944, 5076, 5220, 5370, 5519, 5663, 5794, 5907, 6000, 6069, 6113, 6132, 6127, 6101, 6055, },
	/* LAT: -70 */ {  6299, 6166, 6015, 5850, 5671, 5478, 5273, 5058, 4839, 4624, 4422, 4240, 4082, 3951, 3847, 3771, 3724, 3712, 3741, 3817, 3946, 4126, 4355, 4624, 4919, 5226, 5530, 5814, 6063, 6267, 6418, 6513, 6554, 6547, 6496, 6412, 6299, },
	/* LAT: -60 */ {  6185, 5992, 5790, 5581, 5361, 5127, 4870, 4592, 4299, 4010, 3743, 3516, 3337, 3203, 3104, 3031, 2980, 2960, 2987, 3078, 3247, 3499, 3827, 4214, 4636, 5071, 5492, 5876, 6200, 6449, 6612, 6690, 6691, 6627, 6513, 6362, 6185, },
	/* LAT: -50 */ {  5842, 5612, 5379, 5148, 4915, 4670, 4399, 4096, 3767, 3436, 3134, 2895, 2734, 2643, 2595, 2560, 2526, 2499, 2505, 2578, 2752, 3044, 3441, 3914, 4421, 4927, 5402, 5820, 6159, 6400, 6537, 6578, 6535, 6424, 6261, 6063, 5842, },
	/* LAT: -40 */ {  5392, 5146, 4900, 4660, 4426, 4188, 3933, 3649, 3335, 3009, 2711, 2489, 2373, 2347, 2366, 2387, 2391, 2378, 2365, 2394, 2526, 2804, 3224, 3739, 4286, 4810, 5276, 5664, 5955, 6143, 6230, 6233, 6163, 6031, 5850, 5631, 5392, },
	/* LAT: -30 */ {  4878, 4636, 4397, 4162, 3936, 3717, 3498, 3266, 3010, 2735, 2477, 2296, 2227, 2252, 2319, 2390, 2454, 2504, 2524, 2536, 2605, 2807, 3171, 3661, 4193, 4690, 5107, 5422, 5625, 5724, 5750, 5723, 5644, 5511, 5331, 5114, 4878, },
	/* LAT: -20 */ {  4321, 4107, 3898, 3693, 3497, 3314, 3146, 2982, 2804, 2606, 2415, 2282, 2242, 2286, 2376, 2486, 2614, 2741, 2828, 2863, 2889, 2987, 3229, 3615, 4066, 4493, 4840, 5073, 5178, 5185, 5155, 5108, 5026, 4898, 4731, 4534, 4321, },
	/* LAT: -10 */ {  3790, 3629, 3475, 3329, 3193, 3073, 2969, 2877, 2779, 2664, 2542, 2443, 2399, 2424, 2512, 2641, 2796, 2954, 3077, 3138, 3152, 3180, 3305, 3559, 3886, 4208, 4472, 4633, 4668, 4615, 4547, 4484, 4396, 4271, 4122, 3958, 3790, },
	/* LAT:   0 */ {  3412, 3319, 3234, 3161, 3106, 3068, 3041, 3022, 2997, 2948, 2870, 2775, 2696, 2666, 2709, 2812, 2945, 3080, 3194, 3269, 3299, 3322, 3398, 3556, 3766, 3980, 4159, 4264, 4270, 4202, 4114, 4021, 3910, 3779, 3644, 3520, 3412, },
	/* LAT:  10 */ {  3283, 3251, 3230, 3227, 3251, 3298, 3353, 3406, 3440, 3430, 3361, 3247, 3120, 3027, 3002, 3044, 3125, 3223, 3323, 3408, 3472, 3534, 3623, 3742, 3878, 4017, 4136, 4206, 4208, 4146, 4036, 3892, 3731, 3571, 3436, 3339, 3283, },
	/* LAT:  20 */ {  3399, 3402, 3427, 3481, 3572, 3693, 3821, 3938, 4018, 4030, 3957, 3815, 3649, 3510, 3435, 3424, 3460, 3533, 3630, 3727, 3818, 3917, 4028, 4140, 4249, 4361, 4464, 4529, 4539, 4480, 4343, 4140, 3913, 3703, 3540, 3439, 3399, },
	/* LAT:  30 */ {  3722, 3728, 3782, 3881, 4023, 4194, 4369, 4525, 4633, 4659, 4586, 4431, 4243, 4080, 3975, 3930, 3934, 3986, 4073, 4172, 4270, 4376, 4492, 4610, 4730, 4857, 4978, 5065, 5090, 5032, 4875, 4635, 4361, 4106, 3907, 3778, 3722, },
	/* LAT:  40 */ {  4222, 4219, 4283, 4406, 4572, 4759, 4942, 5098, 5203, 5229, 5161, 5013, 4827, 4654, 4526, 4452, 4426, 4449, 4510, 4590, 4677, 4774, 4889, 5023, 5174, 5336, 5488, 5598, 5636, 5581, 5425, 5187, 4913, 4653, 4443, 4297, 4222, },
	/* LAT:  50 */ {  4832, 4823, 4878, 4987, 5132, 5291, 5441, 5562, 5636, 5646, 5583, 5456, 5294, 5131, 4995, 4898, 4844, 4833, 4858, 4908, 4978, 5069, 5188, 5339, 5514, 5699, 5865, 5982, 6025, 5980, 5849, 5653, 5429, 5214, 5034, 4904, 4832, },
	/* LAT:  60 */ {  5392, 5378, 5405, 5467, 5552, 5646, 5733, 5800, 5834, 5826, 5772, 5679, 5559, 5431, 5312, 5216, 5150, 5117, 5115, 5143, 5200, 5287, 5405, 5552, 5719, 5887, 6034, 6137, 6180, 6158, 6075, 5949, 5804, 5661, 5539, 5447, 5392, },
	/* LAT:  70 */ {  5726, 5705, 5701, 5712, 5734, 5760, 5784, 5799, 5799, 5781, 5742, 5685, 5616, 5540, 5466, 5403, 5355, 5327, 5323, 5343, 5388, 5458, 5549, 5658, 5775, 5889, 5988, 6060, 6099, 6103, 6074, 6021, 5954, 5883, 5818, 5764, 5726, },
	/* LAT:  80 */ {  5790, 5772, 5757, 5745, 5734, 5725, 5715, 5703, 5689, 5671, 5649, 5624, 5597, 5571, 5546, 5526, 5513, 5508, 5513, 5528, 5554, 5589, 5632, 5680, 5729, 5777, 5820, 5854, 5878, 5891, 5893, 5886, 5872, 5853, 5831, 5810, 5790, },
	/* LAT:  90 */ {  5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, 5683, },
};
