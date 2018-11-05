/* Computer Graphics, Assignment 2, Translations, Rotations and Scaling
 *
 * Description ..... Draw teapots that can be interactively rotated with the mouse
 * Created by ...... Daan Kruis
 * Original by ..... Jurgen Sturm
 *
 */

var gl;
var prog;
var width;
var height;

var frame;
var useMyLookat;
var doRotate;
var doTranslate;
var doScale;

var cameraRotation;
var cameraHeight;
var cameraRotationSpeed;
var cameraHeightTarget;

var speed;

var cubeVertexBuffer;
var cubeColorBuffer;
var cubeNormalBuffer;
var cubeIndexBuffer;

var cMat;
var mvMat;
var pMat;
var nMat;

function CameraInitGL() {
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

    gl.viewport(0, 0, width, height);

    m4.perspective(45  * (Math.PI / 180), width / height, 0.1, 100.0, pMat);
}

function CameraInitBuffers() {
    // Initialize vertex, normal, color and indices buffers for the cube.
    cubeVertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(cubeVertices), gl.STATIC_DRAW);

    cubeNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(cubeNormals), gl.STATIC_DRAW);

    cubeColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(cubeColors), gl.STATIC_DRAW);

    cubeIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(cubeIndices), gl.STATIC_DRAW);

    // Unbind the buffers.
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

function DrawCube() {
    // We don't want to use lighting, so we won't transform the normal vectors.
    nMat = m4.identity()

    // Bind matrices and teapot buffers to shaders.
    gl.uniformMatrix4fv(prog.cMatrixUniform, false, cMat);
    gl.uniformMatrix4fv(prog.pMatrixUniform, false, pMat);
    gl.uniformMatrix4fv(prog.mvMatrixUniform, false, mvMat);
    gl.uniformMatrix4fv(prog.nMatrixUniform, false, nMat);

    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVertexBuffer);
    gl.vertexAttribPointer(prog.vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexPositionAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, cubeNormalBuffer);
    gl.vertexAttribPointer(prog.vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexNormalAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, cubeColorBuffer);
    gl.vertexAttribPointer(prog.vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexColorAttribute);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
    // Draw the cube.
    gl.drawElements(gl.TRIANGLES, cubeIndices.length, gl.UNSIGNED_SHORT, 0);
}

function CameraDrawGLScene() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Rotate around scene.
    var cameraX = 20.0 * Math.cos(cameraRotation);
    var cameraZ = 20.0 * Math.sin(cameraRotation);

    cMat = m4.identity();
    if (useMyLookat) {
        // Your lookAt function.
        m4.multiply(cMat,
                    myLookAt(cameraX, cameraHeight, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0),
                    cMat);
    }
    else {
        // Library lookAt function.
        m4.lookAt([cameraX, cameraHeight, cameraZ], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0], cMat);
        m4.inverse(cMat, cMat);
    }

    var horizontal = Math.cos(frame * speed) * 2;
    var vertical = Math.abs(Math.sin(frame * speed) * 10) + 1;
    var rotation = Math.cos(frame * speed) * 90 * (Math.PI / 180);
    var stretch = Math.sin(frame * speed / 3) + 1.5;
    if (doTranslate) {
        if (vertical < 2) {
            stretch = vertical / 2;
        }
        else {
            stretch = 1;
        }
    }

    // Draw three transformed cubes.
    mvMat = m4.identity();
    m4.translate(mvMat, 0, -5, 0, mvMat);
    m4.scale(mvMat, 8.0, 0.2, 8.0, mvMat);
    DrawCube();

    mvMat = m4.identity();
    m4.translate(mvMat, 2, -5, 0, mvMat);
    m4.scale(mvMat, 1.2, -0.5, 1.2, mvMat);
    DrawCube();

    mvMat = m4.identity();
    m4.translate(mvMat, -2, -5, 0, mvMat);
    m4.scale(mvMat, 1.2, -0.5, 1.2, mvMat);
    DrawCube();

    // Draw final moving cube.
    mvMat = m4.identity();
    if (doTranslate) {
        m4.translate(mvMat, horizontal, vertical - 5, 0.0, mvMat);
    }
    if (doRotate) {
        m4.axisRotate(mvMat, [0, 1, 0], rotation, mvMat);
    }
    if (doScale) {
        m4.scale(mvMat, 1 / Math.sqrt(stretch), stretch, 1 / Math.sqrt(stretch), mvMat);
    }
    DrawCube();
    frame++;
    cameraRotation += cameraRotationSpeed;
    cameraHeight = cameraHeight * 0.99 + cameraHeightTarget * 0.01;
}

function CameraRenderLoop() {
    if (active == "camera") {
        window.setTimeout(CameraRenderLoop, 1000/60);
        CameraDrawGLScene();
    }
}

function CameraHandleInput(event) {
    // Handle keypress events.
    switch(event.key) {
        case ' ':
            useMyLookat = !useMyLookat;
            break;
        case 'r':
            doRotate = !doRotate;
            break;
        case 't':
            doTranslate = !doTranslate;
            break;
        case 's':
            doScale = !doScale;
            break;
        case '+':
        case '=': // For when shift is not used.
            speed *= 1.1;
            frame = 0;
            break;
        case '-':
            speed /= 1.1;
            frame = 0;
            break;
        case '<':
        case ',': // For when shift is not used.
            cameraRotationSpeed -= 0.001;
            break;
        case '>':
        case '.': // For when shift is not used.
            cameraRotationSpeed += 0.001;
            break;
        case '[':
            cameraHeightTarget += 1;
            break;
        case ']':
            cameraHeightTarget -= 1;
            break;
        default:
            console.log("Not supported: " + event.key);
            break;
    }
}

function CameraMain() {
    const canvas = document.querySelector("#glCanvas");
    // Initialize the GL context
    gl = canvas.getContext("webgl");

    width = canvas.width;
    height = canvas.height;

    // Initialization of required variables.
    frame = 0;
    useMyLookat = false;
    doRotate = true;
    doTranslate = true;
    doScale = true;

    cameraRotation = 0.2;
    cameraHeight = 100;
    cameraRotationSpeed = 0.007;
    cameraHeightTarget = 3;

    speed = 0.01;

    cubeVertexBuffer = null;
    cubeNormalBuffer = null;
    cubeColorBuffer = null;
    cubeIndexBuffer = null;

    cMat = m4.identity();
    mvMat = m4.identity();
    pMat = m4.identity();
    nMat = m4.identity();

    // Only continue if WebGL is available and working
    if (!gl) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
        return;
    }

    document.onkeypress = CameraHandleInput;

    CameraInitGL();
    CameraInitBuffers();
    CameraRenderLoop();
}
