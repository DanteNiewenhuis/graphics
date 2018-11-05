/*
 * Student names:
 * Student numbers:
 *
 */

function myOrtho(left, right, bottom, top, near, far) {

    Nx = 2;
    Ny = 2;
    Nz = 2;
    var mat = [
        Nx/(right-left),    0.0,                0.0,            -(left+right)/2,
        0.0,                Ny/(top-bottom),    0.0,            -(bottom+top)/2,
        0.0,                0.0,                Nz/(near-far),  -(near+far)/2,
        0.0,                0.0,                0.0,            1.0
    ];

    return mat;
}
