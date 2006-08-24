/* Copyright (C) 2002 Jean-Marc Valin 
   File: gain_table.c
   Codebook for 3-tap pitch prediction gain (128 entries)
  
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

const signed char gain_cdbk_nb[384] = {
-32,-32,-32,
-28,-67,-5,
-42,-6,-32,
-57,-10,-54,
-16,27,-41,
19,-19,-40,
-45,24,-21,
-8,-14,-18,
1,14,-58,
-18,-88,-39,
-38,21,-18,
-19,20,-43,
10,17,-48,
-52,-58,-13,
-44,-1,-11,
-12,-11,-34,
14,0,-46,
-37,-35,-34,
-25,44,-30,
6,-4,-63,
-31,43,-41,
-23,30,-43,
-43,26,-14,
-33,1,-13,
-13,18,-37,
-46,-73,-45,
-36,24,-25,
-36,-11,-20,
-25,12,-18,
-36,-69,-59,
-45,6,8,
-22,-14,-24,
-1,13,-44,
-39,-48,-26,
-32,31,-37,
-33,15,-46,
-24,30,-36,
-41,31,-23,
-50,22,-4,
-22,2,-21,
-17,30,-34,
-7,-60,-28,
-38,42,-28,
-44,-11,21,
-16,8,-44,
-39,-55,-43,
-11,-35,26,
-9,0,-34,
-8,121,-81,
7,-16,-22,
-37,33,-31,
-27,-7,-36,
-34,70,-57,
-37,-11,-48,
-40,17,-1,
-33,6,-6,
-9,0,-20,
-21,69,-33,
-29,33,-31,
-55,12,-1,
-33,27,-22,
-50,-33,-47,
-50,54,51,
-1,-5,-44,
-4,22,-40,
-39,-66,-25,
-33,1,-26,
-24,-23,-25,
-11,21,-45,
-25,-45,-19,
-43,105,-16,
5,-21,1,
-16,11,-33,
-13,-99,-4,
-37,33,-15,
-25,37,-63,
-36,24,-31,
-53,-56,-38,
-41,-4,4,
-33,13,-30,
49,52,-94,
-5,-30,-15,
1,38,-40,
-23,12,-36,
-17,40,-47,
-37,-41,-39,
-49,34,0,
-18,-7,-4,
-16,17,-27,
30,5,-62,
4,48,-68,
-43,11,-11,
-18,19,-15,
-23,-62,-39,
-42,10,-2,
-21,-13,-13,
-9,13,-47,
-23,-62,-24,
-44,60,-21,
-18,-3,-52,
-22,22,-36,
-75,57,16,
-19,3,10,
-29,23,-38,
-5,-62,-51,
-51,40,-18,
-42,13,-24,
-34,14,-20,
-56,-75,-26,
-26,32,15,
-26,17,-29,
-7,28,-52,
-12,-30,5,
-5,-48,-5,
2,2,-43,
21,16,16,
-25,-45,-32,
-43,18,-10,
9,0,-1,
-1,7,-30,
19,-48,-4,
-28,25,-29,
-22,0,-31,
-32,17,-10,
-64,-41,-62,
-52,15,16,
-30,-22,-32,
-7,9,-38};
