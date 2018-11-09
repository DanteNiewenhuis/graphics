/*
 * Student name .... Dante Niewenhuis, Thomas Bellucci
 * Student email ... th.bellucci@gmail.com & d.niewenhuis@hotmail.com
 * Collegekaart .... 11058595, 11257245
 * Date ............ 9 november 2018
 */

function norm(a) {
    // Normalizes a vector to unit length.
    var l = Math.sqrt(dot(a, a));
    return vectorDivide(a, l);
}

function crossProduct(a, b) {
    // Calculates the cross-product between two vectors.
    return [a[1]*b[2] - a[2]*b[1], 
            a[2]*b[0] - a[0]*b[2], 
            a[0]*b[1] - a[1]*b[0]];
}

function dot(a, b) {
    // Calculates the dot-product between two vectors.
    var res = 0;
    for (var i = 0; i < 3; i++) {
        res += a[i] * b[i];
    }
    return res;
}

function vectorDivide(a, l) {
    // Multiplies vector with 1 / a.
    var res = [0,0,0];
    for (var i = 0; i < 3; i++) {
        res[i] = a[i] / l;
    }
    return res;
}

function findNonColinear(a) {
    // Calculates a non-colinear for the vector a.
    var res = [a[0], a[1], a[2]];
    var smallestIndex = 0;
    var smallestValue = res[0];
    for (var i = 1; i < 3; i++) {
        if (res[i] < smallestValue) {
            smallestValue = res[i];
            smallestIndex = i;
        }
    }
    res[smallestIndex] = 1;
    return res;
}

function myLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ) {

    // Translates camera to origin.
    var mat = [
        1.0,   0.0,   0.0,   0.0,
        0.0,   1.0,   0.0,   0.0,
        0.0,   0.0,   1.0,   0.0,
        -eyeX, -eyeY, -eyeZ, 1.0
    ];

    // Setting up orthonormal basis.
    var cz = [eyeX - centerX, eyeY - centerY, eyeZ - centerZ];
    cz = norm(cz);

    var cx = norm(crossProduct([upX, upY, upZ], cz));

    var cy = norm(crossProduct(cz, cx));

    // Create matrix that transforms world to camera coordinates.
    var R = [cx[0], cx[1], cx[2], 0.0,
             cy[0], cy[1], cy[2], 0.0,
             cz[0], cz[1], cz[2], 0.0,
             0.0, 0.0, 0.0, 1.0];
			 
    // Combine translation and rotation.
    return m4.multiply(m4.transpose(R), mat);
}
