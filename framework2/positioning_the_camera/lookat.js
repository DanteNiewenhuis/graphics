/*
 * Student Names:
 * Student Numbers:
 *
 */

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

function myLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ) {

    var mat = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -eyeX, -eyeY, -eyeZ, 1.0
    ];

    var cz = [eyeX - centerX, eyeY - centerY, eyeZ - centerZ];
    cz = norm(cz);

    var cx = norm(crossProduct([upX, upY, upZ], cz));

    var cy = norm(crossProduct(cz, cx));

    var R = [cx[0], cx[1], cx[2], 0.0,
             cy[0], cy[1], cy[2], 0.0,
             cz[0], cz[1], cz[2], 0.0,
             0.0, 0.0, 0.0, 1.0];

    return m4.multiply(m4.transpose(R), mat);
}
