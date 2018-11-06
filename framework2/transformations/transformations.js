/*
 * Student names:
 * Student numbers:
 *
 */

function myTranslate(x, y, z) {
    // Translate by x, y and z.
    var mat = [
               1.0, 0.0, 0.0, 0.0,
               0.0, 1.0, 0.0, 0.0,
               0.0, 0.0, 1.0, 0.0,
               x, y, z, 1.0
              ];

   return mat;
}

function myScale(x, y, z) {
    // Scale by x, y and z.
    var mat = [
               x, 0.0, 0.0, 0.0,
               0.0, y, 0.0, 0.0,
               0.0, 0.0, z, 0.0,
               0.0, 0.0, 0.0, 1.0
              ];

   return mat;
}

function norm(a) {
    var l = Math.sqrt(dot(a, a));

    return vectorDivide(a, l);
}

function crossProduct(a, b) {
    return [a[1]*b[2] - a[2]*b[1], 
            a[2]*b[0] - a[0]*b[2], 
            a[0]*b[1] - a[1]*b[0]];
}

function dot(a, b) {
    var res = 0;
    for (var i = 0; i < 3; i++) {
        res += a[i] * b[i];
    }

    return res;
}

function vectorDivide(a, l) {
    var res = [0,0,0];
    for (var i = 0; i < 3; i++) {
        res[i] = a[i] / l;
    }

    return res;
}

function findNonColinear(a) {
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

function myRotate(angle, x, y, z) {
    // Rotate by angle around [x, y, z]^T.

    //
    // 1. Create the orthonormal basis
    //
    
    var w = norm([x, y, z]);

    var t = findNonColinear(w);

    var cross = crossProduct(t,w);

    var crossLength = Math.sqrt(dot(cross, cross));

    var u = vectorDivide(cross, crossLength);

    var v = crossProduct(w, u);
    
    // 2. Set up the three matrices making up the rotation
    //
    var A = [
            u[0], u[1], u[2],0.0,
            v[0], v[1], v[2],0.0,
            w[0], w[1], w[2],0.0,
            0.0,  0.0,  0.0, 1.0
            ];

    var B = [
             Math.cos(angle),   Math.sin(angle),   0.0, 0.0,
            -Math.sin(angle),   Math.cos(angle),    0.0, 0.0,
             0.0,               0.0,                1.0, 0.0,
             0.0,               0.0,                0.0, 1.0
            ];

    var C = m4.transpose(A);


    var mat = m4.multiply(A, m4.multiply(B, C));
    return mat;
}