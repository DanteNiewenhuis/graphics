/*
 * Student name .... Dante Niewenhuis, Thomas Bellucci
 * Student email ... th.bellucci@gmail.com & d.niewenhuis@hotmail.com
 * Collegekaart .... 11058595, 11257245
 * Date ............ 9 november 2018
 */

function myOrtho(left, right, bottom, top, near, far) {
	// Transform cuboid to 2x2x2 cube at the origin.
    var mat = [
        2/(right-left),  0.0,             0.0,           0.0,
        0.0,             2/(top-bottom),  0.0,           0.0,
        0.0,             0.0,             2/(near-far),  0.0,
        -(left+right)/2, -(bottom+top)/2, -(near+far)/2, 1.0
    ];

    return mat;
}
