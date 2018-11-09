/* Computer Graphics, Assignment 2, Translations, Rotations and Scaling
 *
 * Description ..... Draw teapots that can be interactively rotated with the mouse
 * Created by ...... Daan Kruis
 * Original by ..... Paul Melis
 *
 * Student name .... Dante Niewenhuis, Thomas Bellucci
 * Student email ... th.bellucci@gmail.com & d.niewenhuis@hotmail.com
 * Collegekaart .... 11058595, 11257245
 * Date ............ 9 november 2018
 *
 */

var gl;
var prog;
var width;
var height;

var order;
var rotate_axis;
var rotate;

var x_rotation;
var z_rotation;

var prev_x;
var prev_y;

var teapotVertexBuffer;
var teapotNormalBuffer;
var teapotColorBuffer;
var teapotIndexBuffer;

var axisVertexBuffer;
var axisNormalBuffer;
var axisColorBuffer;
var axisIndexBuffer;

var cMat;
var mvMat;
var pMat;
var nMat;

function GimbalInitGL() {
    // Initialize OpenGl settings.
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.depthFunc(gl.LESS);
    gl.enable(gl.DEPTH_TEST);

    // Add fragment and vertex shaders to program.
    var fragShader = gl.createShader(gl.FRAGMENT_SHADER);
    var vertShader = gl.createShader(gl.VERTEX_SHADER);

    gl.shaderSource(fragShader, document.getElementById("fragShader").text);
    gl.compileShader(fragShader);
    if (!gl.getShaderParameter(fragShader, gl.COMPILE_STATUS)) {
        alert("Error while compiling fragment shader: " +
              gl.getShaderInfoLog(vertShader));
    }

    gl.shaderSource(vertShader, document.getElementById("vertShader").text);
    gl.compileShader(vertShader);
    if (!gl.getShaderParameter(vertShader, gl.COMPILE_STATUS)) {
        alert("Error while compiling vertex shader: " +
              gl.getShaderInfoLog(vertShader));
    }

    prog = gl.createProgram();
    gl.attachShader(prog, vertShader);
    gl.attachShader(prog, fragShader);
    gl.linkProgram(prog);

    if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
        alert('Error while initialising shaders.');
    }

    gl.useProgram(prog);

    // Get the location of the shader attributes and matrices.
    prog.vertexPositionAttribute = gl.getAttribLocation(prog, 'aVertexPosition');
    prog.vertexNormalAttribute = gl.getAttribLocation(prog, 'aVertexNormal');
    prog.vertexColorAttribute = gl.getAttribLocation(prog, 'aVertexColor');
    prog.cMatrixUniform = gl.getUniformLocation(prog, 'uCMatrix');
    prog.pMatrixUniform = gl.getUniformLocation(prog, 'uPMatrix');
    prog.mvMatrixUniform = gl.getUniformLocation(prog, 'uMVMatrix');
    prog.nMatrixUniform = gl.getUniformLocation(prog, 'uNMatrix');

    m4.lookAt([15.0, 12.0, 25.0], [15.0, 0.0, 0.0], [0.0, 1.0, 0.0], cMat);
    m4.inverse(cMat, cMat);


    m4.perspective(90.0 / (width / height)  * (Math.PI / 180),
                   width / height, 0.1, 100.0, pMat);
}

function GimbalInitBuffers() {
    // Initialize vertex, normal, color and indices buffers for the teapot.
    teapotVertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, teapotVertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(teapotVertices), gl.STATIC_DRAW);

    teapotNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, teapotNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(teapotNormals), gl.STATIC_DRAW);

    teapotColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, teapotColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(teapotColors), gl.STATIC_DRAW);

    teapotIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, teapotIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(teapotIndices), gl.STATIC_DRAW);

    // Initialize vertex, normal, color and indices buffers for the axis.
    axisVertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, axisVertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(axisVertices), gl.STATIC_DRAW);

    axisNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, axisNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(axisNormals), gl.STATIC_DRAW);

    axisColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, axisColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(axisColors), gl.STATIC_DRAW);

    axisIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, axisIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(axisIndices), gl.STATIC_DRAW);

    // Unbind the buffers.
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
}

function DrawRotatedTeapot(rotx, roty, rotz) {
    if (order == "XYZ") {
        m4.xRotate(mvMat, rotx * (Math.PI / 180), mvMat);
        m4.yRotate(mvMat, roty * (Math.PI / 180), mvMat);
        m4.zRotate(mvMat, rotz * (Math.PI / 180), mvMat);
    }
    else {
        m4.yRotate(mvMat, roty * (Math.PI / 180), mvMat);
        m4.xRotate(mvMat, rotx * (Math.PI / 180), mvMat);
        m4.zRotate(mvMat, rotz * (Math.PI / 180), mvMat);
    }

    m4.inverse(mvMat, nMat);
    m4.transpose(nMat, nMat);

    // Bind matrices and teapot buffers to shaders.
    gl.uniformMatrix4fv(prog.cMatrixUniform, false, cMat);
    gl.uniformMatrix4fv(prog.pMatrixUniform, false, pMat);
    gl.uniformMatrix4fv(prog.mvMatrixUniform, false, mvMat);
    gl.uniformMatrix4fv(prog.nMatrixUniform, false, nMat);

    gl.bindBuffer(gl.ARRAY_BUFFER, teapotVertexBuffer);
    gl.vertexAttribPointer(prog.vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexPositionAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, teapotNormalBuffer);
    gl.vertexAttribPointer(prog.vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexNormalAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, teapotColorBuffer);
    gl.vertexAttribPointer(prog.vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexColorAttribute);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, teapotIndexBuffer);
    // Draw the teapot.
    gl.drawElements(gl.TRIANGLES, teapotIndices.length, gl.UNSIGNED_SHORT, 0);

    // We don't want to apply lighting to the axis.
    nMat = m4.identity();
    gl.uniformMatrix4fv(prog.nMatrixUniform, false, nMat);

    // Bind matrices and axis buffers to shaders.
    gl.lineWidth(2);
    gl.bindBuffer(gl.ARRAY_BUFFER, axisVertexBuffer);
    gl.vertexAttribPointer(prog.vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexPositionAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, axisNormalBuffer);
    gl.vertexAttribPointer(prog.vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexNormalAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, axisColorBuffer);
    gl.vertexAttribPointer(prog.vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexColorAttribute);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, axisIndexBuffer);
    // Draw axis.
    gl.drawElements(gl.LINES, axisIndices.length, gl.UNSIGNED_SHORT, 0);
    gl.lineWidth(1);
}

function DrawTeapots() {
    mvMat = m4.identity();
    DrawRotatedTeapot(x_rotation, 0.0, z_rotation);

    mvMat = m4.identity();
    // Translate Teapot to 15.0, 0.0, 0.0 (Used to illustrate how to translate).
    m4.translate(mvMat, 15.0, 0.0, 0.0, mvMat);
    DrawRotatedTeapot(x_rotation, 45.0, z_rotation);

    mvMat = m4.identity();
    // Translate Teapot to 30.0, 0.0, 0.0 (Used to illustrate how to translate).
    m4.translate(mvMat, 30.0, 0.0, 0.0, mvMat);
    DrawRotatedTeapot(x_rotation, 90.0, z_rotation);
}

function GimbalDrawGLScene() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.viewport(0, 0, width, height);

    var horizontal = Math.cos(frame * speed) * 2;
    var vertical = Math.abs(Math.sin(frame * speed) * 10) + 1;
    var rotation = Math.cos(frame * speed) * 90 * (Math.PI / 180);
    var stretch = 1 + 0.3 * Math.sin(frame * speed / 2);

    DrawTeapots();
}

function GimbalRenderLoop() {
    if (active == "gimbal") {
        window.setTimeout(GimbalRenderLoop, 1000/60);
        GimbalDrawGLScene();
    }
}

function GimbalHandleInput(event) {
    // Handle keypress events.
    switch(event.key) {
        case '1':
            order = "XYZ"
            console.log("Rotation order is now X, Y, Z");
            break;
        case '2':
            order = "YXZ"
            console.log("Rotation order is now Y, X, Z");
            break;
        case 'r':
            x_rotation = 0.0;
            z_rotation = 0.0;
            rotate_axis = "XZ";
            break;
        case 'x':
            rotate_axis = "X"
            break;
        case 'z':
            rotate_axis = "Z"
            break;
        default:
            console.log("Not supported: " + event.key);
            break;
    }
}

function mouseDown(event) {
    prev_x = event.clientX;
    prev_y = event.clientY;
    rotate = true;
}

function mouseMove(event) {
    if (rotate) {
        if (rotate_axis != "Z") {
            x_rotation += (event.clientY - prev_y) * 0.25;
        }
        if (rotate_axis != "X") {
            z_rotation += (event.clientX - prev_x) * 0.25;
        }
        prev_x = event.clientX;
        prev_y = event.clientY;
    }
}

function mouseUp(event) {
    rotate = false;
}

function GimbalMain() {
    const canvas = document.querySelector("#glCanvas");
    // Initialize the GL context
    gl = canvas.getContext("webgl");

    // Initialization of the required variables.
    width = canvas.width;
    height = canvas.height;

    order = "XYZ";
    rotate_axis = "XZ";
    rotate = false;

    x_rotation = 0.0;
    z_rotation = 0.0;

    prev_x = 0.0;
    prev_y = 0.0;

    teapotVertexBuffer = null;
    teapotNormalBuffer = null;
    teapotColorBuffer = null;
    teapotIndexBuffer = null;

    axisVertexBuffer = null;
    axisNormalBuffer = null;
    axisColorBuffer = null;
    axisIndexBuffer = null;

    cMat = m4.identity();
    mvMat = m4.identity();
    pMat = m4.identity();
    nMat = m4.identity();

    // Only continue if WebGL is available and working
    if (!gl) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
        return;
    }

    // Bind input handles.
    document.onkeypress = GimbalHandleInput;
    document.getElementById("glCanvas").addEventListener("mousedown", mouseDown);
    document.getElementById("glCanvas").addEventListener("mouseup", mouseUp);
    document.getElementById("glCanvas").addEventListener("mousemove", mouseMove);


    GimbalInitGL();
    GimbalInitBuffers();
    GimbalRenderLoop();
}
