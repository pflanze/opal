/* Copyright (C) 2002 Jean-Marc Valin 
   File: gain_table_lbr.c
   Codebook for 3-tap pitch prediction gain (32 entries)
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.  

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

float gain_cdbk_lbr[] = {
0,0,0,0,0,0,0,0,0,0,0,0,
0.019578,  -0.411369,  0.250244,  0.039156,  -0.822738,  0.500488,  0.0161076,  0.205885,  -0.00979855,  -0.000383298,  -0.169224,  -0.0626221,
-0.141413,  0.127455,  -0.177373,  -0.282826,  0.25491,  -0.354746,  0.0360476,  0.0452142,  -0.0501657,  -0.0199976,  -0.0162448,  -0.0314612,
-0.379174,  0.154715,  -0.359933,  -0.758348,  0.30943,  -0.719866,  0.117328,  0.111374,  -0.272954,  -0.143773,  -0.0239367,  -0.129552,
0.295340,  1.014952,  -0.144606,  0.59068,  2.0299,  -0.289212,  -0.599512,  0.293536,  0.0854159,  -0.0872257,  -1.03013,  -0.0209109,
0.431555,  -0.107415,  0.360701,  0.86311,  -0.21483,  0.721402,  0.092711,  0.0774894,  -0.311325,  -0.18624,  -0.011538,  -0.130105,
-0.141305,  0.735394,  0.312635,  -0.28261,  1.47079,  0.62527,  0.20783,  -0.45982,  0.0883538,  -0.0199671,  -0.540804,  -0.0977406,
0.382416,  0.267769,  0.318738,  0.764832,  0.535538,  0.637476,  -0.204798,  -0.170696,  -0.243781,  -0.146242,  -0.0717002,  -0.101594,
0.511146,  0.524061,  -0.190435,  1.02229,  1.04812,  -0.38087,  -0.535743,  0.199599,  0.19468,  -0.26127,  -0.27464,  -0.0362655,
0.153482,  -0.531485,  -0.149959,  0.306964,  -1.06297,  -0.299918,  0.163147,  -0.159402,  0.046032,  -0.0235567,  -0.282476,  -0.0224877,
-0.094091,  0.930054,  0.139366,  -0.188182,  1.86011,  0.278732,  0.175019,  -0.259236,  0.0262262,  -0.00885312,  -0.865,  -0.0194229,
0.164167,  0.711936,  -0.077780,  0.328334,  1.42387,  -0.15556,  -0.233753,  0.110749,  0.0255378,  -0.0269508,  -0.506853,  -0.00604973,
0.503705,  0.823130,  -0.273699,  1.00741,  1.64626,  -0.547398,  -0.829229,  0.45058,  0.275727,  -0.253719,  -0.677543,  -0.0749111,
-0.330264,  -0.613346,  0.085310,  -0.660528,  -1.22669,  0.17062,  -0.405132,  0.104649,  0.0563496,  -0.109074,  -0.376193,  -0.0072778,
-0.083597,  0.481953,  0.201470,  -0.167194,  0.963906,  0.40294,  0.0805796,  -0.194198,  0.0336846,  -0.00698846,  -0.232279,  -0.0405902,
0.195682,  0.429066,  0.059682,  0.391364,  0.858132,  0.119364,  -0.167921,  -0.051215,  -0.0233574,  -0.0382914,  -0.184098,  -0.00356194,
0.598746,  1.523378,  -0.189717,  1.19749,  3.04676,  -0.379434,  -1.82423,  0.578021,  0.227185,  -0.358497,  -2.32068,  -0.0359925,
-0.010502,  -0.257728,  -0.018047,  -0.021004,  -0.515456,  -0.036094,  -0.00541332,  -0.00930243,  -0.000379059,  -0.000110292,  -0.0664237,  -0.000325694,
-0.132438,  1.383543,  0.280042,  -0.264876,  2.76709,  0.560084,  0.366467,  -0.7749,  0.0741764,  -0.0175398,  -1.91419,  -0.0784235,
0.234771,  0.555249,  -0.210053,  0.469542,  1.1105,  -0.420106,  -0.260713,  0.233263,  0.0986287,  -0.0551174,  -0.308301,  -0.0441223,
0.010973,  1.090455,  -0.009557,  0.021946,  2.18091,  -0.019114,  -0.0239311,  0.020843,  0.000209738,  -0.000120407,  -1.18909,  -9.13362e-05,
0.141315,  0.930896,  -0.128939,  0.28263,  1.86179,  -0.257878,  -0.263099,  0.240058,  0.036442,  -0.0199699,  -0.866567,  -0.0166253,
-0.168645,  0.950529,  0.314244,  -0.33729,  1.90106,  0.628488,  0.320604,  -0.597396,  0.105991,  -0.0284411,  -0.903505,  -0.0987493,
-0.028768,  0.695554,  0.133637,  -0.057536,  1.39111,  0.267274,  0.0400194,  -0.185903,  0.00768894,  -0.000827598,  -0.483795,  -0.0178588,
0.246305,  0.740436,  0.073124,  0.49261,  1.48087,  0.146248,  -0.364746,  -0.108287,  -0.0360216,  -0.0606662,  -0.548245,  -0.00534712,
0.280190,  -0.787092,  0.268726,  0.56038,  -1.57418,  0.537452,  0.441071,  0.423024,  -0.150589,  -0.0785064,  -0.619514,  -0.0722137,
0.010162,  0.894487,  0.006648,  0.020324,  1.78897,  0.013296,  -0.0181796,  -0.0118931,  -0.000135114,  -0.000103266,  -0.800107,  -4.41959e-05,
0.177218,  0.572144,  0.427882,  0.354436,  1.14429,  0.855764,  -0.202788,  -0.48962,  -0.151657,  -0.0314062,  -0.327349,  -0.183083,
-0.237882,  -0.484537,  -0.303846,  -0.475764,  -0.969074,  -0.607692,  -0.230525,  -0.294449,  -0.144559,  -0.0565878,  -0.234776,  -0.0923224,
-0.211570,  0.684685,  0.539195,  -0.42314,  1.36937,  1.07839,  0.289718,  -0.738357,  0.228155,  -0.0447619,  -0.468794,  -0.290731,
0.064373,  0.236576,  0.042304,  0.128746,  0.473152,  0.084608,  -0.0304582,  -0.0200162,  -0.00544647,  -0.00414388,  -0.0559682,  -0.00178963,
0.347794,  0.726175,  -0.126887,  0.695588,  1.45235,  -0.253774,  -0.505119,  0.184284,  0.0882611,  -0.120961,  -0.52733,  -0.0161003,
};
