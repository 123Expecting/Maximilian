/*
 *  maximilian.cpp
 *  platform independent synthesis library using portaudio or rtaudio
 *
 *  Created by Mick Grierson on 29/12/2009.
 *  Copyright 2009 Mick Grierson & Strangeloop Limited. All rights reserved.
 *	Thanks to the Goldsmiths Creative Computing Team.
 *	Special thanks to Arturo Castro for the PortAudio implementation.
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without
 *	restriction, including without limitation the rights to use,
 *	copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following
 *	conditions:
 *	
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *	OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "maximilian.h"
#include "math.h"
#include <sstream>


#ifdef VORBIS
extern "C" {
    #include "stb_vorbis.h"
}
#endif

//This used to be important for dealing with multichannel playback
float chandiv= 1;

int maxiSettings::sampleRate = 44100;
int maxiSettings::channels = 2;
int maxiSettings::bufferSize = 1024;


//this is a 514-point sinewave table that has many uses. 
maxiType sineBuffer[514]={0,0.012268,0.024536,0.036804,0.049042,0.06131,0.073547,0.085785,0.097992,0.1102,0.12241,0.13455,0.1467,0.15884,0.17093,0.18301,0.19507,0.20709,0.21909,0.23105,0.24295,0.25485,0.26669,0.2785,0.29025,0.30197,0.31366,0.32529,0.33685,0.34839,0.35986,0.37128,0.38266,0.39395,0.40521,0.41641,0.42752,0.4386,0.44958,0.46051,0.47137,0.48215,0.49286,0.50351,0.51407,0.52457,0.53497,0.54529,0.55554,0.5657,0.57578,0.58575,0.59567,0.60547,0.6152,0.62482,0.63437,0.6438,0.65314,0.66238,0.67151,0.68057,0.68951,0.69833,0.70706,0.7157,0.72421,0.7326,0.74091,0.74908,0.75717,0.76514,0.77298,0.7807,0.7883,0.79581,0.80316,0.81042,0.81754,0.82455,0.83142,0.8382,0.84482,0.85132,0.8577,0.86392,0.87006,0.87604,0.88187,0.8876,0.89319,0.89862,0.90396,0.90912,0.91415,0.91907,0.92383,0.92847,0.93295,0.93729,0.9415,0.94556,0.94949,0.95325,0.95691,0.96039,0.96375,0.96692,0.97,0.9729,0.97565,0.97827,0.98074,0.98306,0.98523,0.98724,0.98914,0.99084,0.99243,0.99387,0.99515,0.99628,0.99725,0.99808,0.99875,0.99927,0.99966,0.99988,0.99997,0.99988,0.99966,0.99927,0.99875,0.99808,0.99725,0.99628,0.99515,0.99387,0.99243,0.99084,0.98914,0.98724,0.98523,0.98306,0.98074,0.97827,0.97565,0.9729,0.97,0.96692,0.96375,0.96039,0.95691,0.95325,0.94949,0.94556,0.9415,0.93729,0.93295,0.92847,0.92383,0.91907,0.91415,0.90912,0.90396,0.89862,0.89319,0.8876,0.88187,0.87604,0.87006,0.86392,0.8577,0.85132,0.84482,0.8382,0.83142,0.82455,0.81754,0.81042,0.80316,0.79581,0.7883,0.7807,0.77298,0.76514,0.75717,0.74908,0.74091,0.7326,0.72421,0.7157,0.70706,0.69833,0.68951,0.68057,0.67151,0.66238,0.65314,0.6438,0.63437,0.62482,0.6152,0.60547,0.59567,0.58575,0.57578,0.5657,0.55554,0.54529,0.53497,0.52457,0.51407,0.50351,0.49286,0.48215,0.47137,0.46051,0.44958,0.4386,0.42752,0.41641,0.40521,0.39395,0.38266,0.37128,0.35986,0.34839,0.33685,0.32529,0.31366,0.30197,0.29025,0.2785,0.26669,0.25485,0.24295,0.23105,0.21909,0.20709,0.19507,0.18301,0.17093,0.15884,0.1467,0.13455,0.12241,0.1102,0.097992,0.085785,0.073547,0.06131,0.049042,0.036804,0.024536,0.012268,0,-0.012268,-0.024536,-0.036804,-0.049042,-0.06131,-0.073547,-0.085785,-0.097992,-0.1102,-0.12241,-0.13455,-0.1467,-0.15884,-0.17093,-0.18301,-0.19507,-0.20709,-0.21909,-0.23105,-0.24295,-0.25485,-0.26669,-0.2785,-0.29025,-0.30197,-0.31366,-0.32529,-0.33685,-0.34839,-0.35986,-0.37128,-0.38266,-0.39395,-0.40521,-0.41641,-0.42752,-0.4386,-0.44958,-0.46051,-0.47137,-0.48215,-0.49286,-0.50351,-0.51407,-0.52457,-0.53497,-0.54529,-0.55554,-0.5657,-0.57578,-0.58575,-0.59567,-0.60547,-0.6152,-0.62482,-0.63437,-0.6438,-0.65314,-0.66238,-0.67151,-0.68057,-0.68951,-0.69833,-0.70706,-0.7157,-0.72421,-0.7326,-0.74091,-0.74908,-0.75717,-0.76514,-0.77298,-0.7807,-0.7883,-0.79581,-0.80316,-0.81042,-0.81754,-0.82455,-0.83142,-0.8382,-0.84482,-0.85132,-0.8577,-0.86392,-0.87006,-0.87604,-0.88187,-0.8876,-0.89319,-0.89862,-0.90396,-0.90912,-0.91415,-0.91907,-0.92383,-0.92847,-0.93295,-0.93729,-0.9415,-0.94556,-0.94949,-0.95325,-0.95691,-0.96039,-0.96375,-0.96692,-0.97,-0.9729,-0.97565,-0.97827,-0.98074,-0.98306,-0.98523,-0.98724,-0.98914,-0.99084,-0.99243,-0.99387,-0.99515,-0.99628,-0.99725,-0.99808,-0.99875,-0.99927,-0.99966,-0.99988,-0.99997,-0.99988,-0.99966,-0.99927,-0.99875,-0.99808,-0.99725,-0.99628,-0.99515,-0.99387,-0.99243,-0.99084,-0.98914,-0.98724,-0.98523,-0.98306,-0.98074,-0.97827,-0.97565,-0.9729,-0.97,-0.96692,-0.96375,-0.96039,-0.95691,-0.95325,-0.94949,-0.94556,-0.9415,-0.93729,-0.93295,-0.92847,-0.92383,-0.91907,-0.91415,-0.90912,-0.90396,-0.89862,-0.89319,-0.8876,-0.88187,-0.87604,-0.87006,-0.86392,-0.8577,-0.85132,-0.84482,-0.8382,-0.83142,-0.82455,-0.81754,-0.81042,-0.80316,-0.79581,-0.7883,-0.7807,-0.77298,-0.76514,-0.75717,-0.74908,-0.74091,-0.7326,-0.72421,-0.7157,-0.70706,-0.69833,-0.68951,-0.68057,-0.67151,-0.66238,-0.65314,-0.6438,-0.63437,-0.62482,-0.6152,-0.60547,-0.59567,-0.58575,-0.57578,-0.5657,-0.55554,-0.54529,-0.53497,-0.52457,-0.51407,-0.50351,-0.49286,-0.48215,-0.47137,-0.46051,-0.44958,-0.4386,-0.42752,-0.41641,-0.40521,-0.39395,-0.38266,-0.37128,-0.35986,-0.34839,-0.33685,-0.32529,-0.31366,-0.30197,-0.29025,-0.2785,-0.26669,-0.25485,-0.24295,-0.23105,-0.21909,-0.20709,-0.19507,-0.18301,-0.17093,-0.15884,-0.1467,-0.13455,-0.12241,-0.1102,-0.097992,-0.085785,-0.073547,-0.06131,-0.049042,-0.036804,-0.024536,-0.012268,0,0.012268
};

// This is a transition table that helps with bandlimited oscs.
maxiType transition[1001]={-0.500003,-0.500003,-0.500023,-0.500063,-0.500121,-0.500179,-0.500259,
    -0.50036,-0.500476,-0.500591,-0.500732,-0.500893,-0.501066,-0.501239,
    -0.50144,-0.501661,-0.501891,-0.502123,-0.502382,-0.502662,-0.502949,
    -0.50324,-0.503555,-0.503895,-0.504238,-0.504587,-0.504958,-0.505356,
    -0.505754,-0.506162,-0.506589,-0.507042,-0.507495,-0.50796,-0.508444,
    -0.508951,-0.509458,-0.509979,-0.510518,-0.511079,-0.511638,-0.512213,
    -0.512808,-0.51342,-0.51403,-0.514659,-0.515307,-0.51597,-0.51663,-0.517312,
    -0.518012,-0.518724,-0.519433,-0.520166,-0.520916,-0.521675,-0.522432,
    -0.523214,-0.524013,-0.524819,-0.525624,-0.526451,-0.527298,-0.528147,
    -0.528999,-0.52987,-0.530762,-0.531654,-0.532551,-0.533464,-0.534399,
    -0.535332,-0.536271,-0.537226,-0.538202,-0.539172,-0.540152,-0.541148,
    -0.542161,-0.543168,-0.544187,-0.54522,-0.546269,-0.54731,-0.548365,
    -0.549434,-0.550516,-0.55159,-0.552679,-0.553781,-0.554893,-0.555997,
    -0.557118,-0.558252,-0.559391,-0.560524,-0.561674,-0.562836,-0.564001,
    -0.565161,-0.566336,-0.567524,-0.568712,-0.569896,-0.571095,-0.572306,
    -0.573514,-0.574721,-0.575939,-0.577171,-0.578396,-0.579622,-0.580858,
    -0.582108,-0.583348,-0.58459,-0.585842,-0.587106,-0.588358,-0.589614,
    -0.590879,-0.592154,-0.593415,-0.594682,-0.595957,-0.59724,-0.598507,
    -0.599782,-0.601064,-0.602351,-0.603623,-0.604902,-0.606189,-0.607476,
    -0.60875,-0.610032,-0.611319,-0.612605,-0.613877,-0.615157,-0.616443,
    -0.617723,-0.618992,-0.620268,-0.621548,-0.62282,-0.624083,-0.62535,
    -0.626622,-0.627882,-0.629135,-0.630391,-0.631652,-0.632898,-0.634138,
    -0.63538,-0.636626,-0.637854,-0.639078,-0.640304,-0.641531,-0.642739,
    -0.643943,-0.645149,-0.646355,-0.647538,-0.64872,-0.649903,-0.651084,
    -0.652241,-0.653397,-0.654553,-0.655705,-0.656834,-0.657961,-0.659087,
    -0.660206,-0.661304,-0.662399,-0.663492,-0.664575,-0.665639,-0.666699,
    -0.667756,-0.6688,-0.669827,-0.670849,-0.671866,-0.672868,-0.673854,
    -0.674835,-0.675811,-0.676767,-0.677709,-0.678646,-0.679576,-0.680484,
    -0.68138,-0.682269,-0.683151,-0.684008,-0.684854,-0.685693,-0.686524,
    -0.687327,-0.688119,-0.688905,-0.689682,-0.690428,-0.691164,-0.691893,
    -0.692613,-0.6933,-0.693978,-0.694647,-0.695305,-0.695932,-0.696549,
    -0.697156,-0.697748,-0.698313,-0.698865,-0.699407,-0.699932,-0.700431,
    -0.700917,-0.701391,-0.701845,-0.702276,-0.702693,-0.703097,-0.703478,
    -0.703837,-0.704183,-0.704514,-0.704819,-0.705105,-0.705378,-0.705633,
    -0.70586,-0.706069,-0.706265,-0.706444,-0.706591,-0.706721,-0.706837,
    -0.706938,-0.707003,-0.707051,-0.707086,-0.707106,-0.707086,-0.707051,
    -0.707001,-0.706935,-0.706832,-0.706711,-0.706576,-0.706421,-0.706233,
    -0.706025,-0.705802,-0.705557,-0.705282,-0.704984,-0.704671,-0.704334,
    -0.703969,-0.703582,-0.703176,-0.702746,-0.702288,-0.70181,-0.701312,
    -0.700785,-0.700234,-0.699664,-0.69907,-0.698447,-0.6978,-0.697135,
    -0.696446,-0.695725,-0.694981,-0.694219,-0.693435,-0.692613,-0.691771,
    -0.690911,-0.69003,-0.689108,-0.688166,-0.687206,-0.686227,-0.685204,
    -0.684162,-0.683101,-0.682019,-0.680898,-0.679755,-0.678592,-0.677407,
    -0.676187,-0.674941,-0.673676,-0.672386,-0.671066,-0.669718,-0.66835,
    -0.666955,-0.665532,-0.664083,-0.662611,-0.661112,-0.659585,-0.658035,
    -0.656459,-0.654854,-0.653223,-0.651572,-0.649892,-0.648181,-0.646446,
    -0.644691,-0.642909,-0.641093,-0.639253,-0.637393,-0.63551,-0.633588,
    -0.631644,-0.62968,-0.627695,-0.625668,-0.623621,-0.621553,-0.619464,
    -0.617334,-0.615183,-0.613011,-0.610817,-0.608587,-0.606333,-0.604058,
    -0.60176,-0.599429,-0.597072,-0.594695,-0.592293,-0.589862,-0.587404,
    -0.584925,-0.58242,-0.579888,-0.577331,-0.574751,-0.572145,-0.569512,
    -0.566858,-0.564178,-0.561471,-0.558739,-0.555988,-0.553209,-0.550402,
    -0.547572,-0.544723,-0.54185,-0.538944,-0.536018,-0.533072,-0.530105,
    -0.527103,-0.524081,-0.52104,-0.51798,-0.514883,-0.511767,-0.508633,
    -0.505479,-0.502291,-0.499083,-0.495857,-0.492611,-0.489335,-0.486037,
    -0.48272,-0.479384,-0.476021,-0.472634,-0.46923,-0.465805,-0.462356,
    -0.458884,-0.455394,-0.451882,-0.448348,-0.444795,-0.44122,-0.437624,
    -0.434008,-0.430374,-0.426718,-0.423041,-0.419344,-0.415631,-0.411897,
    -0.40814,-0.404365,-0.400575,-0.396766,-0.392933,-0.389082,-0.385217,
    -0.381336,-0.377428,-0.373505,-0.369568,-0.365616,-0.361638,-0.357645,
    -0.353638,-0.349617,-0.345572,-0.341512,-0.337438,-0.33335,-0.329242,
    -0.325118,-0.32098,-0.316829,-0.31266,-0.308474,-0.304276,-0.300063,
    -0.295836,-0.291593,-0.287337,-0.283067,-0.278783,-0.274487,-0.270176,
    -0.265852,-0.261515,-0.257168,-0.252806,-0.248431,-0.244045,-0.239649,
    -0.23524,-0.230817,-0.226385,-0.221943,-0.21749,-0.213024,-0.208548,
    -0.204064,-0.199571,-0.195064,-0.190549,-0.186026,-0.181495,-0.176952,
    -0.1724,-0.167842,-0.163277,-0.1587,-0.154117,-0.149527,-0.14493,-0.140325,
    -0.135712,-0.131094,-0.12647,-0.121839,-0.117201,-0.112559,-0.10791,
    -0.103257,-0.0985979,-0.0939343,-0.0892662,-0.0845935,-0.079917,-0.0752362,
    -0.0705516,-0.0658635,-0.0611729,-0.0564786,-0.0517814,-0.0470818,-0.0423802,
    -0.0376765,-0.0329703,-0.0282629,-0.0235542,-0.0188445,-0.0141335,-0.00942183,
    -0.00470983,2.41979e-06,0.00471481,0.00942681,0.0141384,0.0188494,0.023559,
    0.028268,0.0329754,0.0376813,0.0423851,0.0470868,0.0517863,0.0564836,
    0.0611777,0.0658683,0.0705566,0.0752412,0.0799218,0.0845982,0.0892712,
    0.0939393,0.0986028,0.103262,0.107915,0.112563,0.117206,0.121844,0.126475,
    0.131099,0.135717,0.14033,0.144935,0.149531,0.154122,0.158705,0.163281,
    0.167847,0.172405,0.176956,0.1815,0.18603,0.190553,0.195069,0.199576,
    0.204068,0.208552,0.213028,0.217495,0.221947,0.226389,0.230822,0.235245,
    0.239653,0.244049,0.248436,0.252811,0.257173,0.26152,0.265857,0.270181,
    0.274491,0.278788,0.283071,0.287341,0.291597,0.29584,0.300068,0.30428,
    0.308478,0.312664,0.316833,0.320984,0.325122,0.329246,0.333354,0.337442,
    0.341516,0.345576,0.34962,0.353642,0.357649,0.361642,0.36562,0.369572,
    0.373509,0.377432,0.38134,0.385221,0.389086,0.392936,0.39677,0.400579,
    0.404369,0.408143,0.4119,0.415634,0.419347,0.423044,0.426721,0.430377,
    0.434011,0.437627,0.441223,0.444798,0.448351,0.451885,0.455397,0.458887,
    0.462359,0.465807,0.469232,0.472637,0.476024,0.479386,0.482723,0.486039,
    0.489338,0.492613,0.49586,0.499086,0.502294,0.505481,0.508635,0.511769,
    0.514885,0.517982,0.521042,0.524083,0.527105,0.530107,0.533074,0.53602,
    0.538946,0.541851,0.544725,0.547574,0.550404,0.553211,0.555989,0.55874,
    0.561472,0.564179,0.566859,0.569514,0.572146,0.574753,0.577332,0.579889,
    0.582421,0.584926,0.587405,0.589863,0.592294,0.594696,0.597073,0.59943,
    0.60176,0.604059,0.606333,0.608588,0.610818,0.613012,0.615183,0.617335,
    0.619464,0.621553,0.623621,0.625669,0.627696,0.629681,0.631645,0.633588,
    0.63551,0.637393,0.639253,0.641093,0.642909,0.644691,0.646446,0.648181,
    0.649892,0.651572,0.653223,0.654854,0.656459,0.658035,0.659585,0.661112,
    0.662611,0.664083,0.665532,0.666955,0.66835,0.669718,0.671066,0.672386,
    0.673676,0.674941,0.676187,0.677407,0.678592,0.679755,0.680898,0.682019,
    0.683101,0.684162,0.685204,0.686227,0.687206,0.688166,0.689108,0.69003,
    0.690911,0.691771,0.692613,0.693435,0.694219,0.694981,0.695725,0.696447,
    0.697135,0.6978,0.698447,0.69907,0.699664,0.700234,0.700786,0.701312,
    0.70181,0.702288,0.702746,0.703177,0.703582,0.703969,0.704334,0.704671,
    0.704984,0.705282,0.705557,0.705802,0.706025,0.706233,0.706422,0.706576,
    0.706712,0.706832,0.706936,0.707002,0.707051,0.707086,0.707106,0.707086,
    0.707051,0.707003,0.706939,0.706838,0.706721,0.706592,0.706445,0.706265,
    0.70607,0.705861,0.705634,0.705378,0.705105,0.70482,0.704515,0.704184,
    0.703837,0.703478,0.703097,0.702694,0.702276,0.701846,0.701392,0.700917,
    0.700432,0.699932,0.699408,0.698866,0.698314,0.697749,0.697156,0.696549,
    0.695933,0.695305,0.694648,0.693979,0.693301,0.692613,0.691894,0.691165,
    0.690428,0.689683,0.688905,0.68812,0.687327,0.686525,0.685693,0.684854,
    0.684009,0.683152,0.68227,0.68138,0.680485,0.679577,0.678647,0.67771,
    0.676768,0.675811,0.674836,0.673855,0.672869,0.671867,0.670849,0.669827,
    0.668801,0.667757,0.6667,0.66564,0.664576,0.663493,0.6624,0.661305,
    0.660207,0.659088,0.657962,0.656834,0.655705,0.654553,0.653398,0.652241,
    0.651085,0.649903,0.648721,0.647539,0.646356,0.645149,0.643944,0.642739,
    0.641532,0.640304,0.639079,0.637855,0.636626,0.635381,0.634139,0.632899,
    0.631652,0.630392,0.629136,0.627883,0.626622,0.62535,0.624083,0.62282,
    0.621548,0.620268,0.618993,0.617724,0.616443,0.615158,0.613878,0.612605,
    0.61132,0.610032,0.608751,0.607477,0.606189,0.604903,0.603623,0.602351,
    0.601065,0.599782,0.598508,0.59724,0.595957,0.594682,0.593415,0.592154,
    0.59088,0.589615,0.588359,0.587106,0.585843,0.584591,0.583349,0.582108,
    0.580859,0.579623,0.578397,0.577172,0.575939,0.574721,0.573515,0.572307,
    0.571095,0.569897,0.568713,0.567525,0.566337,0.565161,0.564002,0.562837,
    0.561674,0.560525,0.559392,0.558252,0.557119,0.555998,0.554893,0.553782,
    0.552679,0.55159,0.550516,0.549434,0.548365,0.54731,0.546269,0.54522,
    0.544187,0.543168,0.542161,0.541148,0.540153,0.539173,0.538202,0.537226,
    0.536271,0.535332,0.5344,0.533464,0.532551,0.531654,0.530762,0.52987,
    0.528999,0.528147,0.527298,0.526451,0.525624,0.524819,0.524014,0.523214,
    0.522432,0.521675,0.520916,0.520166,0.519433,0.518724,0.518012,0.517312,
    0.51663,0.51597,0.515307,0.51466,0.51403,0.51342,0.512808,0.512213,
    0.511638,0.511079,0.510518,0.509979,0.509458,0.508951,0.508444,0.50796,
    0.507495,0.507042,0.506589,0.506162,0.505754,0.505356,0.504958,0.504587,
    0.504237,0.503895,0.503555,0.50324,0.502949,0.502662,0.502382,0.502123,
    0.501891,0.501661,0.50144,0.501239,0.501066,0.500893,0.500732,0.500591,
    0.500476,0.50036,0.500259,0.500179,0.500121,0.500063,0.500023,0.500003,0.500003};

//This is a lookup table for converting midi to frequency
maxiType mtofarray[129]={0, 8.661957, 9.177024, 9.722718, 10.3, 10.913383, 11.562325, 12.25, 12.978271, 13.75, 14.567617, 15.433853, 16.351599, 17.323914, 18.354048, 19.445436, 20.601723, 21.826765, 23.124651, 24.5, 25.956543, 27.5, 29.135235, 30.867706, 32.703197, 34.647827, 36.708096, 38.890873, 41.203445, 43.65353, 46.249302, 49., 51.913086, 55., 58.27047, 61.735413, 65.406395, 69.295654, 73.416191, 77.781746, 82.406891, 87.30706, 92.498604, 97.998856, 103.826172, 110., 116.540939, 123.470825, 130.81279, 138.591309, 146.832382, 155.563492, 164.813782, 174.61412, 184.997208, 195.997711, 207.652344, 220., 233.081879, 246.94165, 261.62558, 277.182617,293.664764, 311.126984, 329.627563, 349.228241, 369.994415, 391.995422, 415.304688, 440., 466.163757, 493.883301, 523.25116, 554.365234, 587.329529, 622.253967, 659.255127, 698.456482, 739.988831, 783.990845, 830.609375, 880., 932.327515, 987.766602, 1046.502319, 1108.730469, 1174.659058, 1244.507935, 1318.510254, 1396.912964, 1479.977661, 1567.981689, 1661.21875, 1760., 1864.655029, 1975.533203, 2093.004639, 2217.460938, 2349.318115, 2489.015869, 2637.020508, 2793.825928, 2959.955322, 3135.963379, 3322.4375, 3520., 3729.31, 3951.066406, 4186.009277, 4434.921875, 4698.63623, 4978.031738, 5274.041016, 5587.651855, 5919.910645, 6271.926758, 6644.875, 7040., 7458.620117, 7902.132812, 8372.018555, 8869.84375, 9397.272461, 9956.063477, 10548.082031, 11175.303711, 11839.821289, 12543.853516, 13289.75};

void setup();//use this to do any initialisation if you want.

void play(maxiType *channels);//run dac!

///////////////////////////////////////////////////////////////////////////
// maxiOsc
///////////////////////////////////////////////////////////////////////////


maxiOsc::maxiOsc(){
    //When you create an oscillator, the constructor sets the phase of the oscillator to 0.
	phase = 0.0;
}

maxiType maxiOsc::noise() {
    //White Noise
	//always the same unless you seed it.
	float r = rand()/(float)RAND_MAX;
	output=r*2-1;
	return(output);
}

void maxiOsc::phaseReset(maxiType phaseIn) {
    //This allows you to set the phase of the oscillator to anything you like.
	phase=phaseIn;
	
}

maxiType maxiOsc::sinewave(maxiType frequency) {
    //This is a sinewave oscillator
	output=sin (phase*(TWOPI));
	if ( phase >= 1.0 ) phase -= 1.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	return(output);
	
}

maxiType maxiOsc::sinebuf4(maxiType frequency) {
    //This is a sinewave oscillator that uses 4 point interpolation on a 514 point buffer
	maxiType remainder;
	maxiType a,b,c,d,a1,a2,a3;
	phase += 512./(maxiSettings::sampleRate/(frequency));
	if ( phase >= 511 ) phase -=512;
	remainder = phase - floor(phase);
	
	if (phase==0) {
		a=sineBuffer[(long) 512];
		b=sineBuffer[(long) phase];
		c=sineBuffer[(long) phase+1];
		d=sineBuffer[(long) phase+2];
		
	} else {
		a=sineBuffer[(long) phase-1];
		b=sineBuffer[(long) phase];
		c=sineBuffer[(long) phase+1];
		d=sineBuffer[(long) phase+2];
		
	}
	
	a1 = 0.5f * (c - a);
	a2 = a - 2.5 * b + 2.f * c - 0.5f * d;
	a3 = 0.5f * (d - a) + 1.5f * (b - c);
	output = maxiType (((a3 * remainder + a2) * remainder + a1) * remainder + b);
	return(output);
}

maxiType maxiOsc::sinebuf(maxiType frequency) { //specify the frequency of the oscillator in Hz / cps etc.
    //This is a sinewave oscillator that uses linear interpolation on a 514 point buffer
	maxiType remainder;
 	phase += 512./(maxiSettings::sampleRate/(frequency*chandiv));
	if ( phase >= 511 ) phase -=512;
	remainder = phase - floor(phase);
	output = (maxiType) ((1-remainder) * sineBuffer[1+ (long) phase] + remainder * sineBuffer[2+(long) phase]);
	return(output);
}

maxiType maxiOsc::coswave(maxiType frequency) {
    //This is a cosine oscillator
	output=cos (phase*(TWOPI));
	if ( phase >= 1.0 ) phase -= 1.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	return(output);
	
}

maxiType maxiOsc::phasor(maxiType frequency) {
    //This produces a floating point linear ramp between 0 and 1 at the desired frequency 
	output=phase;
	if ( phase >= 1.0 ) phase -= 1.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	return(output);
} 

maxiType maxiOsc::square(maxiType frequency) {
    //This is a square wave
	if (phase<0.5) output=-1;
	if (phase>0.5) output=1;
	if ( phase >= 1.0 ) phase -= 1.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	return(output);
}

maxiType maxiOsc::pulse(maxiType frequency, maxiType duty) {
    //This is a pulse generator that creates a signal between -1 and 1.
	if (duty<0.) duty=0;
	if (duty>1.) duty=1;
	if ( phase >= 1.0 ) phase -= 1.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	if (phase<duty) output=-1.;
	if (phase>duty) output=1.;
	return(output);
}

maxiType maxiOsc::phasor(maxiType frequency, maxiType startphase, maxiType endphase) {
    //This is a phasor that takes a value for the start and end of the ramp. 
	output=phase;
	if (phase<startphase) {
		phase=startphase;
	}
	if ( phase >= endphase ) phase = startphase;
	phase += ((endphase-startphase)/(maxiSettings::sampleRate/(frequency)));
	return(output);
}


maxiType maxiOsc::saw(maxiType frequency) {
	//Sawtooth generator. This is like a phasor but goes between -1 and 1
	output=phase;
	if ( phase >= 1.0 ) phase -= 2.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	return(output);
	
} 

maxiType maxiOsc::triangle(maxiType frequency) {
    //This is a triangle wave.
	if ( phase >= 1.0 ) phase -= 1.0;
	phase += (1./(maxiSettings::sampleRate/(frequency)));
	if (phase <= 0.5 ) {
		output =(phase - 0.25) * 4;
	} else {
		output =((1.0-phase) - 0.25) * 4;
	}
	return(output);
	
}

maxiType maxiOsc::sawn(maxiType frequency) {
	//Bandlimited sawtooth generator. Woohoo.
    if ( phase >= 0.5 ) phase -= 1.0;
    phase += (1./(maxiSettings::sampleRate/(frequency)));
	maxiType temp=(8820.22/frequency)*phase;
    if (temp<-0.5) {
        temp=-0.5;
    }
    if (temp>0.5) {
        temp=0.5;
    }
    temp*=1000.0f;
    temp+=500.0f;
    maxiType remainder = temp - floor(temp);
    output = (maxiType) ((1.0f-remainder) * transition[(long)temp] + remainder * transition[1+(long)temp]) - phase;
	return(output);
	
}

///////////////////////////////////////////////////////////////////////////
// maxiEnvelope
///////////////////////////////////////////////////////////////////////////

maxiType maxiEnvelope::line(int numberofsegments,maxiType segments[1000]) {
	//This is a basic multi-segment ramp generator that you can use for more or less anything.
    //However, it's not that intuitive.
    if (isPlaying==1) {//only make a sound once you've been triggered
	period=2./(segments[valindex+1]*0.004);
	nextval=segments[valindex+2];
	currentval=segments[valindex];
	if (currentval-amplitude > 0.0000001 && valindex < numberofsegments) {
		amplitude += ((currentval-startval)/(maxiSettings::sampleRate/period));
	} else if (currentval-amplitude < -0.0000001 && valindex < numberofsegments) {
		amplitude -= (((currentval-startval)*(-1))/(maxiSettings::sampleRate/period));
	} else if (valindex >numberofsegments-1) {
		valindex=numberofsegments-2;
	} else {
		valindex=valindex+2;
		startval=currentval;
	}
	output=amplitude;
		
	}
	else {
		output=0;
		
	}
	return(output);
}

//and this
void maxiEnvelope::trigger(int index, maxiType amp) {
	isPlaying=1;//ok the envelope is being used now.
	valindex=index;
	amplitude=amp;
	
}

///////////////////////////////////////////////////////////////////////////
// maxiDelayline
///////////////////////////////////////////////////////////////////////////

//Delay with feedback
maxiDelayline::maxiDelayline() {
	memset( memory, 0, 88200*sizeof (maxiType) );
    phase=0;
}


maxiType maxiDelayline::dl(maxiType input, int size, maxiType feedback)  {
	if ( phase >=size ) {
		phase = 0;
	}
	output=memory[phase];
	memory[phase]=(memory[phase]*feedback)+(input*feedback)*0.5;
	phase+=1;
	return(output);
	
}

maxiType maxiDelayline::dl(maxiType input, int size, maxiType feedback, int position)  {
	if ( phase >=size ) phase = 0;
	if ( position >=size ) position = 0;
	output=memory[position];
	memory[phase]=(memory[phase]*feedback)+(input*feedback)*chandiv;
	phase+=1;
	return(output);
	
}

///////////////////////////////////////////////////////////////////////////
// maxiFilter
///////////////////////////////////////////////////////////////////////////

//I particularly like these. cutoff between 0 and 1
maxiType maxiFilter::lopass(maxiType input, maxiType cutoff) {
	output=outputs[0] + cutoff*(input-outputs[0]);
	outputs[0]=output;
	return(output);
}
//as above
maxiType maxiFilter::hipass(maxiType input, maxiType cutoff) {
	output=input-(outputs[0] + cutoff*(input-outputs[0]));
	outputs[0]=output;
	return(output);
}
//awesome. cuttof is freq in hz. res is between 1 and whatever. Watch out!
maxiType maxiFilter::lores(maxiType input,maxiType cutoff1, maxiType resonance) {
	cutoff=cutoff1*0.5;
	if (cutoff<10) cutoff=10;
	if (cutoff>(maxiSettings::sampleRate*0.5)) cutoff=(maxiSettings::sampleRate*0.5);
	if (resonance<1.) resonance = 1.;
	z=cos(TWOPI*cutoff/maxiSettings::sampleRate);
	c=2-2*z;
	maxiType r=(sqrt(2.0)*sqrt(-pow(static_cast<maxiType>(z-1.0),static_cast<maxiType>(3.0)))+resonance*(z-1))/(resonance*(z-1));
	x=x+(input-y)*c;
	y=y+x;
	x=x*r;
	output=y;
	return(output);
}

//working hires filter
maxiType maxiFilter::hires(maxiType input,maxiType cutoff1, maxiType resonance) {
	cutoff=cutoff1*0.5;
	if (cutoff<10) cutoff=10;
	if (cutoff>(maxiSettings::sampleRate*0.5)) cutoff=(maxiSettings::sampleRate*0.5);
	if (resonance<1.) resonance = 1.;
	z=cos(TWOPI*cutoff/maxiSettings::sampleRate);
	c=2-2*z;
	maxiType r=(sqrt(2.0)*sqrt(-pow(static_cast<maxiType>(z-1.0),static_cast<maxiType>(3.0)))+resonance*(z-1))/(resonance*(z-1));
	x=x+(input-y)*c;
	y=y+x;
	x=x*r;
	output=input-y;
	return(output);
}

//This works a bit. Needs attention.
maxiType maxiFilter::bandpass(maxiType input,maxiType cutoff1, maxiType resonance) {
	cutoff=cutoff1;
	if (cutoff>(maxiSettings::sampleRate*0.5)) cutoff=(maxiSettings::sampleRate*0.5);
	if (resonance>=1.) resonance=0.999999;
	z=cos(TWOPI*cutoff/maxiSettings::sampleRate);
	inputs[0] = (1-resonance)*(sqrt(resonance*(resonance-4.0*pow(static_cast<maxiType>(z),static_cast<maxiType>(2.0))+2.0)+1));
	inputs[1] = 2*z*resonance;
	inputs[2] = pow(static_cast<maxiType>(resonance*-1),static_cast<maxiType>(2.0));
	
	output=inputs[0]*input+inputs[1]*outputs[1]+inputs[2]*outputs[2];
	outputs[2]=outputs[1];
	outputs[1]=output;
	return(output);
}

///////////////////////////////////////////////////////////////////////////
// maxiMix
///////////////////////////////////////////////////////////////////////////

//stereo bus
maxiType *maxiMix::stereo(maxiType input,maxiType two[2],maxiType x) {
	if (x>1) x=1;
	if (x<0) x=0;
	two[0]=input*sqrt(1.0-x);
	two[1]=input*sqrt(x);
	return(two);
} 

//quad bus
maxiType *maxiMix::quad(maxiType input,maxiType four[4],maxiType x,maxiType y) {
	if (x>1) x=1;
	if (x<0) x=0;
	if (y>1) y=1;
	if (y<0) y=0;
	four[0]=input*sqrt((1.0-x)*y);
	four[1]=input*sqrt((1.0-x)*(1.0-y));
	four[2]=input*sqrt(x*y);
	four[3]=input*sqrt(x*(1.0-y));
	return(four);
}

//ambisonic bus
maxiType *maxiMix::ambisonic(maxiType input,maxiType eight[8],maxiType x,maxiType y,maxiType z) {
	if (x>1) x=1;
	if (x<0) x=0;
	if (y>1) y=1;
	if (y<0) y=0;
	if (z>1) y=1;
	if (z<0) y=0;
	eight[0]=input*(sqrt((1.0-x)*y)*1.0-z);
	eight[1]=input*(sqrt((1.0-x)*(1.0-y))*1.0-z);
	eight[2]=input*(sqrt(x*y)*1.0-z);
	eight[3]=input*(sqrt(x*(1.0-y))*1.0-z);
	eight[4]=input*(sqrt((1.0-x)*y)*z);
	eight[5]=input*(sqrt((1.0-x)*(1.0-y))*z);
	eight[6]=input*sqrt((x*y)*z);
	eight[7]=input*sqrt((x*(1.0-y))*z);
	return(eight);
}

///////////////////////////////////////////////////////////////////////////
// maxiSampler
///////////////////////////////////////////////////////////////////////////

template<class source>
bool maxiSampler<source>::load(string fileName, int channel) {
    return samples.load(fileName, channel);
}

template<class source>
void maxiSampler<source>::trigger() {
	position = 0;
    recordPosition = 0;
}


template<class source>
maxiType maxiSampler<source>::play() {
	position++;
	if ((long) position >= samples.getLength()) position=0;
	output = (maxiType) samples[(long)position]/32767.0;
	return output;
}

//start end and points are between 0 and 1
template<class source>
maxiType maxiSampler<source>::playLoop(maxiType start, maxiType end) {
	position++;
    if (position < samples.getLength() * start) position = samples.getLength() * start;
	if ((long) position >= samples.getLength() * end) position = samples.getLength() * start;
	output = (maxiType) samples[(long)position]/32767.0;
	return output;
}

template<class source>
maxiType maxiSampler<source>::playOnce() {
	position++;
	if ((long) position< samples.getLength())
        output = (maxiType) samples[(long)position]/32767.0;
    else {
        output=0;
    }
	return output;

}

template<class source>
void maxiSampler<source>::setPosition(maxiType newPos) {
    position = maxiMap::clamp<maxiType>(newPos, 0.0, 1.0) * samples.getLength();
}

template<class source>
maxiType maxiSampler<source>::playUntil(maxiType end) {
	position++;
	if ((long) position < samples.getLength() * end)
        output = (maxiType) samples[(long)position]/32767.0;
    else {
        output=0;
    }
	return output;
}

template<class source>
maxiType maxiSampler<source>::playOnce(maxiType speed) {
	position=position+((speed*chandiv)/(maxiSettings::sampleRate/samples.getSampleRate()));
	maxiType remainder = position - (long) position;
	if ((long) position < samples.getLength())
		output = (maxiType) ((1-remainder) * samples[1+ (long) position] + remainder * samples[2+(long) position])/32767;//linear interpolation
	else 
		output=0;
	return(output);
}

template<class source>
maxiType maxiSampler<source>::play(maxiType speed) {
	maxiType remainder;
	long a,b;
	position=position+((speed*chandiv)/(maxiSettings::sampleRate/samples.getSampleRate()));
	if (speed >=0) {
		
		if ((long) position>= samples.getLength()-1) position=1;
		remainder = position - floor(position);
		if (position+1 < samples.getLength()) {
			a=position+1;
			
		}
		else {
			a=samples.getLength()-1;
		}
		if (position+2<samples.getLength())
		{
		b=position+2;
		}
		else {
		b=samples.getLength()-1;
		}
		
		output = (maxiType) ((1-remainder) * samples[a] + remainder * samples[b])/32767;//linear interpolation
} else {
		if ((long) position<0) position= samples.getLength();
		remainder = position - floor(position);
		if (position-1>=0) {
			a=position-1;
				
			}
			else {
				a=0;
			}
	if (position-2>=0) {
		b=position-2;
			}
			else {
				b=0;
			}
		output = (maxiType) ((-1-remainder) * samples[a] + remainder * samples[b])/32767;//linear interpolation
	}	
	return(output);
}

template<class source>
maxiType maxiSampler<source>::play(maxiType frequency, maxiType start, maxiType end) {
	return play(frequency, start, end, position);
}

template<class source>
maxiType maxiSampler<source>::play(maxiType frequency, maxiType start, maxiType end, maxiType &pos) {
	maxiType remainder;
	if (end>=samples.getLength()) end=samples.getLength()-1;
	long a,b;

	if (frequency >0.) {
		if (pos<start) {
			pos=start;
		}
		
		if ( pos >= end ) pos = start;
		pos += ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = pos - floor(pos);
		long posl = floor(pos);
		if (posl+1 < samples.getLength()) {
			a=posl+1;
			
		}
		else {
			a=posl-1;
		}
		if (posl+2 < samples.getLength()) {
			b=posl+2;
		}
		else {
			b=samples.getLength()-1;
		}

		output = (maxiType) ((1-remainder) * samples[a] +
						   remainder * samples[b])/32767;//linear interpolation
	} else {
		frequency=frequency-(frequency+frequency);
		if ( pos <= start ) pos = end;
		pos -= ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = pos - floor(pos);
		long posl = floor(pos);
		if (posl-1>=0) {
			a=posl-1;
		}
		else {
			a=0;
		}
		if (posl-2>=0) {
			b=posl-2;
		}
		else {
			b=0;
		}		
		output = (maxiType) ((-1-remainder) * samples[a] +
						   remainder * samples[b])/32767;//linear interpolation
		
	}
	
	return(output);
}


//better cubic inerpolation. Cobbled together from various (pd externals, yehar, other places).
template<class source>
maxiType maxiSampler<source>::play4(maxiType frequency, maxiType start, maxiType end) {
	maxiType remainder;
	maxiType a,b,c,d,a1,a2,a3;
	if (frequency >0.) {
		if (position<start) {
			position=start;
		}
		if ( position >= end ) position = start;
		position += ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = position - floor(position);
		if (position>0) {
			a=samples[(int)(floor(position))-1];

		} else {
			a=samples[0];
			
		}
		
		b=samples[(long) position];
		if (position<end-2) {
			c=samples[(long) position+1];

		} else {
			c=samples[0];

		}
		if (position<end-3) {
			d=samples[(long) position+2];

		} else {
			d=samples[0];
		}
		a1 = 0.5f * (c - a);
		a2 = a - 2.5 * b + 2.f * c - 0.5f * d;
		a3 = 0.5f * (d - a) + 1.5f * (b - c);
		output = (maxiType) (((a3 * remainder + a2) * remainder + a1) * remainder + b) / 32767;
		
	} else {
		frequency=frequency-(frequency+frequency);
		if ( position <= start ) position = end;
		position -= ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = position - floor(position);
		if (position>start && position < end-1) {
			a=samples[(long) position+1];
			
		} else {
			a=samples[0];
			
		}
		
		b=samples[(long) position];
		if (position>start) {
			c=samples[(long) position-1];
			
		} else {
			c=samples[0];
			
		}
		if (position>start+1) {
			d=samples[(long) position-2];
			
		} else {
			d=samples[0];
		}
		a1 = 0.5f * (c - a);
		a2 = a - 2.5 * b + 2.f * c - 0.5f * d;
		a3 = 0.5f * (d - a) + 1.5f * (b - c);
		output = (maxiType) (((a3 * remainder + a2) * -remainder + a1) * -remainder + b) / 32767;
		
	}
	
	return(output);
}

template<class source>
maxiType maxiSampler<source>::bufferPlay(unsigned char &bufferin,long length) {
	maxiType remainder;
	short* buffer = (short *)&bufferin;
	position=(position+1);
	remainder = position - (long) position;
	if ((long) position>length) position=0;
	output = (maxiType) ((1-remainder) * buffer[1+ (long) position] + remainder * buffer[2+(long) position])/32767;//linear interpolation
	return(output);
}

template<class source>
maxiType maxiSampler<source>::bufferPlay(unsigned char &bufferin,maxiType speed,long length) {
	maxiType remainder;
	long a,b;
	short* buffer = (short *)&bufferin;
	position=position+((speed*chandiv)/(maxiSettings::sampleRate/samples.getSampleRate()));
	if (speed >=0) {
		
		if ((long) position>=length-1) position=1;
		remainder = position - floor(position);
		if (position+1<length) {
			a=position+1;
			
		}
		else {
			a=length-1;
		}
		if (position+2<length)
		{
			b=position+2;
		}
		else {
			b=length-1;
		}
		
		output = (maxiType) ((1-remainder) * buffer[a] + remainder * buffer[b])/32767;//linear interpolation
	} else {
		if ((long) position<0) position=length;
		remainder = position - floor(position);
		if (position-1>=0) {
			a=position-1;
			
		}
		else {
			a=0;
		}
		if (position-2>=0) {
			b=position-2;
		}
		else {
			b=0;
		}
		output = (maxiType) ((-1-remainder) * buffer[a] + remainder * buffer[b])/32767;//linear interpolation
	}	
	return(output);
}

template<class source>
maxiType maxiSampler<source>::bufferPlay(unsigned char &bufferin,maxiType frequency, maxiType start, maxiType end) {
	maxiType remainder;
	long length=end;
	long a,b;
	short* buffer = (short *)&bufferin;
	if (frequency >0.) {
		if (position<start) {
			position=start;
		}
		
		if ( position >= end ) position = start;
		position += ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = position - floor(position);
		long pos = floor(position);
		if (pos+1<length) {
			a=pos+1;
			
		}
		else {
			a=pos-1;
		}
		if (pos+2<length) {
			b=pos+2;
		}
		else {
			b=length-1;
		}
		
		output = (maxiType) ((1-remainder) * buffer[a] +
						   remainder * buffer[b])/32767;//linear interpolation
	} else {
		frequency=frequency-(frequency+frequency);
		if ( position <= start ) position = end;
		position -= ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = position - floor(position);
		long pos = floor(position);
		if (pos-1>=0) {
			a=pos-1;
		}
		else {
			a=0;
		}
		if (pos-2>=0) {
			b=pos-2;
		}
		else {
			b=0;
		}		
		output = (maxiType) ((-1-remainder) * buffer[a] +
						   remainder * buffer[b])/32767;//linear interpolation
		
	}
	
	return(output);
}

//better cubic inerpolation. Cobbled together from various (pd externals, yehar, other places).
template<class source>
maxiType maxiSampler<source>::bufferPlay4(unsigned char &bufferin,maxiType frequency, maxiType start, maxiType end) {
	maxiType remainder;
	maxiType a,b,c,d,a1,a2,a3;
	short* buffer = (short*)&bufferin;
	if (frequency >0.) {
		if (position<start) {
			position=start;
		}
		if ( position >= end ) position = start;
		position += ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = position - floor(position);
		if (position>0) {
			a=buffer[(int)(floor(position))-1];
			
		} else {
			a=buffer[0];
			
		}
		
		b=buffer[(long) position];
		if (position<end-2) {
			c=buffer[(long) position+1];
			
		} else {
			c=buffer[0];
			
		}
		if (position<end-3) {
			d=buffer[(long) position+2];
			
		} else {
			d=buffer[0];
		}
		a1 = 0.5f * (c - a);
		a2 = a - 2.5 * b + 2.f * c - 0.5f * d;
		a3 = 0.5f * (d - a) + 1.5f * (b - c);
		output = (maxiType) (((a3 * remainder + a2) * remainder + a1) * remainder + b) / 32767;
		
	} else {
		frequency=frequency-(frequency+frequency);
		if ( position <= start ) position = end;
		position -= ((end-start)/(maxiSettings::sampleRate/(frequency*chandiv)));
		remainder = position - floor(position);
		if (position>start && position < end-1) {
			a=buffer[(long) position+1];
			
		} else {
			a=buffer[0];
			
		}
		
		b=buffer[(long) position];
		if (position>start) {
			c=buffer[(long) position-1];
			
		} else {
			c=buffer[0];
			
		}
		if (position>start+1) {
			d=buffer[(long) position-2];
			
		} else {
			d=buffer[0];
		}
		a1 = 0.5f * (c - a);
		a2 = a - 2.5 * b + 2.f * c - 0.5f * d;
		a3 = 0.5f * (d - a) + 1.5f * (b - c);
		output = (maxiType) (((a3 * remainder + a2) * -remainder + a1) * -remainder + b) / 32767;
		
	}
	
	return(output);
}


template<class source>
void maxiSampler<source>::setLength(unsigned long numSamples) {
    samples.setLength(numSamples);
    position=0;
    recordPosition=0;
}

template<class source>
void maxiSampler<source>::clear() {
    samples.clear();
}

template<class source>
void maxiSampler<source>::reset() {
    position=0;
}

template<class source>
void maxiSampler<source>::normalise(float maxLevel) {
    short maxValue = 0;
    for(int i=0; i < samples.getLength(); i++) {
        if (abs(samples[i]) > maxValue) {
            maxValue = abs(samples[i]);
        }
    }
    float scale = 32767.0 * maxLevel / (float) maxValue;
    for(int i=0; i < samples.getLength(); i++) {
        samples[i] = round(scale * (float) samples[i]);
    }
}

template<class source>
void maxiSampler<source>::autoTrim(float alpha, float threshold, bool trimStart, bool trimEnd) {
    int startMarker=0;
    if(trimStart) {
        maxiLagExp<float> startLag(alpha, 0);
        while(startMarker < samples.getLength()) {
            startLag.addSample(abs(samples[startMarker]));
            if (startLag.value() > threshold) {
                break;
            }
            startMarker++;
        }
    }
    int endMarker = samples.getLength()-1;
    if(trimEnd) {
        maxiLagExp<float> endLag(alpha, 0);
        while(endMarker > 0) {
            endLag.addSample(abs(samples[endMarker]));
            if (endLag.value() > threshold) {
                break;
            }
            endMarker--;
        }
    }
    
    cout << "Autotrim: start: " << startMarker << ", end: " << endMarker << endl;
    int newLength = endMarker - startMarker;
    if (newLength > 0) {
        samples.trim(startMarker, endMarker);
        position=0;
        recordPosition=0;
        //envelope the start
        int fadeSize=min((long)100, samples.getLength());
        for(int i=0; i < fadeSize; i++) {
            float factor = i / (float) fadeSize;
            samples[i] = round(samples[i] * factor);
            samples[samples.getLength() - 1 - i] = round(samples[samples.getLength() - 1 - i] * factor);
        }
    }
}

template<class source>
bool maxiSampler<source>::save() {
    return save(myPath);
}

template<class source>
bool maxiSampler<source>::save(string filename)
{
    myPath = filename;
    return samples.save(filename);
}

template<class source>
string maxiSampler<source>::getSummary()
{
    return samples.getSummary();
}

template<class source>
void maxiSampler<source>::loopRecord(maxiType newSample, const bool recordEnabled, const maxiType recordMix, maxiType start, maxiType end) {
    loopRecordLag.addSample(recordEnabled);
    if (recordPosition < start * samples.getLength()) recordPosition = start * samples.getLength();
    if(recordEnabled) {
        maxiType currentSample = samples[(unsigned long)recordPosition] / 32767.0;
        newSample = (recordMix * currentSample) + ((1.0 - recordMix) * newSample);
        newSample *= loopRecordLag.value();
        samples[(unsigned long)recordPosition] = newSample * 32767;
    }
    ++recordPosition;
    if (recordPosition >= end * samples.getLength())
        recordPosition= start * samples.getLength();
}

template<class source>
maxiSampler<source>& maxiSampler<source>::operator=(const maxiSampler<source> &src) {
    if (this == &src)
        return *this;
    position=0;
    recordPosition = 0;
    samples = src.samples;
    return *this;
}


///////////////////////////////////////////////////////////////////////////
// maxiDyn
///////////////////////////////////////////////////////////////////////////

/* OK this compressor and gate are now ready to use. The envelopes, like all the envelopes in this recent update, use stupid algorithms for
 incrementing - consequently a long attack is something like 0.0001 and a long release is like 0.9999.
 Annoyingly, a short attack is 0.1, and a short release is 0.99. I'll sort this out laters */

maxiType maxiDyn::gate(maxiType input, maxiType threshold, long holdtime, maxiType attack, maxiType release) {
	
	if (fabs(input)>threshold && attackphase!=1){ 
		holdcount=0;
		releasephase=0;
		attackphase=1;
		if(amplitude==0) amplitude=0.01;
	}
	
	if (attackphase==1 && amplitude<1) {
		amplitude*=(1+attack);
		output=input*amplitude;
	}
	
	if (amplitude>=1) {
		attackphase=0;
		holdphase=1;
	}
	
	if (holdcount<holdtime && holdphase==1) {
		output=input;
		holdcount++;
	}
	
	if (holdcount==holdtime) {
		holdphase=0;
		releasephase=1;
	}
	
	if (releasephase==1 && amplitude>0.) {
		output=input*(amplitude*=release);
		
	}
	
	return output;
}


maxiType maxiDyn::compressor(maxiType input, maxiType ratio, maxiType threshold, maxiType attack, maxiType release) {
	
	if (fabs(input)>threshold && attackphase!=1){ 
		holdcount=0;
		releasephase=0;
		attackphase=1;
		if(currentRatio==0) currentRatio=ratio;
	}
	
	if (attackphase==1 && currentRatio<ratio-1) {
		currentRatio*=(1+attack);
	}
	
	if (currentRatio>=ratio-1) {
		attackphase=0;
		releasephase=1;
	}
	
	if (releasephase==1 && currentRatio>0.) {
		currentRatio*=release;		
	}
	
	if (input>0.) {
		output = input/(1.+currentRatio);
	} else {
		output = input/(1.+currentRatio);
	}
	
	return output*(1+log(ratio));
}

///////////////////////////////////////////////////////////////////////////
// maxiEnv
///////////////////////////////////////////////////////////////////////////

/* Lots of people struggle with the envelope generators so here's a new easy one.
 It takes mental numbers for attack and release tho. Basically, they're exponentials.
 I'll map them out later so that it's a bit more intuitive */
maxiType maxiEnv::ar(maxiType input, maxiType attack, maxiType release, long holdtime, int trigger) {
	
	if (trigger==1 && attackphase!=1 && holdphase!=1){ 
		holdcount=0;
		releasephase=0;
		attackphase=1;
	}
	
	if (attackphase==1) {
		amplitude+=(1*attack);
		output=input*amplitude;
	}
	
	if (amplitude>=1) {
		amplitude=1;
		attackphase=0;
		holdphase=1;
	}
	
	if (holdcount<holdtime && holdphase==1) {
		output=input;
		holdcount++;
	}
	
	if (holdcount==holdtime && trigger==1) {
		output=input;
	}
	
	if (holdcount==holdtime && trigger!=1) {
		holdphase=0;
		releasephase=1;
	}
	
	if (releasephase==1 && amplitude>0.) {
		output=input*(amplitude*=release);
		
	}
	
	return output;
}

/* and here's a new adsr. It's not bad, very simple to use*/

maxiType maxiEnv::adsr(maxiType input, maxiType attack, maxiType decay, maxiType sustain, maxiType release, long holdtime, int trigger) {
	
	if (trigger==1 && attackphase!=1 && holdphase!=1 && decayphase!=1){ 
		holdcount=0;
		decayphase=0;
		sustainphase=0;
		releasephase=0;
		attackphase=1;
	}
	
	if (attackphase==1) {
		amplitude+=(1*attack);
		output=input*amplitude;
	}
	
	if (amplitude>=1) {
		amplitude=1;
		attackphase=0;
		decayphase=1;
	}
	
	if (decayphase==1) {
		output=input*(amplitude*=decay);	
		if (amplitude<=sustain) {
			decayphase=0;
			holdphase=1;
		}
	}
	
	if (holdcount<holdtime && holdphase==1) {
		output=input*amplitude;
		holdcount++;
	}
	
	if (holdcount==holdtime && trigger==1) {
		output=input*amplitude;
	}
	
	if (holdcount==holdtime && trigger!=1) {
		holdphase=0;
		releasephase=1;
	}
	
	if (releasephase==1 && amplitude>0.) {
		output=input*(amplitude*=release);
		
	}
	
	return output;
}

maxiType convert::mtof(int midinote) {
	
	return mtofarray[midinote];
}

///////////////////////////////////////////////////////////////////////////
// sampleSource
///////////////////////////////////////////////////////////////////////////

bool sampleSource::load(string filename, int channel) {
    cout << "sampleSource: operation not supported\n";
    return false;
}

bool sampleSource::save(string filename) {
    cout << "sampleSource: operation not supported\n";
    return false;
}

void sampleSource::unload() {
    cout << "sampleSource: operation not supported\n";
}

string sampleSource::getSummary() {
    cout << "sampleSource: operation not supported\n";
    return "";
}

long sampleSource::getLength() {
    return 0;
}

void sampleSource::setLength(unsigned long newLength) {
    cout << "sampleSource: operation not supported\n";
}

void sampleSource::clear() {
    cout << "sampleSource: operation not supported\n";
}

void sampleSource::trim(unsigned long start, unsigned long end) {
    cout << "sampleSource: operation not supported\n";
}

int sampleSource::getSampleRate() {
    return 1;
}

///////////////////////////////////////////////////////////////////////////
// memSampleSource
///////////////////////////////////////////////////////////////////////////

bool memSampleSource::load(const string filename, const int channel) {
	bool result;
	ifstream inFile( filename.c_str(), ios::in | ios::binary);
	result = inFile.is_open();
	if (result) {
		bool datafound = false;
		inFile.seekg(4, ios::beg);
		inFile.read( (char*) &myChunkSize, 4 ); // read the ChunkSize
		
		inFile.seekg(16, ios::beg);
		inFile.read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size
		
		//inFile.seekg(20, ios::beg);
		inFile.read( (char*) &myFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM
		
		//inFile.seekg(22, ios::beg);
		inFile.read( (char*) &myChannels, sizeof(short) ); // read the # of channels (1 or 2)
		
		//inFile.seekg(24, ios::beg);
		inFile.read( (char*) &mySampleRate, sizeof(int) ); // read the samplerate
		
		//inFile.seekg(28, ios::beg);
		inFile.read( (char*) &myByteRate, sizeof(int) ); // read the byterate
		
		//inFile.seekg(32, ios::beg);
		inFile.read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign
		
		//inFile.seekg(34, ios::beg);
		inFile.read( (char*) &myBitsPerSample, sizeof(short) ); // read the bitspersample
		
		//ignore any extra chunks
		char chunkID[5]="";
		chunkID[4] = 0;
		int filePos = 36;
		while(!datafound && !inFile.eof()) {
			inFile.seekg(filePos, ios::beg);
			inFile.read((char*) &chunkID, sizeof(char) * 4);
			inFile.seekg(filePos + 4, ios::beg);
			inFile.read( (char*) &myDataSize, sizeof(int) ); // read the size of the data
			filePos += 8;
			if (strcmp(chunkID,"data") == 0) {
				datafound = true;
			}else{
				filePos += myDataSize;
			}
		}
		
		// read the data chunk
        data.resize(myDataSize / 2.0); //data is <short>
		inFile.seekg(filePos, ios::beg);
		inFile.read((char*)(&data[0]), myDataSize);
		inFile.close(); // close the input file
		length=data.size();
        
        //keep one channel, discard the rest
		if (myChannels>1) {
			int pos=0;
			for (int i=channel;i<length;i+=myChannels) {
				data[pos]=data[i];
				pos++;
			}
            trim(0, length/myChannels);
		}
        cout << "Channels: " << myChannels << ", length: " << length << ", loaded channel: " << channel << endl;
	}else {
        cout << "ERROR: Could not load sample." << endl;
        
	}
	
	
	return result;
    
}

bool memSampleSource::save(const string filename) {
    fstream myFile (filename.c_str(), ios::out | ios::binary);
    // write the wav file per the wav file format
    myFile.seekp (0, ios::beg);
    myFile.write ("RIFF", 4);
    myFile.write ((char*) &myChunkSize, 4);
    myFile.write ("WAVE", 4);
    myFile.write ("fmt ", 4);
    myFile.write ((char*) &mySubChunk1Size, 4);
    myFile.write ((char*) &myFormat, 2);
    myFile.write ((char*) &myChannels, 2);
    myFile.write ((char*) &mySampleRate, 4);
    myFile.write ((char*) &myByteRate, 4);
    myFile.write ((char*) &myBlockAlign, 2);
    myFile.write ((char*) &myBitsPerSample, 2);
    myFile.write ("data", 4);
    myFile.write ((char*) &myDataSize, 4);
    myFile.write ((char*) &data[0], myDataSize);
    return true;
}

void memSampleSource::unload() {
    data.resize(0);
}

memSampleSource::~memSampleSource() {
    unload();
}

string memSampleSource::getSummary() {
    stringstream s;
    s << " Format: " << myFormat << "\n Channels: " << myChannels << "\n SampleRate: " << mySampleRate << "\n ByteRate: " << myByteRate << "\n BlockAlign: " << myBlockAlign << "\n BitsPerSample: " << myBitsPerSample << "\n DataSize: " << myDataSize << endl;
    return s.str();
    
}

void memSampleSource::setLength(unsigned long newLength) {
    trim(0, newLength);
}

void memSampleSource::clear() {
    data *= 0;
}

void memSampleSource::trim(unsigned long start, unsigned long end) {
    std::valarray<short> temp(end - start);
    temp = data[slice(start, end, 1)];
    data.resize(temp.size());
    data = temp;
    length = data.size();
}

///////////////////////////////////////////////////////////////////////////
// oggSampleSource
///////////////////////////////////////////////////////////////////////////

#ifdef VORBIS
bool oggSampleSource::load(const string filename, const int channel) {
    bool result;
    int channelx;
    short * oggdata;
    myDataSize = stb_vorbis_decode_filename(const_cast<char*>(filename.c_str()), &channelx, &oggdata);
    result = myDataSize > 0;
    printf("\nchannels = %d\nlength = %d",channelx,myDataSize);
    printf("\n");
    myChannels=(short)channelx;
    length=myDataSize;
    mySampleRate=44100;
    data.resize(length);
    for(int i=0; i < length; i++) data[i] = oggdata[i];
    free(oggdata);
    
    //keep one channel, discard the rest
    if (myChannels>1) {
        int pos=0;
        for (int i=channel;i<length;i+=myChannels) {
            data[pos]=data[i];
            pos++;
        }
        trim(0, length/myChannels);
    }
	return result;
}

bool oggSampleSource::save(const string filename) {
    cout << "Warning: ogg is being saved in .wav format\n";
    memSampleSource::save(filename);
}
#endif

///////////////////////////////////////////////////////////////////////////
// fileSampleSource
///////////////////////////////////////////////////////////////////////////
bool fileSampleSource::load(const string filename, const int channel, int _bufferSize, int _blockSize, int _threadSleepTime){
    bufferSize = _bufferSize;
    blockSize = _blockSize;
    threadSleepTime = _threadSleepTime;
    load(filename, channel);
}

bool fileSampleSource::load(const string _filename, const int _channel) {
    filename = _filename;
	bool result;
	int myChunkSize;
	int	mySubChunk1Size;
	short myFormat;
	int myByteRate;
	short myBlockAlign;
	short myBitsPerSample;
	int	myDataSize;
    channel = _channel;
	inFile.open(filename.c_str(), ios::in | ios::binary);
	result = inFile.is_open();
	if (result) {
		bool datafound = false;
		inFile.seekg(4, ios::beg);
		inFile.read( (char*) &myChunkSize, 4 ); // read the ChunkSize
		inFile.seekg(16, ios::beg);
		inFile.read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size
		inFile.read( (char*) &myFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM
		inFile.read( (char*) &numChannels, sizeof(short) ); // read the # of channels (1 or 2)
		inFile.read( (char*) &mySampleRate, sizeof(int) ); // read the samplerate
		inFile.read( (char*) &myByteRate, sizeof(int) ); // read the byterate
		inFile.read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign
		inFile.read( (char*) &myBitsPerSample, sizeof(short) ); // read the bitspersample

		//ignore any extra chunks
		char chunkID[5]="";
		chunkID[4] = 0;
		filePos = 36;
		while(!datafound && !inFile.eof()) {
			inFile.seekg(filePos, ios::beg);
			inFile.read((char*) &chunkID, sizeof(char) * 4);
			inFile.seekg(filePos + 4, ios::beg);
			inFile.read( (char*) &myDataSize, sizeof(int) ); // read the size of the data
			filePos += 8;
			if (strcmp(chunkID,"data") == 0) {
				datafound = true;
			}else{
				filePos += myDataSize;
			}
		}
		
		// read the data chunk
        length = myDataSize / 2.0;
        fileStartPos = filePos;
        fileLength = myDataSize;
        fileEndPos = fileStartPos + fileLength;
        
        if (bufferSize > length / 2.0) {
            bufferSize = length / 2.0;
        }
        bufferSize -= (bufferSize % 2); //even out the number
        data.resize(bufferSize);
        bufferPos = bufferSize / 2;
        filePos = 0;
        frame.resize(numChannels * bufferSize); //max amount needed
        fileWinEndPos = fileStartPos + ((bufferSize / 2) * 2 * numChannels);
        fileWinStartPos = fileEndPos - ((bufferSize / 2) * 2 * numChannels);
        fileWinCenter = 0;
        fileCenterPos = 0; // in <short> format, single channel
        //fill up the buffer
        //read ahead
        inFile.seekg(fileStartPos, ios::beg);
        for(int i=0; i < bufferSize/2; i++) {
//            inFile.seekg(fileStartPos + (i * numChannels * 2), ios::beg);
            inFile.read((char*)(&frame[0]), numChannels * 2);
            data[bufferPos + i] = frame[channel];
        }
        //read from behind
        inFile.seekg(fileWinStartPos);
        for(int i=0; i < bufferSize/2; i++) {
//            inFile.seekg(fileWinStartPos + (i * numChannels * 2));
            inFile.read((char*)(&frame[0]), numChannels * 2);
            data[i] = frame[channel];
        }
        
        inFile.seekg(fileWinEndPos, ios::beg);

//        blockSize = 1024;
        bufferCenter = bufferPos;
        //start off producer thread
        getPocoThread().setPriority(Poco::Thread::PRIO_LOW);
        startThread(false, false);
        
        cout << "fileSampleSource cued\n";
	}else {
        cout << "ERROR: Could not load sample." << endl;
        
	}
	return result;
}

void fileSampleSource::unload() {
    stopThread();
    inFile.close(); // close the input file    
}

fileSampleSource::~fileSampleSource() {
    unload();
}

void fileSampleSource::threadedFunction() {
    cout << "Thread running\n";
    while(isThreadRunning()) {
//        cout << "Rv: " << diffRv << ", " << "Fwd: " << diffFwd << endl;
        //going forward?
        if (diffFwd >= blockSize && diffFwd > -diffRv) {
            //quantise to blocksize (to prevent paging?)
            int diffRemainder = diffFwd % blockSize;
            int readSize = diffFwd - diffRemainder;
//            inFile.seekg(fileWinEndPos, ios::beg);
            if (fileWinEndPos + (readSize * 2 * numChannels) < fileEndPos) {
                inFile.read((char*)(&frame[0]), numChannels * 2 * readSize);
            }else{
                int diff1=(fileEndPos - fileWinEndPos) / 2 / numChannels;
                int diff2= readSize - diff1;
                inFile.read((char*)(&frame[0]), numChannels * 2 * diff1);
                inFile.seekg(fileStartPos, ios::beg);
                inFile.read((char*)(&frame[diff1 * numChannels * 2]), numChannels * 2 * diff2);
            }
            if(inFile.eof())
                cout << "File read error: eof" << endl;
            if(inFile.fail())
                cout << "File read error: fail" << endl;
            long bufferWinEndPos = bufferCenter + (bufferSize / 2);
            int ch=channel;
            for(int i=0; i < readSize; i++, ch += numChannels) {
                if (bufferWinEndPos >= bufferSize) {
                    bufferWinEndPos -= bufferSize;
                }
                data[bufferWinEndPos] = frame[ch];
                bufferWinEndPos++;
            }
            //sort out positioning
            bufferCenter = maxiMap::wrapUp<long>(bufferCenter + readSize, bufferSize, bufferSize);
            fileWinEndPos = maxiMap::wrapUp<long>(fileWinEndPos + (readSize * 2 * numChannels), fileEndPos, fileLength);
            fileWinStartPos = maxiMap::wrapUp<long>(fileWinStartPos + (readSize * 2 * numChannels), fileEndPos, fileLength);
            fileWinCenter =maxiMap::wrapUp<long>(fileWinCenter + (readSize * 2 * numChannels), fileEndPos, fileLength);
            fileCenterPos = (fileWinCenter - fileStartPos) / 2 / numChannels; //map file center pos to single channel number space
            diffFwd = 0;
            diffRv = 0;            
        }else // in reverse?
            if (diffRv <= -blockSize) {
                //quantise to blocksize (to prevent paging?)
                int diffRemainder = (-diffRv % blockSize);
                int readSize = diffRv + diffRemainder;
                //grab blocksize worth of data from file, wrapping if needed
                long readBlockSize = -readSize * 2 * numChannels;
                long readStartPos = fileWinStartPos - readBlockSize;
                if (readStartPos >= fileStartPos && readStartPos + readBlockSize < fileEndPos) {
                    inFile.seekg(readStartPos, ios::beg);
                    inFile.read((char*)(&frame[0]), readBlockSize);
                }else{
                    readStartPos = fileEndPos - abs(fileStartPos - readStartPos);
                    int diff1=(fileEndPos - readStartPos) / 2 / numChannels;
                    int diff2= -readSize - diff1;
                    inFile.read((char*)(&frame[0]), numChannels * 2 * diff1);
                    inFile.seekg(fileStartPos, ios::beg);
                    inFile.read((char*)(&frame[diff1 * numChannels * 2]), numChannels * 2 * diff2);
                }
                if(inFile.eof())
                    cout << "File read error: eof" << endl;
                if(inFile.fail())
                    cout << "File read error: fail" << endl;
                long bufferWinStartPos = bufferCenter - (bufferSize / 2) - -readSize;
                int ch=channel;
                for(int i=0; i < -readSize; i++, ch += numChannels) {
                    if (bufferWinStartPos < 0) {
                        bufferWinStartPos += bufferSize;
                    }
                    data[bufferWinStartPos] = frame[ch];
                    bufferWinStartPos++;
                }
                //sort out positioning
                bufferCenter = maxiMap::wrapDown<long>(bufferCenter - -readSize, 0, bufferSize);
                fileWinEndPos = maxiMap::wrapDown<long>(fileWinEndPos - (-readSize * 2 * numChannels), fileStartPos, fileLength);
                fileWinStartPos = maxiMap::wrapDown<long>(fileWinStartPos - (-readSize * 2 * numChannels), fileStartPos, fileLength);
                fileWinCenter =maxiMap::wrapDown<long>(fileWinCenter - (-readSize * 2 * numChannels), fileStartPos, fileLength);
                fileCenterPos = (fileWinCenter - fileStartPos) / 2 / numChannels;
                diffFwd = 0;
                diffRv = 0;
        }
        //cout << "Buf center: " << bufferCenter << ", buf pos: " << bufferPos << ", file win start: " << fileWinStartPos << ", file win center: " << fileWinCenter << ", file win end: " << fileWinEndPos << ", file center pos: " << fileCenterPos << endl;
        sleep(threadSleepTime);
    }
}

fileSampleSource& fileSampleSource::operator=(const fileSampleSource &src) {
    load(src.filename, src.channel);
    bufferPos = src.bufferPos;
    data = src.data;
    return *this;
}

inline long fileSampleSource::getLength() {
    return length;
};

inline int fileSampleSource::getSampleRate() {
    return mySampleRate;
}

inline short& fileSampleSource::operator[](const long idx) {
    //map from file position to buffer position
    //making an assumption the position is in the buffer already
    int diff = idx - fileCenterPos;
    //wrapping around?
    if (abs(diff) > bufferSize) {
        //going forward
        if (idx < fileCenterPos) {
            diff = idx + ((fileLength / 2 / numChannels) - fileCenterPos);
        }else{ // in reverse
            diff = -1 * (fileCenterPos + ((fileLength / 2 / numChannels) - idx));
        }
    }
    bufferPos = bufferCenter + diff;
    if (bufferPos < 0) {
        bufferPos += bufferSize;
    }else if (bufferPos >= bufferSize) {
        bufferPos -= bufferSize;
    }
    if (diff < 0) {
        diffRv = min(diff, diffRv);
    }else{
        diffFwd = max(diff, diffFwd);
    }
//    cout << diff << endl;
    return data[bufferPos];
}



///////////////////////////////////////////////////////////////////////////
// maxiDistortion
///////////////////////////////////////////////////////////////////////////

maxiType maxiDistortion::fastatan(maxiType x)
{
    return (x / (1.0 + 0.28 * (x * x)));
}

maxiType maxiDistortion::atanDist(const maxiType in, const maxiType shape) {
    maxiType out;
    out = (1.0 / atan(shape)) * atan(in * shape);
    return out;
}

maxiType maxiDistortion::fastAtanDist(const maxiType in, const maxiType shape) {
    maxiType out;
    out = (1.0 / fastatan(shape)) * fastatan(in * shape);
    return out;
}

///////////////////////////////////////////////////////////////////////////
// maxiFlanger
///////////////////////////////////////////////////////////////////////////

maxiType maxiFlanger::flange(const maxiType input, const unsigned int delay, const maxiType feedback, const maxiType speed, const maxiType depth)
{
    //todo: needs fixing
    maxiType output;
    maxiType lfoVal = lfo.triangle(speed);
    output = dl.dl(input, delay + (lfoVal * depth * delay) + 1, feedback) ;
    maxiType normalise = (1 - fabs(output));
    output *= normalise;
    return (output + input) / 2.0;
}


///////////////////////////////////////////////////////////////////////////
// maxiChorus
///////////////////////////////////////////////////////////////////////////

maxiType maxiChorus::chorus(const maxiType input, const unsigned int delay, const maxiType feedback, const maxiType speed, const maxiType depth)
{
    //this needs fixing
    maxiType output1, output2;
    maxiType lfoVal = lfo.noise();
    lfoVal = lopass.lores(lfoVal, speed, 1.0) * 2.0;
    output1 = dl.dl(input, delay + (lfoVal * depth * delay) + 1, feedback) ;
    output2 = dl2.dl(input, (delay + (lfoVal * depth * delay * 1.02) + 1) * 0.98, feedback * 0.99) ;
    output1 *= (1.0 - fabs(output1));
    output2 *= (1.0 - fabs(output2));
    return (output1 + output2 + input) / 3.0;
}

///////////////////////////////////////////////////////////////////////////
// maxiDCBlocker
///////////////////////////////////////////////////////////////////////////

maxiType maxiDCBlocker::play(maxiType input, maxiType R) {
    ym1 = input - xm1 + R * ym1;
    xm1 = input;
    return ym1;
}

///////////////////////////////////////////////////////////////////////////
// maxiSVF
///////////////////////////////////////////////////////////////////////////

maxiSVF& maxiSVF::setCutoff(maxiType cutoff) {
    setParams(cutoff, res);
    return *this;
}

//from 0 upwards, starts to ring from 2-3ish, cracks a bit around 10
maxiSVF& maxiSVF::setResonance(maxiType q) {
    setParams(freq, q);
    return *this;
}

//run the filter, and get a mixture of lowpass, bandpass, highpass and notch outputs
maxiType maxiSVF::play(maxiType w, maxiType lpmix, maxiType bpmix, maxiType hpmix, maxiType notchmix) {
    maxiType low, band, high, notch;
    maxiType v1z = v1;
    maxiType v2z = v2;
    maxiType v3 = w + v0z - 2.0 * v2z;
    v1 += g1*v3-g2*v1z;
    v2 += g3*v3+g4*v1z;
    v0z = w;
    low = v2;
    band = v1;
    high = w-k*v1-v2;
    notch = w-k*v1;
    return (low * lpmix) + (band * bpmix) + (high * hpmix) + (notch * notchmix);
}

void maxiSVF::setParams(maxiType _freq, maxiType _res) {
    freq = _freq;
    res = _res;
    g = tan(PI * freq / maxiSettings::sampleRate);
    damping = res == 0 ? 0 : 1.0 / res;
    k = damping;
    ginv = g / (1.0 + g * (g + k));
    g1 = ginv;
    g2 = 2.0 * (g + k) * ginv;
    g3 = g * ginv;
    g4 = 2.0 * ginv;
}


//pre instantiation, so the templated code can stay here in the .cpp file
template class maxiSampler<memSampleSource>;
template class maxiSampler<fileSampleSource>;
#ifdef VORBIS
template class maxiSampler<oggSampleSource>;
#endif
