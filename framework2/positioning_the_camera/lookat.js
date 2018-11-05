/*
 * Student Names:
 * Student Numbers:
 *
 */

function myLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ) {

    var mat = [
        1.0, 0.0, 0.0, -centerX,
        0.0, 1.0, 0.0, -centerY,
        0.0, 0.0, 1.0, -centerZ,
        0.0, 0.0, 0.0, 1.0
    ];

    return mat;
}
